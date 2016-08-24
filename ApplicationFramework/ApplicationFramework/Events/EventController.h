#pragma once

#include "Event.h"

NS_FK_BEGIN

class Controller;
class EventListenerController;

class EventController : public Event
{
public:
    /** ControllerEventType Controller event type.*/
    enum class ControllerEventType
    {
        CONNECTION,
        BUTTON_STATUS_CHANGED,
        AXIS_STATUS_CHANGED,
    };
    
    /** Create a EventController with controller event type, contrlloer and key code.
     *
     * @param type A given controller event type.
     * @param controller A given controller pointer.
     * @param keyCode A given key code.
     * @return An autoreleased EventController object.
     */
	EventController(ControllerEventType type, Controller* controller, int keyCode);
    /** Create a EventController with controller event type, contrlloer and whether or not is connected.
     *
     * @param type A given controller event type.
     * @param controller A given controller pointer.
     * @param isConnected True if it is connected.
     * @return An autoreleased EventController object.
     */
    EventController(ControllerEventType type, Controller* controller, bool isConnected);
    
    virtual ~EventController();

    /** Gets the event type of the controller.
     *
     * @return The event type of the controller.
     */
    ControllerEventType getControllerEventType() const { return _controllerEventType; }
    Controller* getController() const { return _controller; }

    /** Gets the key code of the controller.
     *
     * @return The key code of the controller.
     */
    int getKeyCode() const{ return _keyCode; }
    void setKeyCode(int keyCode) { _keyCode = keyCode;}

    /** Sets the connect status.
     *
     * @param True if it's connected.
     */
    void setConnectStatus(bool isConnected) {_isConnected = isConnected;}
    /** Gets the connect status.
     *
     * @return True if it's connected.
     */
    bool isConnected() const { return _isConnected; }
    
protected:
    ControllerEventType _controllerEventType;
    Controller* _controller;
    int _keyCode;
    bool _isConnected;

    friend class EventListenerController;
};

NS_FK_END

