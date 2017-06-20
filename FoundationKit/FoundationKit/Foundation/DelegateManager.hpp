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
#include "FoundationKit/Foundation/FuncationSelector.hpp"

NS_FK_BEGIN

class DelegateObserver
{
public:
    typedef std::shared_ptr<DelegateObserver> Pointer;
    static Pointer Create(const std::string& name, SelectorPointer selector, void* target = nullptr, bool callOnce = false);
    ~DelegateObserver();
    void                          Invoke(const ArgumentList& args);
    void*                         GetTarget()const;
    const SelectorPointer         GetSelector()const;
    const std::string&            GetName()const;
    bool                          IsCallOnce();
    bool operator ==(const DelegateObserver& other);
    bool operator != (const DelegateObserver& other);
protected:
    DelegateObserver(const std::string& name, SelectorPointer selector, void* target = nullptr, bool callOnce = false);
private:
    std::string             _name;
    SelectorPointer         _pSelector;
    void*                   _target;
    bool                    _callOnce;
};

inline bool operator==(const DelegateObserver& l, const DelegateObserver& r)
{
    return (l.GetTarget() == r.GetTarget() && l.GetName() == r.GetName());
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
    void AddObserver(const std::string& name, SelectorPointer selector, void* target = nullptr, bool callOnce = false);
    void RemoveObserver( const std::string& name);
    void RemoveObserver( const char* name);
    void RemoveObserver( void* target);
    void RemoveObserver(const std::string& name, void* target);
    void InvokeDelegate(const std::string& name, const ArgumentList& args);

protected:
    // Check whether the observer exists by the specified target and name.
    bool ObserverExisted(const std::string& name,void* target = nullptr);

private:
    DelegateManager();
    ObserverList   _observers;
    std::mutex     _mutex;
};

NS_FK_END
#endif // FOUNDATIONKIT_DELEGATEMANAGER_HPP
