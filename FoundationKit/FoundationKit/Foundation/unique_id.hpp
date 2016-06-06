
#pragma once
#include <chrono>
#include <sstream>
#include <iomanip>
#include <type_traits>
#include "FoundationKit/Base/Types.h"
#include "FoundationKit/Platform/Platform.h"

NS_FK_BEGIN

const uint64 INVALID_UNIQUE_ID = -1;
class unique_id
{
    static uint64 create();
public:
    static uint64_t sequence;
    static uint64_t machineid;
};



NS_FK_END

