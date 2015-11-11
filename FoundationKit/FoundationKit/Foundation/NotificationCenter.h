/****************************************************************************
  Copyright (c) 2015 libo All rights reserved.

  losemymind.libo@gmail.com

****************************************************************************/
#pragma once
#include <mutex>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "NotificationObserver.h"
#include "FoundationKit/FoundationKitDefines.h"

NS_FK_BEGIN
/** Thread safe NotificationCenter.
 *  
 */
namespace Notifications
{
#define NOTIFI_CALLBACK(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, ##__VA_ARGS__)

    class NotificationCenter
    {
    public:
        // Get NotificationCenter instance.
        static NotificationCenter* defaultCenter();

        // When application exit, must be call this method release NotificationCenter;
        static void                destroyInstance();

       /** @brief Adds an observer for the specified target.
        *  @param[in] target   The target which wants to observer notification events.
        *  @param[in] selector The callback function which will be invoked when the
        *                      specified notification event was posted.
        *  @param[in] name     The name of this notification.
        *  @param[in] sender   The object whose notifications the target wants to receive.
        *                      Only notifications sent by this sender are delivered to the target.
        *                      nullptr means that the sender is not used to decide whether to deliver
        *                      the notification to target.
        */
        NotificationObserver::Ptr addObserver(const ObjectTypePtr target,
            const NotificationSelector& selector, 
            const std::string& name, 
            const ObjectTypePtr sender = nullptr);

       /** @brief Removes the observer by the specified target and name.
        *  @param target The target of this notification.
        *  @param name The name of this notification.
        */
        void removeObserver(const ObjectTypePtr target, const std::string& name);

        /** @brief Removes the observer by the specified target and name.
         *  @param observer The NotificationObserver shared_ptr object.
         */
        void removeObserver(NotificationObserver::Ptr observerPtr);

       /** @brief Removes all notifications registered by this target
        *  @param target The target of this notification.
        *  @returns the number of observers removed
        */
        std::size_t removeAllObservers(const ObjectTypePtr target);

       /** @brief Posts one notification event by name.
        *  @param name The name of this notification.
        */
        void postNotification(const std::string& name);

       /** @brief Posts one notification event by name.
        *  @param name The name of this notification.
        *  @param sender The object posting the notification. Can be nullptr
        */
        void postNotification(const std::string& name, const ObjectTypePtr sender);

        /** @brief Posts one notification event by name.
         *  @param observerPtr The std::shared_ptr of NotificationObserver.
         */
        void postNotification(NotificationObserver::Ptr observerPtr, void* param = nullptr);

       /** @brief Posts one notification event by name.
        *  @param name The name of this notification.
        *  @param sender The object posting the notification. Can be nullptr
        *  @param param  The param posting the notification. Can be nullptr
        */
        void postNotification(const std::string& name, const ObjectTypePtr sender, void* param);

        /** @brief Returns true if there is at least one registered observer.
         *
         * Can be used to improve performance if an expensive notification
         * shall only be created and posted if there are any observers.
         */
        bool hasObservers() const;

        // Returns the number of registered observers.
        std::size_t countObservers() const;

        ~NotificationCenter();
    private:
        NotificationCenter();
        // Check whether the observer exists by the specified target and name.
        bool observerExisted(const ObjectTypePtr target, const std::string& name, const ObjectTypePtr sender, NotificationObserver::Ptr& observerPtr);

    private:
        typedef std::vector<NotificationObserver::Ptr> ObserverList;
        ObserverList  _observers;
        mutable std::mutex _mutex;
        static NotificationCenter* _defaultCenter;
    };

}//namespace Notifications

#define DefaultNotificationCenter Notifications::NotificationCenter::defaultCenter()

NS_FK_END