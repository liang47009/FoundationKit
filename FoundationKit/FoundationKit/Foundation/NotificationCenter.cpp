/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com
 
 ****************************************************************************/
#include "FoundationKit/Foundation/NotificationCenter.hpp"
NS_FK_BEGIN

/************************************************************************/
/*                          NotificationObserver                            */
/************************************************************************/

NotificationObserver::Pointer NotificationObserver::Create(const std::string& name, FunctionBoxPointer selector, void* target /*= nullptr*/, bool callOnce /*= false*/)
{
    NotificationObserver *observer = new  NotificationObserver(name, selector, target, callOnce);
    return NotificationObserver::Pointer(observer);
}

NotificationObserver::NotificationObserver(const std::string& name, FunctionBoxPointer selector, void* target /*= nullptr*/, bool callOnce /*= false*/)
: _name(name)
, _pSelector(selector)
, _target(target)
, _callOnce(callOnce)
{

}

NotificationObserver::~NotificationObserver()
{

}

void NotificationObserver::Invoke(const ValueList& args)
{
    if (_pSelector)
    {
        _pSelector->Invoke(args);
    }
}


void* NotificationObserver::GetTarget() const
{
    return _target;
}

const FunctionBoxPointer NotificationObserver::GetSelector() const
{
    return _pSelector;
}

const std::string& NotificationObserver::GetName() const
{
    return _name;
}

bool NotificationObserver::IsCallOnce()
{
    return _callOnce;
}

bool NotificationObserver::operator==(const NotificationObserver& other)
{
    return (this->_target == other._target&&
            this->_name == other._name);
}

bool NotificationObserver::operator!=(const NotificationObserver& other)
{
    return !(*this == other);
}


/************************************************************************/
/*                  NotificationCenter                                  */
/************************************************************************/

NotificationCenter NotificationCenter::DefaultCenter;

NotificationCenter::NotificationCenter()
{

}

NotificationCenter::~NotificationCenter()
{

}

void NotificationCenter::AddObserver(const std::string& name, FunctionBoxPointer selector, void* target /*= nullptr*/, bool callOnce /*= false*/)
{
    std::lock_guard<std::mutex> lock(_mutex);
    NotificationObserver::Pointer observer;
    do 
    {
        void* InternalTarget = target;
        if (InternalTarget == nullptr)
        {
            InternalTarget = selector.get();
        }
        BREAK_IF(this->ObserverExisted(name, InternalTarget));
        observer = NotificationObserver::Create(name, selector, InternalTarget, callOnce);
        BREAK_IF(!observer);
        _observers.push_back(observer);
    } while (false);
}

void NotificationCenter::RemoveObserver(const std::string& name)
{
    std::lock_guard<std::mutex> lock(_mutex);
    ObserverList::iterator iter = _observers.begin();
    while(iter !=  _observers.end())
    {
        NotificationObserver::Pointer observer = *iter;
        if (!observer)
            continue;
        if (observer->GetName() == name)
        {
            iter = _observers.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

void NotificationCenter::RemoveObserver(void* target)
{
    std::lock_guard<std::mutex> lock(_mutex);
    ObserverList::iterator iter = _observers.begin();
    while(iter !=  _observers.end())
    {
        NotificationObserver::Pointer observer = *iter;
        if (!observer)
            continue;
        if (observer->GetTarget() == target)
        {
            iter = _observers.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

void NotificationCenter::RemoveObserver(const std::string& name, void* target)
{
    std::lock_guard<std::mutex> lock(_mutex);
    ObserverList::iterator iter = _observers.begin();
    while(iter !=  _observers.end())
    {
        NotificationObserver::Pointer observer = *iter;
        if (!observer)
            continue;
        if (observer->GetName() == name && observer->GetTarget() == target)
        {
            iter = _observers.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

void NotificationCenter::Invoke(const std::string& name, const ValueList& args)
{
    std::unique_lock<std::mutex> lock(_mutex);
    ObserverList observersToCopy(_observers);
    lock.unlock();

    ObserverList::iterator iter = observersToCopy.begin();
    while(iter !=  observersToCopy.end())
    {
        NotificationObserver::Pointer observer = *iter;
        if (!observer)
            continue;
        if (observer->GetName() == name)
        {
            observer->Invoke(args);
            if (observer->IsCallOnce())
            {
                RemoveObserver(observer->GetName(), observer->GetTarget());
            }
        }
        ++iter;
    }
}

bool NotificationCenter::ObserverExisted(const std::string& name, void* target /*= nullptr*/)
{
    ObserverList::const_iterator iter = _observers.begin();
    for (; iter != _observers.end(); ++iter)
    {
        NotificationObserver::Pointer observer = *iter;
        if (!observer)
            continue;
        if (observer->GetTarget() == target && observer->GetName() == name)
        {
            return true;
        }
    }
    return false;
}

NS_FK_END


