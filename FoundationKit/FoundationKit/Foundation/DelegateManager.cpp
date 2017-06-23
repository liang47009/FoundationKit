/****************************************************************************
 Copyright (c) 2017 libo All rights reserved.
 
 losemymind.libo@gmail.com
 
 ****************************************************************************/

#include "FoundationKit/Foundation/DelegateManager.hpp"
NS_FK_BEGIN

/************************************************************************/
/*                          DelegateObserver                            */
/************************************************************************/

DelegateObserver::Pointer DelegateObserver::Create(const std::string& name, FunctionHandlerPointer selector, void* target /*= nullptr*/, bool callOnce /*= false*/)
{
    DelegateObserver *observer = new  DelegateObserver(name, selector, target, callOnce);
    return DelegateObserver::Pointer(observer);
}

DelegateObserver::DelegateObserver(const std::string& name, FunctionHandlerPointer selector, void* target /*= nullptr*/, bool callOnce /*= false*/)
: _name(name)
, _pSelector(selector)
, _target(target)
, _callOnce(callOnce)
{

}

DelegateObserver::~DelegateObserver()
{

}

void DelegateObserver::Invoke(const ArgumentList& args)
{
    if (_pSelector)
    {
        _pSelector->Invoke(args);
    }
}


void* DelegateObserver::GetTarget() const
{
    return _target;
}

const FunctionHandlerPointer DelegateObserver::GetSelector() const
{
    return _pSelector;
}

const std::string& DelegateObserver::GetName() const
{
    return _name;
}

bool DelegateObserver::IsCallOnce()
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

void DelegateManager::AddObserver(const std::string& name, FunctionHandlerPointer selector, void* target /*= nullptr*/, bool callOnce /*= false*/)
{
    std::lock_guard<std::mutex> lock(_mutex);
    DelegateObserver::Pointer observer;
    do 
    {
        BREAK_IF(this->ObserverExisted(name,target));
        observer = DelegateObserver::Create(name, selector, target, callOnce);
        BREAK_IF(!observer);
        _observers.push_back(observer);
    } while (false);
}

void DelegateManager::RemoveObserver(const std::string& name)
{
    std::lock_guard<std::mutex> lock(_mutex);
    ObserverList::iterator iter = _observers.begin();
    while(iter !=  _observers.end())
    {
        DelegateObserver::Pointer observer = *iter;
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

void DelegateManager::RemoveObserver(const char* name)
{
    std::string strName = name;
    RemoveObserver(strName);
}


void DelegateManager::RemoveObserver(void* target)
{
    std::lock_guard<std::mutex> lock(_mutex);
    ObserverList::iterator iter = _observers.begin();
    while(iter !=  _observers.end())
    {
        DelegateObserver::Pointer observer = *iter;
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

void DelegateManager::RemoveObserver(const std::string& name, void* target)
{
    std::lock_guard<std::mutex> lock(_mutex);
    ObserverList::iterator iter = _observers.begin();
    while(iter !=  _observers.end())
    {
        DelegateObserver::Pointer observer = *iter;
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

void DelegateManager::Invoke(const std::string& name, const ArgumentList& args)
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

bool DelegateManager::ObserverExisted(const std::string& name, void* target /*= nullptr*/)
{
    ObserverList::const_iterator iter = _observers.begin();
    for (; iter != _observers.end(); ++iter)
    {
        DelegateObserver::Pointer observer = *iter;
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


