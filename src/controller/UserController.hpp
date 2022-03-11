
#ifndef example_oatpp_mongo_UserController_hpp
#define example_oatpp_mongo_UserController_hpp

#include <iostream>

#include "db/Database.hpp"
#include "dto/DTOs.hpp"
#include "oatpp-swagger/Types.hpp"
#include "oatpp/core/data/stream/BufferStream.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/web/server/api/ApiController.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

class UserController : public oatpp::web::server::api::ApiController
{
 private:
  OATPP_COMPONENT(std::shared_ptr<db::Database>, m_database);

 public:
  UserController(const std::shared_ptr<ObjectMapper> &objectMapper)
      : oatpp::web::server::api::ApiController(objectMapper)
  {
  }

 public:
  static std::shared_ptr<UserController> createShared(
      OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
  {
    return std::make_shared<UserController>(objectMapper);
  }

  ENDPOINT("GET", "/", root)
  {
    const String html =
        "<html lang='en'>"
        "<head>"
        "<meta charset=utf-8/>"
        "</head>"
        "<body>"
        "<p>Web API for some experiments</p>"
        "<a href='swagger/ui'>Checkout Swagger-UI page</a>"
        "</body>"
        "</html>";
    auto response = createResponse(Status::CODE_200, html);
    response->putHeader(Header::CONTENT_TYPE, "text/html");
    return response;
  }

  ENDPOINT_INFO(getDigiPar)
  {
    info->summary = "Get the digitizer parameters";
    info->addResponse<Object<ExpDto>>(Status::CODE_200, "application/json");
  }
  ADD_CORS(getDigiPar)
  ENDPOINT("GET", "/ELIADE/GetDigiPar", getDigiPar)
  {
    auto dto = m_database->GetDigiPar();
    auto response = createDtoResponse(Status::CODE_200, dto);
    return response;
  }

  ENDPOINT_INFO(getRun)
  {
    info->summary = "Get the current/last run information";
    info->addResponse<Object<RunLogDto>>(Status::CODE_200, "application/json");
  }
  ADD_CORS(getRun)
  ENDPOINT("GET", "/ELIADE/GetLastRun/{expName}", getRun, PATH(String, expName))
  {
    auto dto = m_database->GetRunList(expName, "ServerRunLog", 1);
    auto response = createDtoResponse(Status::CODE_200, dto);
    return response;
  }

  ENDPOINT_INFO(getRunList)
  {
    info->summary = "Get the run information list";
    info->addResponse<List<Object<RunLogDto>>>(Status::CODE_200,
                                               "application/json");
  }
  ADD_CORS(getRunList)
  ENDPOINT("GET", "/ELIADE/GetRunList/{expName}", getRunList,
           PATH(String, expName))
  {
    auto dto = m_database->GetRunList(expName, "ServerRunLog", 20);
    auto response = createDtoResponse(Status::CODE_200, dto);
    return response;
  }

  ENDPOINT_INFO(getAllRunList)
  {
    info->summary = "Get the run information list";
    info->addResponse<List<Object<RunLogDto>>>(Status::CODE_200,
                                               "application/json");
  }
  ADD_CORS(getAllRunList)
  ENDPOINT("GET", "/ELIADE/GetAllRunList/{expName}", getAllRunList,
           PATH(String, expName))
  {
    auto dto = m_database->GetRunList(expName, "ServerRunLog", 0);
    auto response = createDtoResponse(Status::CODE_200, dto);
    return response;
  }

  ENDPOINT_INFO(postStasrtTime)
  {
    info->summary =
        "Post start time with run number.  Creating document in the DB.";
    info->addConsumes<Object<RunLogDto>>("application/json");
    info->addResponse<Object<RunLogDto>>(Status::CODE_200, "application/json");
  }
  ADD_CORS(postStasrtTime)
  ENDPOINT("POST", "/ELIADE/PostStartTime", postStasrtTime,
           BODY_DTO(Object<RunLogDto>, dto))
  {
    auto echo = m_database->PostNewRun(dto, "ServerRunLog");
    auto response = createDtoResponse(Status::CODE_200, echo);
    return response;
  }

  ENDPOINT_INFO(postStopTime)
  {
    info->summary = "Post stop time.  Creating or update document in the DB.";
    info->addConsumes<Object<RunLogDto>>("application/json");
    info->addResponse<Object<RunLogDto>>(Status::CODE_200, "application/json");
  }
  ADD_CORS(postStopTime)
  ENDPOINT("POST", "/ELIADE/PostStopTime", postStopTime,
           BODY_DTO(Object<RunLogDto>, dto))
  {
    auto echo = m_database->PostUpdateRun(dto, "ServerRunLog");
    auto response = createDtoResponse(Status::CODE_200, echo);
    return response;
  }

  ENDPOINT_INFO(enableDump)
  {
    info->summary = "Force ASCII dump";
    info->addConsumes<Object<RunLogDto>>("application/json");
    info->addResponse<Object<RunLogDto>>(Status::CODE_200, "application/json");
  }
  ADD_CORS(enableDump)
  ENDPOINT("POST", "/ELIADE/EnableDump", enableDump,
           BODY_DTO(Object<RunLogDto>, dto))
  {
    auto echo = m_database->PostUpdateRun(dto, "ServerRunLog");
    auto response = createDtoResponse(Status::CODE_200, echo);
    return response;
  }

  ENDPOINT_INFO(checkDump)
  {
    info->summary = "Check ASCII dump or not.  Disable dump after checking.";
    info->addResponse<String>(Status::CODE_200, "text/plain");
  }
  ADD_CORS(checkDump)
  ENDPOINT("GET", "/ELIADE/CheckDump/{expName}", checkDump,
           PATH(String, expName))
  {
    auto dto = m_database->GetRunList(expName, "ServerRunLog");
    // std::string message = std::to_string(dto->dump);
    std::string message = "false";
    if (dto[0]->dump) {
      message = "true";
      dto[0]->dump = false;
      m_database->PostUpdateRun(dto[0], "ServerRunLog");
    }

    auto response = createResponse(Status::CODE_200, message.c_str());
    return response;
  }
};

#include OATPP_CODEGEN_END(ApiController)

#endif /* example_oatpp_mongo_UserController_hpp */
