
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

  // ENDPOINT_INFO(createUser)
  // {
  //   info->summary = "Create new User";
  //   info->addConsumes<Object<UserDto>>("application/json");
  //   info->addResponse<Object<UserDto>>(Status::CODE_200, "application/json");
  // }
  // ENDPOINT("POST", "demo/api/users", createUser,
  //          BODY_DTO(Object<UserDto>, userDto))
  // {
  //   return createDtoResponse(Status::CODE_200, m_database->createUser(userDto));
  // }
  //
  // ENDPOINT_INFO(putUser)
  // {
  //   // general
  //   info->summary = "Update User by username";
  //   info->addConsumes<Object<UserDto>>("application/json");
  //   info->addResponse<Object<UserDto>>(Status::CODE_200, "application/json");
  //   info->addResponse<String>(Status::CODE_404, "text/plain");
  //   // params specific
  //   info->pathParams["username"].description = "username/login";
  // }
  // ENDPOINT("PUT", "demo/api/users/{username}", putUser, PATH(String, username),
  //          BODY_DTO(Object<UserDto>, userDto))
  // {
  //   userDto->username = username;
  //   return createDtoResponse(Status::CODE_200, m_database->updateUser(userDto));
  // }
  //
  // ENDPOINT_INFO(getUser)
  // {
  //   // general
  //   info->summary = "Get one User by username";
  //   info->addResponse<Object<UserDto>>(Status::CODE_200, "application/json");
  //   info->addResponse<String>(Status::CODE_404, "text/plain");
  //   // params specific
  //   info->pathParams["username"].description = "username/login";
  // }
  // ENDPOINT("GET", "demo/api/users/{username}", getUser, PATH(String, username))
  // {
  //   auto user = m_database->getUser(username);
  //   OATPP_ASSERT_HTTP(user, Status::CODE_404, "User not found");
  //   return createDtoResponse(Status::CODE_200, user);
  // }
  //
  // ENDPOINT_INFO(getAllUsers)
  // {
  //   info->summary = "get all stored users";
  //   info->addResponse<List<Object<UserDto>>>(Status::CODE_200,
  //                                            "application/json");
  // }
  // ENDPOINT("GET", "demo/api/users", getAllUsers)
  // {
  //   return createDtoResponse(Status::CODE_200, m_database->getAllUsers());
  // }
  //
  // ENDPOINT_INFO(deleteUser)
  // {
  //   // general
  //   info->summary = "Delete User by username";
  //   info->addResponse<String>(Status::CODE_200, "text/plain");
  //   info->addResponse<String>(Status::CODE_404, "text/plain");
  //   // params specific
  //   info->pathParams["username"].description = "username/login";
  // }
  // ENDPOINT("DELETE", "demo/api/users/{username}", deleteUser,
  //          PATH(String, username))
  // {
  //   bool success = m_database->deleteUser(username);
  //   OATPP_ASSERT_HTTP(success, Status::CODE_500,
  //                     "User not deleted. Perhaps no such User in the Database");
  //   return createResponse(Status::CODE_200, "User successfully deleted");
  // }

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
};

#include OATPP_CODEGEN_END(ApiController)

#endif /* example_oatpp_mongo_UserController_hpp */
