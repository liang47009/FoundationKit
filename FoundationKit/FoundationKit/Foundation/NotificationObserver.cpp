/****************************************************************************
Copyright (c) 2015-2016 libo.

losemymind.libo@gmail.com

****************************************************************************/
#include "NotificationObserver.h"
NS_FK_BEGIN
namespace Notifications
{
    Notification::Notification()
        : _sender(nullptr)
        , _param(nullptr)
    {

    }

    Notification::Notification(ObjectTypePtr sender, void* param)
        : _sender(sender)
        , _param(param)
    {

    }

    //==========================================================================

    NotificationObserver::Ptr NotificationObserver::create(ObjectTypePtr target, const NotificationSelector& selector, const std::string& name, ObjectTypePtr sender /*= nullptr*/)
    {
        NotificationObserver *observer = new (std::nothrow) NotificationObserver(target, selector, name, sender);
        if (!observer)
            return nullptr;
        return NotificationObserver::Ptr(observer);
    }

    NotificationObserver::NotificationObserver(ObjectTypePtr target, 
        const NotificationSelector& selector, 
        const std::string& name, 
        ObjectTypePtr sender /*= nullptr*/)
        :_target(target)
        , _selector(selector)
        , _name(name)
        , _sender(sender)
    {

    }

    NotificationObserver::~NotificationObserver()
    {

    }

    void NotificationObserver::invoke(ObjectTypePtr sender, void* param)
    {
        if (_selector)
        {
            Notification nf(sender, param);
            _selector(&nf);
        }
    }

    ObjectTypePtr NotificationObserver::getTarget() const
    {
        return _target;
    }

    NotificationSelector NotificationObserver::getSelector() const
    {
        return _selector;
    }

    const std::string& NotificationObserver::getName()const
    {
        return _name;
    }

    ObjectTypePtr NotificationObserver::getSender() const
    {
        return _sender;
    }

    bool NotificationObserver::operator==(const NotificationObserver& other)
    {
        return (this->_target == other._target&&
                this->_name == other._name &&
                this->_sender == other._sender);
    }

    bool NotificationObserver::operator==(const NotificationObserver* other)
    {
        return (this->_target == other->_target&&
            this->_name == other->_name &&
            this->_sender == other->_sender);
    }


    bool NotificationObserver::operator!=(const NotificationObserver& other)
    {
        return !(*this == other);
    }

}//namespace Notifications
NS_FK_END