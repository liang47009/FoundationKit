
#include "EventController.h"

NS_FK_BEGIN

EventController::EventController(ControllerEventType type, Controller* controller, int keyCode)
: Event(Type::GAME_CONTROLLER)
, _controllerEventType(type)
, _controller(controller)
, _keyCode(keyCode)
, _isConnected(true)
{
    
}

EventController::EventController(ControllerEventType type, Controller* controller, bool isConnected)
: Event(Type::GAME_CONTROLLER)
, _controllerEventType(type)
, _controller(controller)
, _keyCode(0)
, _isConnected(isConnected)
{
    
}

EventController::~EventController()
{
}

NS_FK_END
