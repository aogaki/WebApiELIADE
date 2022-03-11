
#ifndef example_oatpp_mongo_Database_hpp
#define example_oatpp_mongo_Database_hpp

#include <bsoncxx/document/value.hpp>
#include <mongocxx/pool.hpp>
#include <string>

#include "dto/DTOs.hpp"
#include "oatpp-mongo/bson/mapping/ObjectMapper.hpp"

namespace db
{
class Database
{
 private:
  std::shared_ptr<mongocxx::pool> fEliadePool;
  oatpp::mongo::bson::mapping::ObjectMapper m_objectMapper;

 public:
  Database();

  oatpp::List<oatpp::Object<RunLogDto>> GetRunList(
      oatpp::data::mapping::type::String expName = "",
      std::string collectionName = "RunLog", int listSize = 1);

  oatpp::Object<RunLogDto> PostNewRun(oatpp::Object<RunLogDto> dto,
                                      std::string collectionName = "RunLog");
  oatpp::Object<RunLogDto> PostUpdateRun(oatpp::Object<RunLogDto> dto,
                                         std::string collectionName = "RunLog");

  oatpp::Object<ExpDto> GetDigiPar();
};

}  // namespace db

#endif  //example_oatpp_mongo_Database_hpp
