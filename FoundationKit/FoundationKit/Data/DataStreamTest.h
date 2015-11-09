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
    DataStream  ds1 = ds;
    DataStream  ds2(ds);
    DataStream  dsmove0 = ds;
    DataStream  dsmove1 = ds;
    
    int v0;
    std::string v1;
    float v2;
    bool v3;
    ds >> v0 >> v1 >> v2 >> v3;
    LOG_INFO("ds:VALUE = %d, %s, %f, %s", v0, v1.c_str(), v2, v3?"true":"false");
    
    ds1 >> v0 >> v1 >> v2 >> v3;
    LOG_INFO("ds1:VALUE = %d, %s, %f, %s", v0, v1.c_str(), v2, v3?"true":"false");
    
    ds2 >> v0 >> v1 >> v2 >> v3;
    LOG_INFO("ds2:VALUE = %d, %s, %f, %s", v0, v1.c_str(), v2, v3?"true":"false");
    
    DataStream ds3 = std::move(dsmove0);
    DataStream ds4(std::move(dsmove1));
    
    ds3 >> v0 >> v1 >> v2 >> v3;
    LOG_INFO("ds3:VALUE = %d, %s, %f, %s", v0, v1.c_str(), v2, v3?"true":"false");
    
    ds4 >> v0 >> v1 >> v2 >> v3;
    LOG_INFO("ds4:VALUE = %d, %s, %f, %s", v0, v1.c_str(), v2, v3?"true":"false");
    
    dsmove0 >> v0 >> v1 >> v2 >> v3;
    LOG_INFO("dsmove0:VALUE = %d, %s, %f, %s", v0, v1.c_str(), v2, v3?"true":"false");
    
    dsmove1>> v0 >> v1 >> v2 >> v3;
    LOG_INFO("dsmove1:VALUE = %d, %s, %f, %s", v0, v1.c_str(), v2, v3?"true":"false");
    
}















