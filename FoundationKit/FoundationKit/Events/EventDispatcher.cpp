
#include "EventDispatcher.h"
#include <algorithm>
#include <memory>

#include "EventCustom.h"
#include "EventTouch.h"
#include "EventListenerTouch.h"
#include "EventListenerAcceleration.h"
#include "EventListenerMouse.h"
#include "EventListenerKeyboard.h"
#include "EventListenerController.h"

NS_FK_BEGIN

static EventListener::ListenerID __getListenerID(Event* event)
{
    EventListener::ListenerID ret;
    switch (event->getType())
    {
    case FoundationKit::Event::Type::TOUCH:
        // Touch listener is very special, it contains two kinds of listeners, EventListenerTouchOneByOne and EventListenerTouchAllAtOnce.
        // return UNKNOWN instead.
        FKASSERT(false, "Don't call this method if the event is for touch.");
        break;
    case FoundationKit::Event::Type::KEYBOARD:
        ret = EventListenerKeyboard::LISTENER_ID;
        break;
    case FoundationKit::Event::Type::ACCELERATION:
        ret = EventListenerAcceleration::LISTENER_ID;
        break;
    case FoundationKit::Event::Type::MOUSE:
        ret = EventListenerMouse::LISTENER_ID;
        break;
    case FoundationKit::Event::Type::GAME_CONTROLLER:
        ret = EventListenerController::LISTENER_ID;
        break;
    case FoundationKit::Event::Type::CUSTOM:
    {
        auto customEvent = static_cast<EventCustom*>(event);
        ret = customEvent->getEventName();
    }
        break;
    default:
        FKASSERT(false, "Invalid type!");
        break;
    }
    return ret;
}


EventDispatcher::EventDispatcher()
: _isEnabled(false)
{

}

EventDispatcher::~EventDispatcher()
{
    removeAllEventListeners();
}

void EventDispatcher::pauseEventListenersForTarget(void* target)
{
    auto listenerIter = _targetListenersMap.find(target);
    if (listenerIter != _targetListenersMap.end())
    {
        auto listeners = listenerIter->second;
        for (auto& l : *listeners)
        {
            l->setPaused(true);
        }
    }
}

void EventDispatcher::resumeEventListenersForTarget(void* target)
{
    auto listenerIter = _targetListenersMap.find(target);
    if (listenerIter != _targetListenersMap.end())
    {
        auto listeners = listenerIter->second;
        for (auto& l : *listeners)
        {
            l->setPaused(false);
        }
    }
}

void EventDispatcher::removeEventListenersForTarget(void* target)
{
    auto listenerIter = _targetListenersMap.find(target);
    if (listenerIter != _targetListenersMap.end())
    {
        auto listeners = listenerIter->second;
        auto listenersCopy = *listeners;
        for (auto& l : listenersCopy)
        {
            removeEventListener(l);
        }
    }
}

void EventDispatcher::associateTargetAndEventListener(void* target, EventListener::Pointer listener)
{
    std::vector<EventListener::Pointer>* listeners = nullptr;
    auto found = _targetListenersMap.find(target);
    if (found != _targetListenersMap.end())
    {
        listeners = found->second;
    }
    else
    {
        listeners = new std::vector<EventListener::Pointer>();
        _targetListenersMap.insert(std::make_pair(target, listeners));
    }
    
    listeners->push_back(listener);
}

void EventDispatcher::dissociateTargetAndEventListener(void* target, EventListener::Pointer listener)
{
    std::vector<EventListener::Pointer>* listeners = nullptr;
    auto found = _targetListenersMap.find(target);
    if (found != _targetListenersMap.end())
    {
        listeners = found->second;
        auto iter = std::find(listeners->begin(), listeners->end(), listener);
        if (iter != listeners->end())
        {
            listeners->erase(iter);
        }
        
        if (listeners->empty())
        {
            _targetListenersMap.erase(found);
            delete listeners;
        }
    }
}



void EventDispatcher::addEventListener(EventListener::Pointer listener, void* target /*= nullptr*/,int priority/* = 1*/)
{
    FKASSERT(listener, "Invalid parameters.");
    FKASSERT(!listener->isRegistered(), "The listener has been registered.");
    
    if (!listener->checkAvailable())
        return;
    
    listener->setAssociatedTarget(target);
    listener->setPriority(priority);
    listener->setRegistered(true);
    listener->setPaused(false);

    addEventListenerInternal(listener);
}

EventListenerCustom::Pointer EventDispatcher::addCustomEventListener(const std::string &eventName, const std::function<void(EventCustom*)>& callback)
{
    auto listener = EventListenerCustom::create(eventName, callback);
    addEventListener(listener);
    return listener;
}

