/****************************************************************************
Copyright (c) 2015-2016 libo.

losemymind.libo@gmail.com

****************************************************************************/
#pragma once
#include <memory>
#include <string>
#include <functional>
#include "FoundationKit/FoundationKitDefines.h"

NS_FK_BEGIN

namespace Notifications
{
    struct Notification;

    typedef void*                                ObjectTypePtr;
    typedef std::function<void(Notification*)>   NotificationSelector;

    struct Notification
    {
        Notification();
        Notification(ObjectTypePtr sender, void* param);
        ObjectTypePtr _sender;
        void*         _param;
    };

    class NotificationObserver
    {
    public:
        typedef std::shared_ptr<NotificationObserver> Ptr;

        /** @brief NotificationObserver constructor
         *  @param[in] target   The target which wants to observer notification events.
         *  @param[in] selector The callback function which will be invoked when the
         *                      specified notification event was posted.
         *  @param[in] name     The name of this notification.
         *  @param[in] sender   The object whose notifications the target wants to receive.
         *                      Only notifications sent by this sender are delivered to the target.
         *                      nullptr means that the sender is not used to decide whether to deliver
         *                      the notification to target.
         */

        static Ptr create(ObjectTypePtr target, const NotificationSelector& selector, const std::string& name, ObjectTypePtr sender = nullptr);

        ~NotificationObserver();

       /** @brief Invokes the callback function of this observer 
        *  @param[in] sender   The object whose notifications the target wants to receive.
        *                      Only notifications sent by this sender are delivered to the target.
        *                      nullptr means that the sender is not used to decide whether to deliver
        *                      the notification to target.
        *  @param[in] param    The param.
        */
        void                    invoke(ObjectTypePtr sender, void* param);
        ObjectTypePtr           getTarget()const;
        NotificationSelector    getSelector()const;
        const std::string&      getName()const;
        ObjectTypePtr           getSender() const;

        bool operator ==(const NotificationObserver& other);
        bool operator ==(const NotificationObserver* other);
        bool operator != (const NotificationObserver& other);
    protected:
       /** @brief NotificationObserver constructor
        *  @param[in] target   The target which wants to observer notification events.
        *  @param[in] selector The callback function which will be invoked when the
        *                      specified notification event was posted.
        *  @param[in] name     The name of this notification.
        *  @param[in] sender   The object whose notifications the target wants to receive.
        *                      Only notifications sent by this sender are delivered to the target.
        *                      nullptr means that the sender is not used to decide whether to deliver
        *                      the notification to target.
        */
        NotificationObserver(ObjectTypePtr target, const NotificationSelector& selector, const std::string& name, ObjectTypePtr sender = nullptr);
    private:
        ObjectTypePtr           _target;
        NotificationSelector    _selector;
        std::string             _name;
        ObjectTypePtr           _sender;
    };

}// Notifications
NS_FK_END
