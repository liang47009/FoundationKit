/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_NOTIFICATIONCENTER_HPP
#define FOUNDATIONKIT_NOTIFICATIONCENTER_HPP


#pragma once
#include <vector>
#include <string>
#include <functional>
#include <memory>
#include "FoundationKit/FoundationMacros.hpp"
#include "FoundationKit/Foundation/Singleton.hpp"
#include "FoundationKit/Foundation/FunctionHandler.hpp"

NS_FK_BEGIN

class NotificationObserver
{
public:
    typedef std::shared_ptr<NotificationObserver> Pointer;
    static Pointer Create(const std::string& name, FunctionHandlerPointer selector, void* target = nullptr, bool callOnce = false);
    ~NotificationObserver();
    void                          Invoke(const ArgumentList& args);
    void*                         GetTarget()const;
    const FunctionHandlerPointer  GetSelector()const;
    const std::string&            GetName()const;
    bool                          IsCallOnce();
    bool operator ==(const NotificationObserver& other);
    bool operator != (const NotificationObserver& other);
protected:
    NotificationObserver(const std::string& name, FunctionHandlerPointer selector, void* target = nullptr, bool callOnce = false);
private:
    std::string             _name;
    FunctionHandlerPointer  _pSelector;
    void*                   _target;
    bool                    _callOnce;
};

inline bool operator==(const NotificationObserver& l, const NotificationObserver& r)
{
    return (l.GetTarget() == r.GetTarget() && l.GetName() == r.GetName());
}

inline bool operator!=(const NotificationObserver& l, const NotificationObserver& r)
{
    return !(l == r);
}

class NotificationCenter : public Singleton<NotificationCenter>
{
    friend Singleton<NotificationCenter>;
    typedef std::vector<NotificationObserver::Pointer>   ObserverList;

public:
    ~NotificationCenter();
    void AddObserver(const std::string& name, FunctionHandlerPointer selector, void* target = nullptr, bool callOnce = false);
    void RemoveObserver( const std::string& name);
    void RemoveObserver( const char* name);
    void RemoveObserver( void* target);
    void RemoveObserver(const std::string& name, void* target);
    void Invoke(const std::string& name, const ArgumentList& args);

protected:
    // Check whether the observer exists by the specified target and name.
    bool ObserverExisted(const std::string& name,void* target = nullptr);

private:
    NotificationCenter();
    ObserverList   _observers;
    std::mutex     _mutex;
};

NS_FK_END

#endif // END OF FOUNDATIONKIT_NOTIFICATIONCENTER_HPP