void EventDispatcher::addEventListenerInternal(EventListener::Pointer listener)
{
    EventListenerVector* listeners = nullptr;
    EventListener::ListenerID listenerID = listener->getListenerID();
    auto itr = _listenerMap.find(listenerID);
    if (itr == _listenerMap.end())
    {

        listeners = new (std::nothrow) EventListenerVector();
        _listenerMap.insert(std::make_pair(listenerID, listeners));
    }
    else
    {
        listeners = itr->second;
    }

    listeners->push_back(listener);

    sortEventListenersOfPriority(listenerID);

    auto target = listener->getAssociatedTarget();
    if (target)
    {
        associateTargetAndEventListener(target, listener);
    }

}

void EventDispatcher::removeEventListener(EventListener::Pointer listener)
{
    if (listener == nullptr)
        return;

    bool isFound = false;
    
    auto removeListenerInVector = [&](std::vector<EventListener::Pointer>* listeners)
    {
        if (listeners == nullptr)
            return;
        
        for (auto iter = listeners->begin(); iter != listeners->end(); ++iter)
        {
            auto l = *iter;
            if (l == listener)
            {
                l->setRegistered(false);
                if (l->getAssociatedTarget() != nullptr)
                {
                    dissociateTargetAndEventListener(l->getAssociatedTarget(), l);
                    l->setAssociatedTarget(nullptr);  // nullptr out the node pointer so we don't have any dangling pointers to destroyed nodes.
                }
                listeners->erase(iter);
                isFound = true;
                break;
            }
        }
    };
    
    for (auto iter = _listenerMap.begin(); iter != _listenerMap.end();)
    {
        auto listeners = iter->second;

        if (!isFound)
        {
            removeListenerInVector(listeners);
        }

        if (iter->second->empty())
        {
            auto list = iter->second;
            iter = _listenerMap.erase(iter);
            FK_SAFE_DELETE(list);
        }
        else
        {
            ++iter;
        }
        
        if (isFound)
            break;
    }
}


void EventDispatcher::dispatchEventToListeners(EventListenerVector* listeners, const std::function<bool(EventListener::Pointer)>& onEvent)
{
    if (listeners && !listeners->empty())
    {
        for (auto& l : *listeners)
        {
            if (l->isEnabled() && !l->isPaused() && l->isRegistered() && onEvent(l))
            {
                break;
            }
        }
    }
}


void EventDispatcher::dispatchEvent(Event* event)
{
    if (!_isEnabled)
        return;
    
    if (event->getType() == Event::Type::TOUCH)
    {
        dispatchTouchEvent(static_cast<EventTouch*>(event));
        return;
    }
    
    auto listenerID = __getListenerID(event);

    auto pfnDispatchEventToListeners = &EventDispatcher::dispatchEventToListeners;
    auto iter = _listenerMap.find(listenerID);
    if (iter != _listenerMap.end())
    {
        auto listeners = iter->second;
        
        auto onEvent = [&event](EventListener::Pointer listener) -> bool
        {
            event->setCurrentTarget(listener->getAssociatedTarget());
            listener->_onEvent(event);
            return event->isStopped();
        };
        
        (this->*pfnDispatchEventToListeners)(listeners, onEvent);
    }
    
    updateListeners(event);
}

void EventDispatcher::dispatchCustomEvent(const std::string &eventName, void *optionalUserData)
{
    EventCustom ev(eventName);
    ev.setUserData(optionalUserData);
    dispatchEvent(&ev);
}


