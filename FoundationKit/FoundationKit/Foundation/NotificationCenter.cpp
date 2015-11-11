/****************************************************************************
  Copyright (c) 2015 libo All rights reserved.

  losemymind.libo@gmail.com

****************************************************************************/
#include "NotificationCenter.h"
NS_FK_BEGIN
namespace Notifications
{

    NotificationCenter* NotificationCenter::_defaultCenter = nullptr;

    NotificationCenter* NotificationCenter::defaultCenter()
    {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        if (!_defaultCenter)
        {
            _defaultCenter = new NotificationCenter();
        }
        return _defaultCenter;
    }

    void NotificationCenter::destroyInstance()
    {
        if (_defaultCenter)
        {
            delete _defaultCenter;
            _defaultCenter = nullptr;
        }
    }

    NotificationCenter::NotificationCenter()
    {

    }

    NotificationCenter::~NotificationCenter()
    {
        

    }

    NotificationObserver::Ptr NotificationCenter::addObserver(const ObjectTypePtr target,
        const NotificationSelector& selector, 
        const std::string& name, 
        const ObjectTypePtr sender /*= nullptr*/)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        NotificationObserver::Ptr observer = nullptr;
        do 
        {
            if (this->observerExisted(target, name, sender, observer))
                break;
            observer = NotificationObserver::create(target, selector, name, sender);
            if (!observer)
                break;
            _observers.push_back(observer);
        } while (false);
        return observer;
    }


    void NotificationCenter::removeObserver(const ObjectTypePtr target, const std::string& name)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        ObserverList::iterator iter = _observers.begin();
        for (; iter != _observers.end(); ++iter)
        {
            NotificationObserver* observer = iter->get();
            if (!observer)
                continue;
            if (observer->getTarget() == target && observer->getName() == name)
            {
                _observers.erase(iter);
                return;
            }
        }
    }

    void NotificationCenter::removeObserver(NotificationObserver::Ptr observerPtr)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        ObserverList::iterator iter = _observers.begin();
        for (; iter != _observers.end(); ++iter)
        {
            NotificationObserver* observer = iter->get();
            if (!observer)
                continue;
            if (*observer == *(observerPtr.get()))
            {
                _observers.erase(iter);
                return;
            }
        }
    }

    std::size_t NotificationCenter::removeAllObservers(const ObjectTypePtr target)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        std::size_t nRemoveCount = 0;
        ObserverList::iterator iter = _observers.begin();
        for (; iter != _observers.end(); )
        {
            NotificationObserver* observer = iter->get();
            if (!observer)
            {
                ++iter;
                continue;
            }
            if (observer->getTarget() == target)
            {
                iter = _observers.erase(iter);
                ++nRemoveCount;
            }
            else
            {
                ++iter;
            }
        }
        return nRemoveCount;
    }

    void NotificationCenter::postNotification(NotificationObserver::Ptr observerPtr, void* param /*= nullptr*/)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        ObserverList observersToNotify(_observers);
        lock.unlock();
        for (auto observer : observersToNotify)
        {
            if (*(observer.get()) == *(observerPtr.get()))
            {
                observerPtr->invoke(observer->getSender(), param);
            }
        }
    }

    void NotificationCenter::postNotification(const std::string& name)
    {
        postNotification(name, nullptr);
    }

    void NotificationCenter::postNotification(const std::string& name, const ObjectTypePtr sender)
    {
        postNotification(name, sender, nullptr);
    }

    void NotificationCenter::postNotification(const std::string& name, const ObjectTypePtr sender, void* param)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        ObserverList observersToNotify(_observers);
        lock.unlock();
        for (auto observerPtr : observersToNotify)
        {
            if (observerPtr->getName() == name && (observerPtr->getSender() == sender || observerPtr->getSender() == nullptr || sender == nullptr))
            {
                observerPtr->invoke(sender, param);
            }
        }
    }

    bool NotificationCenter::hasObservers() const
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return !_observers.empty();
    }

    std::size_t NotificationCenter::countObservers() const
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return _observers.size();
    }

    bool NotificationCenter::observerExisted(const ObjectTypePtr target, 
        const std::string& name, 
        const ObjectTypePtr sender,
        NotificationObserver::Ptr& observerPtr)
    {
        ObserverList::const_iterator iter = _observers.begin();
        for (; iter != _observers.end(); ++iter)
        {
            NotificationObserver* observer = iter->get();
            if (!observer)
                continue;
            if (observer->getTarget() == target && observer->getName() == name && observer->getSender() == sender)
            {
                observerPtr = *iter;
                return true;
            }
        }
        return false;
    }
}//namespace Notifications
NS_FK_END