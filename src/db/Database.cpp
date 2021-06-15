
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/json.hpp>
#include <iostream>
#include <mongocxx/client.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/options/insert.hpp>

#include "Database.hpp"
#include "db/Model.hpp"
#include "oatpp/core/data/stream/BufferStream.hpp"

namespace db
{
Database::Database(const mongocxx::uri &uri, const std::string &dbName,
                   const std::string &collectionName)
    : m_pool(std::make_shared<mongocxx::pool>(uri)),
      fEliadePool(std::make_shared<mongocxx::pool>(
          mongocxx::uri("mongodb://daq:nim2camac@172.18.4.56/ELIADE"))),
      m_databaseName(dbName),
      m_collectionName(collectionName)
{
}

oatpp::Object<User> Database::userFromDto(const oatpp::Object<UserDto> &dto)
{
  auto user = User::createShared();
  user->_id = dto->username;
  user->username = dto->username;
  user->active = dto->active;
  user->role = dto->role;
  return user;
}

oatpp::Object<UserDto> Database::dtoFromUser(const oatpp::Object<User> &user)
{
  auto dto = UserDto::createShared();
  dto->username = user->username;
  dto->active = user->active;
  dto->role = user->role;
  return dto;
}

bsoncxx::document::value Database::createMongoDocument(
    const oatpp::Void &polymorph)
{
  // if you have huge docs, you may want to increase starting BufferOutputStream size.
  // Or you may want to use oatpp::data::stream::ChunkedBuffer instead - for no-copy growth.
  oatpp::data::stream::BufferOutputStream stream;
  m_objectMapper.write(&stream, polymorph);
  bsoncxx::document::view view(stream.getData(), stream.getCurrentPosition());
  return bsoncxx::document::value(view);
}

oatpp::Object<UserDto> Database::createUser(
    const oatpp::Object<UserDto> &userDto)
{
  auto conn = m_pool->acquire();
  auto collection = (*conn)[m_databaseName][m_collectionName];
  collection.insert_one(createMongoDocument(userFromDto(userDto)));
  return userDto;
}

oatpp::Object<UserDto> Database::updateUser(
    const oatpp::Object<UserDto> &userDto)
{
  auto conn = m_pool->acquire();
  auto collection = (*conn)[m_databaseName][m_collectionName];

  collection.update_one(
      createMongoDocument(  // <-- Filter
          oatpp::Fields<oatpp::String>({{"_id", userDto->username}})),
      createMongoDocument(  // <-- Set
          oatpp::Fields<oatpp::Any>({
              // map
              {// pair
               "$set",
               oatpp::Fields<oatpp::Any>(
                   {// you can also define a "strict" DTO for $set operation.
                    {"active", userDto->active},
                    {"role", userDto->role}})}  // pair
          })                                    // map
          ));

  return userDto;
}

oatpp::Object<UserDto> Database::getUser(const oatpp::String &username)
{
  auto conn = m_pool->acquire();
  auto collection = (*conn)[m_databaseName][m_collectionName];

  auto result = collection.find_one(createMongoDocument(  // <-- Filter
      oatpp::Fields<oatpp::String>({{"_id", username}})));

  if (result) {
    auto view = result->view();
    auto bson = oatpp::String((const char *)view.data(), view.length(),
                              false /* to not copy view data */);
    auto user = m_objectMapper.readFromString<oatpp::Object<User>>(bson);
    return dtoFromUser(user);
  }

  return nullptr;
}

oatpp::List<oatpp::Object<UserDto>> Database::getAllUsers()
{
  auto conn = m_pool->acquire();
  auto collection = (*conn)[m_databaseName][m_collectionName];

  auto cursor =
      collection.find(createMongoDocument(oatpp::Fields<oatpp::String>({})));

  oatpp::List<oatpp::Object<UserDto>> list({});

  for (auto view : cursor) {
    auto bson = oatpp::String((const char *)view.data(), view.length(),
                              false /* to not copy view data */);
    auto user = m_objectMapper.readFromString<oatpp::Object<User>>(bson);
    list->push_back(dtoFromUser(user));
  }

  return list;
}

bool Database::deleteUser(const oatpp::String &username)
{
  auto conn = m_pool->acquire();
  auto collection = (*conn)[m_databaseName][m_collectionName];

  auto result = collection.delete_one(createMongoDocument(  // <-- Filter
      oatpp::Fields<oatpp::String>({{"_id", username}})));

  if (result) {
    return result->deleted_count() == 1;
  }
  return false;
}

oatpp::Object<RunLogDto> Database::GetLastRun()
{
  auto conn = fEliadePool->acquire();
  auto collection = (*conn)["ELIADE"]["TestRunLog"];

  auto opts = mongocxx::options::find{};
  opts.limit(1);
  auto order = bsoncxx::builder::stream::document{}
               << "start" << -1 << bsoncxx::builder::stream::finalize;
  opts.sort(order.view());
  auto doc = collection.find_one({}, opts);

  if (doc) {
    auto view = doc->view();
    // std::cout << bsoncxx::to_json(view) << std::endl;
    auto dto = RunLogDto::createShared();
    dto->id = view["_id"].get_oid().value.to_string().c_str();
    dto->runNumber = view["runNumber"].get_int32().value;
    dto->start = view["start"].get_int64().value;
    dto->stop = view["stop"].get_int64().value;
    dto->comment = view["comment"].get_utf8().value.to_string().c_str();

    return dto;
  }

  return nullptr;
}

oatpp::Object<RunLogDto> Database::PostStartTime(oatpp::Object<RunLogDto> dto)
{
  auto conn = fEliadePool->acquire();
  auto collection = (*conn)["ELIADE"]["TestRunLog"];

  bsoncxx::builder::stream::document buf{};
  buf << "runNumber" << dto->runNumber << "start" << dto->start << "stop"
      << dto->stop << "comment" << dto->comment->std_str();
  auto result = collection.insert_one(buf.view());
  dto->id = result->inserted_id().get_oid().value.to_string().c_str();
  buf.clear();

  return dto;
}

oatpp::Object<RunLogDto> Database::PostStopTime(oatpp::Object<RunLogDto> dto)
{
  using bsoncxx::builder::stream::close_document;
  using bsoncxx::builder::stream::document;
  using bsoncxx::builder::stream::finalize;
  using bsoncxx::builder::stream::open_document;

  auto conn = fEliadePool->acquire();
  auto collection = (*conn)["ELIADE"]["TestRunLog"];
  try {
    // Check id
    auto id = bsoncxx::oid(dto->id->std_str());
    auto cursor = collection.find(document{} << "_id" << id << finalize);
    auto nDoc = std::distance(cursor.begin(), cursor.end());
    if (nDoc != 1) throw bsoncxx::exception();

    // Update the document in DB
    collection.update_one(document{} << "_id" << id << finalize,
                          document{} << "$set" << open_document << "stop"
                                     << dto->stop << close_document
                                     << finalize);
  } catch (bsoncxx::exception e) {  // This should be for any cases?
    // There is no same id in the DB. or multiple(logically this is not happen)
    // Make new document into DB
    bsoncxx::builder::stream::document buf{};
    buf << "runNumber" << dto->runNumber << "start" << dto->start << "stop"
        << dto->stop << "comment" << dto->comment->std_str();
    auto result = collection.insert_one(buf.view());
    dto->id = result->inserted_id().get_oid().value.to_string().c_str();
    buf.clear();
  }

  return dto;
}

}  // namespace db