void EventDispatcher::dispatchTouchEvent(EventTouch* event)
{
    auto oneByOneListeners = getListeners(EventListenerTouchOneByOne::LISTENER_ID);
    auto allAtOnceListeners = getListeners(EventListenerTouchAllAtOnce::LISTENER_ID);
    
    // If there aren't any touch listeners, return directly.
    if (nullptr == oneByOneListeners && nullptr == allAtOnceListeners)
        return;
    
    bool isNeedsMutableSet = (oneByOneListeners && allAtOnceListeners);
    
    const std::vector<Touch::Pointer>& originalTouches = event->getTouches();
    std::vector<Touch::Pointer> mutableTouches(originalTouches.size());
    std::copy(originalTouches.begin(), originalTouches.end(), mutableTouches.begin());

    //
    // process the target handlers 1st
    //
    if (oneByOneListeners)
    {
        auto mutableTouchesIter = mutableTouches.begin();
        auto touchesIter = originalTouches.begin();
        
        for (; touchesIter != originalTouches.end(); ++touchesIter)
        {
            bool isSwallowed = false;

            auto onTouchEvent = [&](EventListener::Pointer l) -> bool // Return true to break
            { 
                EventListenerTouchOneByOne::Pointer listener = std::static_pointer_cast<EventListenerTouchOneByOne>(l);
                
                // Skip if the listener was removed.
                if (!listener->_isRegistered)
                    return false;
             
                event->setCurrentTarget(listener->_target);
                
                bool isClaimed = false;
                std::vector<Touch::Pointer>::iterator removedIter;
                
                EventTouch::EventCode eventCode = event->getEventCode();
                
                if (eventCode == EventTouch::EventCode::BEGAN)
                {
                    if (listener->onTouchBegan)
                    {
                        isClaimed = listener->onTouchBegan(*touchesIter, event);
                        if (isClaimed && listener->_isRegistered)
                        {
                            listener->_claimedTouches.push_back(*touchesIter);
                        }
                    }
                }
                else if (listener->_claimedTouches.size() > 0
                         && ((removedIter = std::find(listener->_claimedTouches.begin(), listener->_claimedTouches.end(), *touchesIter)) != listener->_claimedTouches.end()))
                {
                    isClaimed = true;
                    
                    switch (eventCode)
                    {
                        case EventTouch::EventCode::MOVED:
                            if (listener->onTouchMoved)
                            {
                                listener->onTouchMoved(*touchesIter, event);
                            }
                            break;
                        case EventTouch::EventCode::ENDED:
                            if (listener->onTouchEnded)
                            {
                                listener->onTouchEnded(*touchesIter, event);
                            }
                            if (listener->_isRegistered)
                            {
                                listener->_claimedTouches.erase(removedIter);
                            }
                            break;
                        case EventTouch::EventCode::CANCELLED:
                            if (listener->onTouchCancelled)
                            {
                                listener->onTouchCancelled(*touchesIter, event);
                            }
                            if (listener->_isRegistered)
                            {
                                listener->_claimedTouches.erase(removedIter);
                            }
                            break;
                        default:
                            FKASSERT(false, "The eventcode is invalid.");
                            break;
                    }
                }
                
                // If the event was stopped, return directly.
                if (event->isStopped())
                {
                    updateListeners(event);
                    return true;
                }
                
                FKASSERT((*touchesIter)->getID() == (*mutableTouchesIter)->getID(),
                         "touchesIter ID should be equal to mutableTouchesIter's ID.");
                
                if (isClaimed && listener->_isRegistered && listener->_needSwallow)
                {
                    if (isNeedsMutableSet)
                    {
                        mutableTouchesIter = mutableTouches.erase(mutableTouchesIter);
                        isSwallowed = true;
                    }
                    return true;
                }
                
                return false;
            };
            
            //
            dispatchEventToListeners(oneByOneListeners, onTouchEvent);
            if (event->isStopped())
            {
                return;
            }
            
            if (!isSwallowed)
                ++mutableTouchesIter;
        }
    }
    
    //
    // process standard handlers 2nd
    //
    if (allAtOnceListeners && mutableTouches.size() > 0)
    {
        
        auto onTouchesEvent = [&](EventListener::Pointer l) -> bool{
            auto listener = std::static_pointer_cast<EventListenerTouchAllAtOnce>(l);
            // Skip if the listener was removed.
            if (!listener->_isRegistered)
                return false;
            
            event->setCurrentTarget(listener->_target);
            
            switch (event->getEventCode())
            {
                case EventTouch::EventCode::BEGAN:
                    if (listener->onTouchesBegan)
                    {
                        listener->onTouchesBegan(mutableTouches, event);
                    }
                    break;
                case EventTouch::EventCode::MOVED:
                    if (listener->onTouchesMoved)
                    {
                        listener->onTouchesMoved(mutableTouches, event);
                    }
                    break;
                case EventTouch::EventCode::ENDED:
                    if (listener->onTouchesEnded)
                    {
                        listener->onTouchesEnded(mutableTouches, event);
                    }
                    break;
                case EventTouch::EventCode::CANCELLED:
                    if (listener->onTouchesCancelled)
                    {
                        listener->onTouchesCancelled(mutableTouches, event);
                    }
                    break;
                default:
                    FKASSERT(false, "The eventcode is invalid.");
                    break;
            }
            
            // If the event was stopped, return directly.
            if (event->isStopped())
            {
                updateListeners(event);
                return true;
            }
            
            return false;
        };
        
        dispatchEventToListeners(allAtOnceListeners, onTouchesEvent);
        if (event->isStopped())
        {
            return;
        }
    }
    
    updateListeners(event);
}

