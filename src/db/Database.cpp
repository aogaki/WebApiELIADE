
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/json.hpp>
#include <iostream>
#include <mongocxx/client.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/options/insert.hpp>

#include "Database.hpp"
#include "oatpp/core/data/stream/BufferStream.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"

using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_document;

namespace db
{
Database::Database()
    : fEliadePool(std::make_shared<mongocxx::pool>(
          mongocxx::uri("mongodb://172.18.4.56/ELIADE")))
{
}

oatpp::Object<RunLogDto> Database::GetLastRun(
    oatpp::data::mapping::type::String expName, std::string collectionName)
{
  auto conn = fEliadePool->acquire();
  auto collection = (*conn)["ELIADE"][collectionName];

  auto sortOpt = mongocxx::options::find{};
  sortOpt.limit(1);
  auto order = bsoncxx::builder::stream::document{}
               << "start" << -1 << bsoncxx::builder::stream::finalize;
  sortOpt.sort(order.view());
  auto key = bsoncxx::builder::stream::document{}
             << "expName" << expName->std_str()
             << bsoncxx::builder::stream::finalize;
  auto doc = collection.find_one({key}, sortOpt);

  auto dto = RunLogDto::createShared();
  if (doc) {
    auto json = bsoncxx::to_json(doc->view());
    oatpp::parser::json::mapping::ObjectMapper objMapper;
    dto = objMapper.readFromString<oatpp::Object<RunLogDto>>(json.c_str());
  }

  return dto;
}

oatpp::Object<RunLogDto> Database::PostNewRun(oatpp::Object<RunLogDto> dto,
                                              std::string collectionName)
{
  oatpp::parser::json::mapping::ObjectMapper objMapper;
  auto json = objMapper.writeToString(dto);
  auto doc = bsoncxx::from_json(json->std_str());

  auto conn = fEliadePool->acquire();
  auto collection = (*conn)["ELIADE"][collectionName];
  auto result = collection.insert_one(doc.view());
  dto->id = result->inserted_id().get_oid().value.to_string().c_str();

  return dto;
}

oatpp::Object<RunLogDto> Database::PostUpdateRun(oatpp::Object<RunLogDto> dto,
                                                 std::string collectionName)
{
  auto conn = fEliadePool->acquire();
  auto collection = (*conn)["ELIADE"][collectionName];
  try {
    // Check id
    auto id = bsoncxx::oid(dto->id->std_str());
    auto cursor = collection.find(document{} << "_id" << id << finalize);
    auto nDoc = std::distance(cursor.begin(), cursor.end());
    if (nDoc != 1) throw bsoncxx::exception();

    // Update the document in DB
    collection.update_one(
        document{} << "_id" << id << finalize,
        document{} << "$set" << open_document << "runNumber" << dto->runNumber
                   << "start" << dto->start << "stop" << dto->stop << "expName"
                   << dto->expName->std_str() << "comment"
                   << dto->comment->std_str() << "dump" << dto->dump
                   << "dataWriting" << dto->dataWriting << close_document
                   << finalize);
  } catch (bsoncxx::exception e) {  // This should be for any cases?
    // There is no same id in the DB. or multiple(logically this is not happen)
    // Make new document into DB
    bsoncxx::builder::stream::document buf{};
    buf << "runNumber" << dto->runNumber << "start" << dto->start << "stop"
        << dto->stop << "expName" << dto->expName->std_str() << "comment"
        << dto->comment->std_str() << "dump" << dto->dump << "dataWriting"
        << dto->dataWriting;
    auto result = collection.insert_one(buf.view());
    dto->id = result->inserted_id().get_oid().value.to_string().c_str();
    buf.clear();
  }

  return dto;
}

std::string Database::PostEveRate(oatpp::Object<EveRateDto> dto)
{
  oatpp::parser::json::mapping::ObjectMapper objMapper;
  auto json = objMapper.writeToString(dto);
  auto doc = bsoncxx::from_json(json->std_str());

  auto conn = fEliadePool->acquire();
  auto collection = (*conn)["ELIADE"]["EventRate"];
  auto result = collection.insert_one(doc.view());

  return "OK";
}

oatpp::Object<EveRateDto> Database::GetEveRate(std::string expName)
{
  auto conn = fEliadePool->acquire();
  auto collection = (*conn)["ELIADE"]["EventRate"];

  auto sortOpt = mongocxx::options::find{};
  sortOpt.limit(1);
  auto order = bsoncxx::builder::stream::document{}
               << "start" << -1 << bsoncxx::builder::stream::finalize;
  sortOpt.sort(order.view());
  auto key = bsoncxx::builder::stream::document{}
             << "expName" << expName << bsoncxx::builder::stream::finalize;
  auto doc = collection.find_one({key}, sortOpt);

  auto dto = EveRateDto::createShared();
  if (doc) {
    auto json = bsoncxx::to_json(doc->view());
    oatpp::parser::json::mapping::ObjectMapper objMapper;
    dto = objMapper.readFromString<oatpp::Object<EveRateDto>>(json.c_str());
  }

  return dto;
}

oatpp::Object<ExpDto> Database::GetDigiPar()
{
  std::string dbName = "ELIADE";
  auto conn = fEliadePool->acquire();
  auto collection = (*conn)[dbName]["Digitizer"];

  auto opts = mongocxx::options::find{};
  opts.limit(1);
  auto order = bsoncxx::builder::stream::document{}
               << "Time" << -1 << bsoncxx::builder::stream::finalize;
  opts.sort(order.view());
  auto doc = collection.find_one({}, opts);

  auto dto = ExpDto::createShared();
  if (doc) {
    dto->_id = doc->view()["_id"].get_oid().value.to_string().c_str();
    dto->Name = doc->view()["Name"].get_utf8().value.to_string().c_str();
    dto->Time = doc->view()["Time"].get_int64().value;

    auto digitizersArray = doc->view()["Digitizers"].get_array().value;
    for (auto it = digitizersArray.begin(); it != digitizersArray.end(); it++) {
      auto view = (*it).get_document().view();

      auto mapper = oatpp::parser::json::mapping::ObjectMapper::createShared();
      auto obj = mapper->readFromString<oatpp::Object<DigitizerDto>>(
          bsoncxx::to_json(view).c_str());

      dto->Digitizers->push_back(obj);
    }
  }

  return dto;
}

}  // namespace db
