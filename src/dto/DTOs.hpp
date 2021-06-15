
#ifndef example_oatpp_mongo_DTOs_hpp
#define example_oatpp_mongo_DTOs_hpp

#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class UserDto : public oatpp::DTO
{
  DTO_INIT(UserDto, DTO)

  DTO_FIELD(String, username);
  DTO_FIELD(Boolean, active);
  DTO_FIELD(String, role);
};

class RunLogDto : public oatpp::DTO
{
  DTO_INIT(RunLogDto, DTO)

  DTO_FIELD(String, id);
  DTO_FIELD(Int32, runNumber);
  DTO_FIELD(Int64, start);
  DTO_FIELD(Int64, stop);
  DTO_FIELD(String, comment);
};

#include OATPP_CODEGEN_END(DTO)

#endif /* example_oatpp_mongo_DTOs_hpp */