void EventDispatcher::updateListeners(Event* event)
{

    auto onUpdateListeners = [this](const EventListener::ListenerID& listenerID)
    {
        auto listenersIter = _listenerMap.find(listenerID);
        if (listenersIter == _listenerMap.end())
            return;

        auto listeners = listenersIter->second;

        if (listeners)
        {
            for (auto iter = listeners->begin(); iter != listeners->end();)
            {
                auto l = *iter;
                if (!l->isRegistered())
                {
                    iter = listeners->erase(iter);
                }
                else
                {
                    ++iter;
                }
            }
        }

        if (listeners && listeners->empty())
        {
            listeners->clear();
        }
    };

    if (event->getType() == Event::Type::TOUCH)
    {
        onUpdateListeners(EventListenerTouchOneByOne::LISTENER_ID);
        onUpdateListeners(EventListenerTouchAllAtOnce::LISTENER_ID);
    }
    else
    {
        onUpdateListeners(__getListenerID(event));
    }
    
    for (auto iter = _listenerMap.begin(); iter != _listenerMap.end();)
    {
        if (iter->second->empty())
        {
            delete iter->second;
            iter = _listenerMap.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

void EventDispatcher::sortEventListenersOfPriority(const EventListener::ListenerID& listenerID)
{
    auto listeners = getListeners(listenerID);

    if (listeners == nullptr)
        return;
    
    // After sort: priority < 0, > 0
    std::sort(listeners->begin(), listeners->end(), [](const EventListener::Pointer l1, const EventListener::Pointer l2) {
        return l1->getPriority() < l2->getPriority();
    });
}

EventDispatcher::EventListenerVector* EventDispatcher::getListeners(const EventListener::ListenerID& listenerID)
{
    auto iter = _listenerMap.find(listenerID);
    if (iter != _listenerMap.end())
    {
        return iter->second;
    }
    
    return nullptr;
}

void EventDispatcher::removeEventListenersForListenerID(const EventListener::ListenerID& listenerID)
{
    auto listenerItemIter = _listenerMap.find(listenerID);
    if (listenerItemIter != _listenerMap.end())
    {
        auto listeners = listenerItemIter->second;

        auto removeAllListenersInVector = [&](std::vector<EventListener::Pointer>* listenerVector){
            if (listenerVector == nullptr)
                return;
            
            for (auto iter = listenerVector->begin(); iter != listenerVector->end();)
            {
                auto l = *iter;
                l->setRegistered(false);
                if (l->getAssociatedTarget() != nullptr)
                {
                    dissociateTargetAndEventListener(l->getAssociatedTarget(), l);
                    l->setAssociatedTarget(nullptr);  // nullptr out the node pointer so we don't have any dangling pointers to destroyed nodes.
                }
                iter = listenerVector->erase(iter);

            }
        };
        removeAllListenersInVector(listeners);
      
        listeners->clear();
        delete listeners;
        _listenerMap.erase(listenerItemIter);
      
    }
}

void EventDispatcher::removeEventListenersForType(EventListener::Type listenerType)
{
    switch (listenerType)
    {
    case FoundationKit::EventListener::Type::TOUCH_ONE_BY_ONE:
        removeEventListenersForListenerID(EventListenerTouchOneByOne::LISTENER_ID);
        break;
    case FoundationKit::EventListener::Type::TOUCH_ALL_AT_ONCE:
        removeEventListenersForListenerID(EventListenerTouchAllAtOnce::LISTENER_ID);
        break;
    case FoundationKit::EventListener::Type::KEYBOARD:
        removeEventListenersForListenerID(EventListenerKeyboard::LISTENER_ID);
        break;
    case FoundationKit::EventListener::Type::MOUSE:
        removeEventListenersForListenerID(EventListenerMouse::LISTENER_ID);
        break;
    case FoundationKit::EventListener::Type::ACCELERATION:
        removeEventListenersForListenerID(EventListenerAcceleration::LISTENER_ID);
        break;
    case FoundationKit::EventListener::Type::GAME_CONTROLLER:
        removeEventListenersForListenerID(EventListenerController::LISTENER_ID);
        break;
    case FoundationKit::EventListener::Type::CUSTOM:
        FKASSERT(false, "Please call EventDispatcher::removeCustomEventListeners.");
        break;
    default:
        FKASSERT(false, "Invalid listener type!");
        break;
    }
}

void EventDispatcher::removeCustomEventListeners(const std::string& customEventName)
{
    removeEventListenersForListenerID(customEventName);
}

void EventDispatcher::removeAllEventListeners()
{
    std::vector<EventListener::ListenerID> idList;
    for (const auto& e : _listenerMap)
    {
        idList.push_back(e.first);
    }
    for (const auto& id : idList)
    {
        removeEventListenersForListenerID(id);
    }
    _listenerMap.clear();
}

void EventDispatcher::setEnabled(bool isEnabled)
{
    _isEnabled = isEnabled;
}

bool EventDispatcher::isEnabled() const
{
    return _isEnabled;
}



NS_FK_END
