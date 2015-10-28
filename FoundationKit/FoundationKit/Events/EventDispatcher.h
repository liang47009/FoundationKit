#ifndef FoundationKit_EventDispatcher_H
#define FoundationKit_EventDispatcher_H



#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include <set>

#include "FoundationKit/FoundationKitDefines.h"
#include "FoundationKit/Foundation/Singleton.h"
#include "EventListener.h"
#include "EventListenerCustom.h"

NS_FK_BEGIN

class Event;
class EventTouch;
class EventCustom;
class EventListenerCustom;

/** @class EventDispatcher
* @brief This class manages event listener subscriptions
and event dispatching.

The EventListener list is managed in such a way that
event listeners can be added and removed even
from within an EventListener, while events are being
dispatched.
*/
class EventDispatcher : public Singleton<EventDispatcher>
{
public:

    // Adds event listener.
   
    /** Adds a event listener for a specified event with the fixed priority.
     *  @param listener The listener of a specified event.
     *  @param target  The target of the listener.
     *  @param priority The fixed priority of the listener.
     *  @note A lower priority will be called before the ones that have a higher value.
     *        0 priority is forbidden for fixed priority since it's used for scene graph based priority.
     */
    void addEventListener(EventListener::Pointer listener, void* target=nullptr, int priority = 1);

    /** Adds a Custom event listener.
     *  It will use a fixed priority of 1.
     * @param eventName A given name of the event.
     * @param callback A given callback method that associated the event name.
     * @return the generated event. Needed in order to remove the event from the dispather
     */
    EventListenerCustom::Pointer addCustomEventListener(const std::string &eventName, const std::function<void(EventCustom*)>& callback);

    /////////////////////////////////////////////
    
    // Removes event listener
    
    /** Remove a listener.
     *
     *  @param listener The specified event listener which needs to be removed.
     */
    void removeEventListener(EventListener::Pointer listener);

    /** Removes all listeners with the same event listener type.
     *
     * @param listenerType A given event listener type which needs to be removed.
     */
    void removeEventListenersForType(EventListener::Type listenerType);

    /** Removes all listeners which are associated with the specified target.
     *
     * @param target A given target node.
     * @param recursive True if remove recursively, the default value is false.
     */
    void removeEventListenersForTarget(void* target);
    
    /** Removes all custom listeners with the same event name.
     *
     * @param customEventName A given event listener name which needs to be removed.
     */
    void removeCustomEventListeners(const std::string& customEventName);

    /** Removes all listeners.
     */
    void removeAllEventListeners();

    /////////////////////////////////////////////
    
    // Pauses / Resumes event listener
    
    /** Pauses all listeners which are associated the specified target.
     *
     * @param target A given target.
     */
    void pauseEventListenersForTarget(void* target);
    
    /** Resumes all listeners which are associated the specified target.
     *
     * @param target A given target node.
     */
    void resumeEventListenersForTarget(void* target);
    
    /** Whether to enable dispatching events.
     *
     * @param isEnabled  True if enable dispatching events.
     */
    void setEnabled(bool isEnabled);

    /** Checks whether dispatching events is enabled.
     *
     * @return True if dispatching events is enabled.
     */
    bool isEnabled() const;

    /////////////////////////////////////////////
    
    /** Dispatches the event.
     *  Also removes all EventListeners marked for deletion from the
     *  event dispatcher list.
     *
     * @param event The event needs to be dispatched.
     */
    void dispatchEvent(Event* event);

    /** Dispatches a Custom Event with a event name an optional user data.
     *
     * @param eventName The name of the event which needs to be dispatched.
     * @param optionalUserData The optional user data, it's a void*, the default value is nullptr.
     */
    void dispatchCustomEvent(const std::string &eventName, void *optionalUserData = nullptr);

    /////////////////////////////////////////////
    
    /** Destructor of EventDispatcher.
     */
    ~EventDispatcher();

protected:

    friend Singleton<EventDispatcher>;

    typedef   std::vector<EventListener::Pointer> EventListenerVector;

    /** Constructor of EventDispatcher.
    */
    EventDispatcher();
    
    /** Adds an event listener with item
     *  @note if it is dispatching event, the added operation will be delayed to the end of current dispatch
     *  @see forceAddEventListener
     */
    void addEventListenerInternal(EventListener::Pointer listener);
    
    
    /** Gets event the listener list for the event listener type. */
    EventListenerVector* getListeners(const EventListener::ListenerID& listenerID);
    
    
    /** Removes all listeners with the same event listener ID */
    void removeEventListenersForListenerID(const EventListener::ListenerID& listenerID);
    
    /** Sorts the listeners of specified type by fixed priority */
    void sortEventListenersOfPriority(const EventListener::ListenerID& listenerID);
    
    /** Updates all listeners
     *  1) Removes all listener items that have been marked as 'removed' when dispatching event.
     *  2) Adds all listener items that have been marked as 'added' when dispatching event.
     */
    void updateListeners(Event* event);

    /** Touch event needs to be processed different with other events since it needs support ALL_AT_ONCE and ONE_BY_NONE mode. */
    void dispatchTouchEvent(EventTouch* event);
    
    /** Associates target with event listener */
    void associateTargetAndEventListener(void* target, EventListener::Pointer listener);
    
    /** Dissociates target with event listener */
    void dissociateTargetAndEventListener(void* target, EventListener::Pointer listener);
    
    /** Dispatches event to listeners with a specified listener type */
    void dispatchEventToListeners(EventListenerVector* listeners, const std::function<bool(EventListener::Pointer)>& onEvent);
    
    /** Listeners map */
    std::unordered_map<EventListener::ListenerID, EventListenerVector*> _listenerMap;

    /** The map of target and event listeners */
    std::unordered_map<void*, EventListenerVector*> _targetListenersMap;
    
    /** Whether to enable dispatching event */
    bool _isEnabled;
};


NS_FK_END

#endif // FoundationKit_EventDispatcher_H

