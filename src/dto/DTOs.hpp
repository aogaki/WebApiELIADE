#ifndef example_oatpp_mongo_DTOs_hpp
#define example_oatpp_mongo_DTOs_hpp

#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class RunLogDto : public oatpp::DTO
{
  DTO_INIT(RunLogDto, DTO)

  DTO_FIELD(Int32, runNumber);
  DTO_FIELD(Int64, start);
  DTO_FIELD(Int64, stop);
  DTO_FIELD(String, expName);
  DTO_FIELD(String, comment);
};

class DigiParDto : public oatpp::DTO
{
  DTO_INIT(DigiParDto, DTO)

  DTO_FIELD(String, name);
  DTO_FIELD(Int64, time);
  DTO_FIELD(String, parameters);
};

class DigitizerDto : public oatpp::DTO
{
  DTO_INIT(DigitizerDto, DTO)

  DTO_FIELD(Int32, LinkType);
  DTO_FIELD(Int32, LinkNum);
  DTO_FIELD(Int32, ConetNode);
  DTO_FIELD(Int32, VMEBaseAddress);
  DTO_FIELD(Int32, BrdNum);
  DTO_FIELD(Int32, RunSyncMode);
  DTO_FIELD(Int32, StartMode);
  DTO_FIELD(Int32, RunSync);
  DTO_FIELD(Int32, ChMask);
  DTO_FIELD(List<Int32>, DCOffset) = {};
  DTO_FIELD(List<Int32>, Polarity) = {};
  DTO_FIELD(List<Int32>, TrgPolarity) = {};
  DTO_FIELD(List<Int32>, ChargeSens) = {};
  DTO_FIELD(Int32, SWTrgMode);
  DTO_FIELD(Int32, ExtTrgMode);
  DTO_FIELD(Int32, ChSelfTrg);
  DTO_FIELD(Boolean, TrgPropagation);
  DTO_FIELD(List<Int32>, TrgTh) = {};
  DTO_FIELD(List<Int32>, TrgHoldOff) = {};
  DTO_FIELD(List<Int32>, ShortGate) = {};
  DTO_FIELD(List<Int32>, LongGate) = {};
  DTO_FIELD(List<Int32>, CoincWindow) = {};
  DTO_FIELD(List<Int32>, NSBaseLine) = {};
  DTO_FIELD(Int32, PileUpRejection);
  DTO_FIELD(Int32, PURGap);
  DTO_FIELD(List<Int32>, DiscrMode) = {};
  DTO_FIELD(List<Int32>, CFDDelay) = {};
  DTO_FIELD(List<Int32>, CFDFraction) = {};
  DTO_FIELD(List<Int32>, RecordLength) = {};
  DTO_FIELD(List<Int32>, PreTriggerSize) = {};
  DTO_FIELD(Int32, IOLevel);
  DTO_FIELD(Int32, AcqMode);
  DTO_FIELD(Int32, Coincidence);
  DTO_FIELD(Int32, EventBuffering);
  DTO_FIELD(List<Int32>, DynamicRange) = {};
  DTO_FIELD(List<Int32>, TrapPoleZero) = {};
  DTO_FIELD(List<Int32>, TrapFlatTop) = {};
  DTO_FIELD(List<Int32>, TrapRiseTime) = {};
  DTO_FIELD(List<Int32>, PeakingTime) = {};
  DTO_FIELD(List<Int32>, TTFSmoothing) = {};
  DTO_FIELD(List<Int32>, SignalRiseTime) = {};
  DTO_FIELD(List<Int32>, NSPeak) = {};
  DTO_FIELD(List<Int32>, PeakHoldOff) = {};
  DTO_FIELD(List<Int32>, BaseLineHoldOff) = {};
  DTO_FIELD(List<Int32>, ZeroCrossAccWindow) = {};
  DTO_FIELD(List<Int32>, TrgAccWindow) = {};
  DTO_FIELD(List<Int32>, DigitalGain) = {};
  DTO_FIELD(List<Float64>, EneFineGain) = {};
  DTO_FIELD(List<Int32>, Decimation) = {};
};

class ExpDto : public oatpp::DTO
{
  DTO_INIT(ExpDto, DTO)

  DTO_FIELD(String, Name);
  DTO_FIELD(Int64, Time);
  DTO_FIELD(List<Object<DigitizerDto>>, Digitizers) = {};
};

#include OATPP_CODEGEN_END(DTO)

#endif /* example_oatpp_mongo_DTOs_hpp */
