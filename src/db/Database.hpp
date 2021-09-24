
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

  oatpp::Object<RunLogDto> GetLastRun(
      oatpp::data::mapping::type::String expName = "",
      std::string collectionName = "RunLog");
  oatpp::Object<RunLogDto> PostNewRun(oatpp::Object<RunLogDto> dto,
                                      std::string collectionName = "RunLog");
  oatpp::Object<RunLogDto> PostUpdateRun(oatpp::Object<RunLogDto> dto,
                                         std::string collectionName = "RunLog");

  std::string PostEveRate(oatpp::Object<EveRateDto> dto);
  oatpp::Object<EveRateDto> GetEveRate(std::string expName);

  oatpp::Object<ExpDto> GetDigiPar();
};

}  // namespace db

#endif  //example_oatpp_mongo_Database_hpp
