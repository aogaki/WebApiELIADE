
#ifndef example_oatpp_mongo_UserController_hpp
#define example_oatpp_mongo_UserController_hpp

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

  ENDPOINT_INFO(getLastRun)
  {
    info->summary = "Get the current/last run information";
    info->addResponse<Object<RunLogDto>>(Status::CODE_200, "application/json");
  }
  ADD_CORS(getLastRun)
  ENDPOINT("GET", "/ELIADE/GetLastRun", getLastRun)
  {
    auto dto = m_database->GetLastRun();
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
    auto echo = m_database->PostStartTime(dto);
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
    auto echo = m_database->PostStopTime(dto);
    auto response = createDtoResponse(Status::CODE_200, echo);
    return response;
  }

  ENDPOINT_INFO(getLastRunTest)
  {
    info->summary = "Get the current/last run information";
    info->addResponse<Object<RunLogDto>>(Status::CODE_200, "application/json");
  }
  ADD_CORS(getLastRunTest)
  ENDPOINT("GET", "/ELIADETest/GetLastRun", getLastRunTest)
  {
    auto dto = m_database->GetLastRun("TestRunLog");
    auto response = createDtoResponse(Status::CODE_200, dto);
    return response;
  }

  ENDPOINT_INFO(postStasrtTimeTest)
  {
    info->summary =
        "Post start time with run number.  Creating document in the DB.";
    info->addConsumes<Object<RunLogDto>>("application/json");
    info->addResponse<Object<RunLogDto>>(Status::CODE_200, "application/json");
  }
  ADD_CORS(postStasrtTimeTest)
  ENDPOINT("POST", "/ELIADETest/PostStartTime", postStasrtTimeTest,
           BODY_DTO(Object<RunLogDto>, dto))
  {
    auto echo = m_database->PostStartTime(dto, "TestRunLog");
    auto response = createDtoResponse(Status::CODE_200, echo);
    return response;
  }

  ENDPOINT_INFO(postStopTimeTest)
  {
    info->summary = "Post stop time.  Creating or update document in the DB.";
    info->addConsumes<Object<RunLogDto>>("application/json");
    info->addResponse<Object<RunLogDto>>(Status::CODE_200, "application/json");
  }
  ADD_CORS(postStopTimeTest)
  ENDPOINT("POST", "/ELIADETest/PostStopTime", postStopTimeTest,
           BODY_DTO(Object<RunLogDto>, dto))
  {
    auto echo = m_database->PostStopTime(dto, "TestRunLog");
    auto response = createDtoResponse(Status::CODE_200, echo);
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

  ENDPOINT_INFO(getLastRunServerTest)
  {
    info->summary = "Get the current/last run information";
    info->addResponse<Object<RunLogDto>>(Status::CODE_200, "application/json");

    info->queryParams.add<String>("expName").description = "Experiment name";
    info->queryParams["expName"].required = false;
  }
  ADD_CORS(getLastRunServerTest)
  ENDPOINT("GET", "/ServerTest/GetLastRun", getLastRunServerTest,
           REQUEST(std::shared_ptr<IncomingRequest>, request))
  {
    auto expName = request->getQueryParameter("expName", "");
    auto dto = m_database->GetLastRun(expName, "ServerRunLog");
    auto response = createDtoResponse(Status::CODE_200, dto);
    return response;
  }

  ENDPOINT_INFO(postStasrtTimeServerTest)
  {
    info->summary =
        "Post start time with run number.  Creating document in the DB.";
    info->addConsumes<Object<RunLogDto>>("application/json");
    info->addResponse<Object<RunLogDto>>(Status::CODE_200, "application/json");

    info->queryParams.add<String>("expName").description = "Experiment name";
    info->queryParams["expName"].required = false;
  }
  ADD_CORS(postStasrtTimeServerTest)
  ENDPOINT("POST", "/ServerTest/PostStartTime", postStasrtTimeServerTest,
           BODY_DTO(Object<RunLogDto>, dto))
  {
    auto echo = m_database->PostStartTime(dto, "ServerRunLog");
    auto response = createDtoResponse(Status::CODE_200, echo);
    return response;
  }

  ENDPOINT_INFO(postStopTimeServerTest)
  {
    info->summary = "Post stop time.  Creating or update document in the DB.";
    info->addConsumes<Object<RunLogDto>>("application/json");
    info->addResponse<Object<RunLogDto>>(Status::CODE_200, "application/json");
  }
  ADD_CORS(postStopTimeServerTest)
  ENDPOINT("POST", "/ServerTest/PostStopTime", postStopTimeServerTest,
           BODY_DTO(Object<RunLogDto>, dto))
  {
    auto echo = m_database->PostStopTime(dto, "ServerRunLog");
    auto response = createDtoResponse(Status::CODE_200, echo);
    return response;
  }
};

#include OATPP_CODEGEN_END(ApiController)

#endif /* example_oatpp_mongo_UserController_hpp */
