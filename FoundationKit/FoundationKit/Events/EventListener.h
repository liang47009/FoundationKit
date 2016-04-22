#pragma once

#include <functional>
#include <string>
#include <memory>
#include "FoundationKit/GenericPlatformMacros.h"

NS_FK_BEGIN

class Event;

/**
 *  The base class of event listener.
 *  If you need custom listener which with different callback, you need to inherit this class.
 *  For instance, you could refer to EventListenerAcceleration, EventListenerKeyboard, EventListenerTouchOneByOne, EventListenerCustom.
 */
class EventListener
{
public:
    enum class Type
    {
        UNKNOWN,
        TOUCH_ONE_BY_ONE,
        TOUCH_ALL_AT_ONCE,
        KEYBOARD,
        MOUSE,
        ACCELERATION,
        GAME_CONTROLLER,
        CUSTOM,
    };

    typedef std::string ListenerID;
    typedef std::shared_ptr<EventListener> Pointer;
protected:
    /** Constructor */
    EventListener();

    /** Initializes event with type and callback function */
    bool init(Type t, const ListenerID& listenerID, const std::function<void(Event*)>& callback);
public:
    /** Destructor */
    virtual ~EventListener();

    /** Checks whether the listener is available. */
    virtual bool checkAvailable() = 0;

    /** Enables or disables the listener
     *  @note Only listeners with `enabled` state will be able to receive events.
     *        When an listener was initialized, it's enabled by default.
     *        An event listener can receive events when it is enabled and is not paused.
     *        paused state is always false when it is a fixed priority listener.
     */
    inline void setEnabled(bool enabled) { _isEnabled = enabled; };

    /** Checks whether the listener is enabled */
    inline bool isEnabled() const { return _isEnabled; };

protected:

    /** Sets paused state for the listener
     *  The paused state is only used for scene graph priority listeners.
     *  `EventDispatcher::resumeAllEventListenersForTarget(node)` will set the paused state to `true`,
     *  while `EventDispatcher::pauseAllEventListenersForTarget(node)` will set it to `false`.
     *  @note 1) Fixed priority listeners will never get paused. If a fixed priority doesn't want to receive events,
     *           call `setEnabled(false)` instead.
     *        2) In `Node`'s onEnter and onExit, the `paused state` of the listeners which associated with that node will be automatically updated.
     */
    inline void setPaused(bool paused) { _paused = paused; };

    /** Checks whether the listener is paused */
    inline bool isPaused() const { return _paused; };

    /** Marks the listener was registered by EventDispatcher */
    inline void setRegistered(bool registered) { _isRegistered = registered; };

    /** Checks whether the listener was registered by EventDispatcher */
    inline bool isRegistered() const { return _isRegistered; };

    /** Gets the type of this listener
     *  @note It's different from `EventType`, e.g. TouchEvent has two kinds of event listeners - EventListenerOneByOne, EventListenerAllAtOnce
     */
    inline Type getType() const { return _type; };

    /** Gets the listener ID of this listener
     *  When event is being dispatched, listener ID is used as key for searching listeners according to event type.
     */
    inline const ListenerID& getListenerID() const { return _listenerID; };

    /** Sets the fixed priority for this listener
     *  @note This method is only used for `fixed priority listeners`, it needs to access a non-zero value.
     *  0 is reserved for scene graph priority listeners
     */
    inline void setPriority(int fixedPriority) { _fixedPriority = fixedPriority; };

    /** Gets the fixed priority of this listener
     *  @return 0 if it's a scene graph priority listener, non-zero for fixed priority listener
     */
    inline int getPriority() const { return _fixedPriority; };

    /** Sets the node associated with this listener */
    inline void setAssociatedTarget(void* target) { _target = target; };

    /** Gets the node associated with this listener
    *  @return nullptr if it's a fixed priority listener, otherwise return non-nullptr
    */
    inline void* getAssociatedTarget() const { return _target; };

    ///////////////
    // Properties
    //////////////
    std::function<void(Event*)> _onEvent;   /// Event callback function

    Type        _type;                             /// Event listener type
    ListenerID  _listenerID;                 /// Event listener ID
    bool        _isRegistered;                     /// Whether the listener has been added to dispatcher.
    void*       _target;            // scene graph based priority
    int         _fixedPriority;   // The higher the number, the higher the priority, 0 is for scene graph base priority.
    bool        _paused;           // Whether the listener is paused
    bool        _isEnabled;        // Whether the listener is enabled
    friend class EventDispatcher;
};

NS_FK_END