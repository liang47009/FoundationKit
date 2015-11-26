#ifndef FoundationKit_MessageQueue_H
#define FoundationKit_MessageQueue_H
#pragma once

#include <mutex>
#include <string>
#include <functional>
#include <unordered_map>
#include "FoundationKit/stdextensions/any.hpp"
#include "FoundationKit/Foundation/function_traits.h"

using namespace std;
class MessageQueue
{
public:
    typedef std::unordered_multimap < std::string, any > MessageMultiMap;
    typedef std::unordered_multimap < std::string, any >::iterator iterator;
    typedef std::lock_guard<std::mutex>   GuardLock;
    typedef std::unique_lock<std::mutex>  UniqueLock;

    MessageQueue();

    template<typename F>
    void listen(std::string& name, F&& fun)
    {
        GuardLock guard(_messageLock);
        _messages.emplace(name, to_function(std::forward<F>(fun)));
    }

    template<typename ...Args>
    void postMessage(std::string& name, Args&&... args)
    {
        GuardLock guard(_messageLock);
        using function_type = std::function < void(Args...) > ;
        auto rangeOfName = _messages.equal_range(name);
        for (iterator it = rangeOfName.first; it != rangeOfName.second; ++it)
        {
            auto fun = any_cast<function_type>(it->second);
            fun(std::forward<Args>(args)...);
        }
    }

    void remove(std::string& name)
    {
        GuardLock guard(_messageLock);
        auto rangeOfName = _messages.equal_range(name);
        _messages.erase(rangeOfName.first, rangeOfName.second);
    }

private:
    MessageMultiMap           _messages;
    std::mutex                _messageLock;
};




#endif // FoundationKit_MessageQueue_H