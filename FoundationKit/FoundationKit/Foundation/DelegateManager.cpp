/****************************************************************************
 Copyright (c) 2016 libo All rights reserved.
 
 losemymind.libo@gmail.com
 
 ****************************************************************************/

#include "DelegateManager.h"
NS_FK_BEGIN

/************************************************************************/
/*                          DelegateObserver                            */
/************************************************************************/

DelegateObserver::Pointer DelegateObserver::create(const std::string& name, DelegateCallbackType& selector, void* target /*= nullptr*/, bool callOnce /*= false*/)
{
    DelegateObserver *observer = new  DelegateObserver(name, selector, target, callOnce);
    return DelegateObserver::Pointer(observer);
}

DelegateObserver::DelegateObserver(const std::string& name, DelegateCallbackType& selector, void* target /*= nullptr*/, bool callOnce /*= false*/)
: _name(name)
, _selector(selector)
, _target(target)
, _callOnce(callOnce)
{

}

DelegateObserver::~DelegateObserver()
{

}

void DelegateObserver::invoke(const void* args)
{
    if (_selector)
    {
        _selector(args);
    }
}


void* DelegateObserver::getTarget() const
{
    return _target;
}

const DelegateCallbackType& DelegateObserver::getSelector() const
{
    return _selector; 
}

const std::string& DelegateObserver::getName() const
{
    return _name;
}

bool DelegateObserver::isCallOnce()
{
    return _callOnce;
}

bool DelegateObserver::operator==(const DelegateObserver& other)
{
    return (this->_target == other._target&&
            this->_name == other._name);
}

bool DelegateObserver::operator!=(const DelegateObserver& other)
{
    return !(*this == other);
}


/************************************************************************/
/*                  DelegateManager                                     */
/************************************************************************/
DelegateManager::DelegateManager()
{

}

DelegateManager::~DelegateManager()
{

}

void DelegateManager::addObserver(const std::string& name, DelegateCallbackType& selector, void* target /*= nullptr*/, bool callOnce /*= false*/)
{
    std::lock_guard<std::mutex> lock(_mutex);
    DelegateObserver::Pointer observer;
    do 
    {
        FK_BREAK_IF(this->observerExisted(name,target));
        observer = DelegateObserver::create(name, selector, target, callOnce);
        FK_BREAK_IF(!observer);
        _observers.push_back(observer);
    } while (false);
}

void DelegateManager::removeObserver(const std::string& name)
{
    std::lock_guard<std::mutex> lock(_mutex);
    ObserverList::iterator iter = _observers.begin();
    while(iter !=  _observers.end())
    {
        DelegateObserver::Pointer observer = *iter;
        if (!observer)
            continue;
        if (observer->getName() == name)
        {
            iter = _observers.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

void DelegateManager::removeObserver(const char* name)
{
    std::string strName = name;
    removeObserver(strName);
}


void DelegateManager::removeObserver(void* target)
{
    std::lock_guard<std::mutex> lock(_mutex);
    ObserverList::iterator iter = _observers.begin();
    while(iter !=  _observers.end())
    {
        DelegateObserver::Pointer observer = *iter;
        if (!observer)
            continue;
        if (observer->getTarget() == target)
        {
            iter = _observers.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

void DelegateManager::removeObserver(const std::string& name, void* target)
{
    std::lock_guard<std::mutex> lock(_mutex);
    ObserverList::iterator iter = _observers.begin();
    while(iter !=  _observers.end())
    {
        DelegateObserver::Pointer observer = *iter;
        if (!observer)
            continue;
        if (observer->getName() == name && observer->getTarget() == target)
        {
            iter = _observers.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

void DelegateManager::invokeDelegate(const std::string& name, const void* args)
{
    std::unique_lock<std::mutex> lock(_mutex);
    ObserverList observersToCopy(_observers);
    lock.unlock();

    ObserverList::iterator iter = observersToCopy.begin();
    while(iter !=  observersToCopy.end())
    {
        DelegateObserver::Pointer observer = *iter;
        if (!observer)
            continue;
        if (observer->getName() == name)
        {
            observer->invoke(args);
            if (observer->isCallOnce())
            {
                iter = observersToCopy.erase(iter);
                continue;
            }
        }
        ++iter;
    }
}

bool DelegateManager::observerExisted(const std::string& name, void* target /*= nullptr*/)
{
    ObserverList::const_iterator iter = _observers.begin();
    for (; iter != _observers.end(); ++iter)
    {
        DelegateObserver::Pointer observer = *iter;
        if (!observer)
            continue;
        if (observer->getTarget() == target && observer->getName() == name)
        {
            return true;
        }
    }
    return false;
}

NS_FK_END


