
#pragma once


#include "FoundationKit/Base/Types.h"
#include "FoundationKit/Platform/Platform.h"

NS_FK_BEGIN
const uint64 INVALID_UNIQUE_ID = 0xffffffffffffffff;
class unique_id
{
    static uint32               sequence;
    static uint64               machineid;
    static std::mutex           threadMutex;
public:
    static uint64 create();
    static uint64 createThreadSafe();

};

#if 0
#include <algorithm>
#include <vector>
#include <mutex>
#include <thread>
static void TestUniqueId()
{
    std::vector<uint64>  ids;
    uint64 MaxLoop = 10000000;
    std::mutex  _idsMutex;
    ids.reserve(MaxLoop * std::thread::hardware_concurrency());
    auto createId = [&]
    {
        for (uint64 i = 0; i < MaxLoop; i++)
        {
            std::lock_guard<std::mutex> _lock(_idsMutex);
            uint64 uid = unique_id::create();
            ids.push_back(uid);
        }
    };
    std::vector<std::thread>  threads;

    for (int i = 0; i < std::thread::hardware_concurrency(); i++)
    {
        threads.push_back(std::thread(createId));
    }
    for (int i = 0; i < std::thread::hardware_concurrency(); i++)
    {
        threads[i].join();
    }
    std::vector<uint64>::iterator new_end = std::unique(ids.begin(), ids.end());
    std::vector<uint64> newIds(new_end, ids.end());
    ids.erase(new_end, ids.end());
}

#endif



NS_FK_END

