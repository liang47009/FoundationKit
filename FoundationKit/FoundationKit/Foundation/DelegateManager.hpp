/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_DELEGATEMANAGER_HPP
#define FOUNDATIONKIT_DELEGATEMANAGER_HPP

#pragma once
#include <vector>
#include <string>
#include <functional>
#include <memory>
#include "FoundationKit/FoundationMacros.hpp"
#include "FoundationKit/Foundation/Singleton.hpp"

NS_FK_BEGIN

typedef std::function<void(const void*)>   DelegateCallbackType;
class DelegateObserver
{
public:
    typedef std::shared_ptr<DelegateObserver> Pointer;
    static Pointer create(const std::string& name, DelegateCallbackType& selector, void* target = nullptr, bool callOnce = false);
    ~DelegateObserver();
    void                          invoke(const void* args);
    void*                         getTarget()const;
    const DelegateCallbackType&   getSelector()const;
    const std::string&            getName()const;
    bool                          isCallOnce();
    bool operator ==(const DelegateObserver& other);
    bool operator != (const DelegateObserver& other);
protected:
    DelegateObserver(const std::string& name, DelegateCallbackType& selector, void* target = nullptr, bool callOnce = false);
private:
    std::string             _name;
    DelegateCallbackType    _selector;
    void*                   _target;
    bool                    _callOnce;
};

inline bool operator==(const DelegateObserver& l, const DelegateObserver& r)
{
    return (l.getTarget() == r.getTarget() && l.getName() == r.getName());
}

inline bool operator!=(const DelegateObserver& l, const DelegateObserver& r)
{
    return !(l == r);
}

class DelegateManager : public Singleton<DelegateManager>
{
    friend Singleton<DelegateManager>;
    typedef std::vector<DelegateObserver::Pointer>   ObserverList;


public:
    ~DelegateManager();
    void addObserver(const std::string& name, DelegateCallbackType& selector, void* target = nullptr, bool callOnce = false);
    void removeObserver( const std::string& name);
    void removeObserver( const char* name);
    void removeObserver( void* target);
    void removeObserver(const std::string& name, void* target);
    void invokeDelegate(const std::string& name, const void* args);

protected:
    // Check whether the observer exists by the specified target and name.
    bool observerExisted(const std::string& name,void* target = nullptr);

private:
    DelegateManager();
    ObserverList   _observers;
    std::mutex     _mutex;
};

NS_FK_END
#endif // FOUNDATIONKIT_DELEGATEMANAGER_HPP
