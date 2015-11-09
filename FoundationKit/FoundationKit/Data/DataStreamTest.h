#pragma once

#include "FoundationKit/Foundation/Logger.h"
#include <utility>
#include <string>
#include "DataStream.h"

using namespace FoundationKit;

void TestDataStream()
{
    DataStream  ds;
    
    ds << 10 << "This is test" << 10.f << true;
    
    int v0;
    std::string v1;
    float v2;
    bool v3;
    ds >> v0 >> v1 >> v2 >> v3;

    LOG_INFO("VALUE = %d, %s, %f, %s", v0, v1.c_str(), v2, v3?"true":"false");
    
    
}















