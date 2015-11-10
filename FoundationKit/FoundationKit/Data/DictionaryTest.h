#pragma once

#include "Dictionary.h"
#include "FoundationKit/Foundation/Logger.h"
#include <utility>

using namespace FoundationKit;

void TestDictionary()
{
    Dictionary<int, std::string>  dict;
    dict.insert({ 10, "this is test 0 " });
    dict.insert({ 11, "this is test 1 " });
    dict.insert({ 12, "this is test 2 " });
    dict.insert({ 13, "this is test 3 " });
    dict.insert({ 14, "this is test 4 " });

    Dictionary<int, std::string>  dict1 = dict;
    LOG_INFO("===== dict1 =====");
    for (auto& vpair: dict1)
    {
        LOG_INFO("===== KEY=%d, VALUE=%s", vpair.first, vpair.second.c_str());
    }

    Dictionary<int, std::string>  dict2 = std::move(dict);
    LOG_INFO("===== dict2 =====");
    for (auto& vpair : dict2)
    {
        LOG_INFO("===== KEY=%d, VALUE=%s", vpair.first, vpair.second.c_str());
    }

    // dict is invalid.
    for (auto& vpair : dict)
    {
        LOG_INFO("===== KEY=%d, VALUE=%s", vpair.first, vpair.second.c_str());
    }

    std::string value0 = dict2[10];
    std::string value1 = dict2[11];
    LOG_INFO("===== value0=%s, value1=%s", value0.c_str(), value1.c_str());

    auto _arg = std::pair<int, size_t>(10, 0);
    value0 = dict2[_arg];
    //value1 = dict2[std::pair<int, size_t>(10, 1)]; // crash

    int ikey = 10;
    value0 = dict2.at(ikey);
    value1 = dict2.at(ikey, 0);
    
    std::string value2 = dict2.at(_arg);

    LOG_INFO("===== value0=%s, value1=%s, value2=%s", value0.c_str(), value1.c_str(), value2.c_str());

    auto iter = dict2.begin();
    for (; iter != dict2.end(); ++iter)
    {
        LOG_INFO("===== KEY=%d, VALUE=%s", iter->first, iter->second.c_str());
    }

    auto iter1 = dict2.find(ikey);
    if (iter1 != dict2.cend())
    {
        LOG_INFO("===== KEY=%d, VALUE=%s", iter1->first, iter1->second.c_str());
    }
    else
    {
        LOG_INFO("===== can't find value with key %d", ikey);
    }

    LOG_INFO("===== Item count with %d = %d", ikey, dict2.count(ikey));
    dict2.insert({10,"value 0"});
    dict2.insert({10,"value 1"});
    dict2.insert({10,"value 2"});
    dict2.insert({10,"value 3"});
    dict2.insert(ikey, value0);
    
    auto values = dict2.valuesOfKey(10);
    for (auto value: values)
    {
        LOG_INFO("===== KEY=%d, VALUE=%s", 10, value.c_str());
    }
    
    auto keys = dict2.allKeys();
    for (auto key: keys)
    {
        LOG_INFO("===== KEY=%d", key);
    }


}















