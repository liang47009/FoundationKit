
#include "EventListenerController.h"
#include "EventController.h"
#include "GameController.h"

NS_FK_BEGIN

const std::string EventListenerController::LISTENER_ID = "losemymind.libo.GamepadControllerListener";

EventListenerController::Pointer EventListenerController::create()
{
    auto ret = new (std::nothrow) EventListenerController();
    if (ret && ret->init())
    {

    }
    else
    {
        FK_SAFE_DELETE(ret);
    }
    return EventListenerController::Pointer(ret);

}

bool EventListenerController::init()
{
    auto listener = [this](Event* event){
        auto evtController = static_cast<EventController*>(event);
        switch (evtController->getControllerEventType())
        {
        case EventController::ControllerEventType::CONNECTION:
            if (evtController->isConnected())
            {
                if (this->onConnected)
                    this->onConnected(evtController->getController(), event);
            }
            else
            {
                if (this->onDisconnected)
                    this->onDisconnected(evtController->getController(), event);
            }
            break;
        case EventController::ControllerEventType::BUTTON_STATUS_CHANGED:
            {
                const auto&  keyStatus = evtController->_controller->_allKeyStatus[evtController->_keyCode];
                const auto&  keyPrevStatus = evtController->_controller->_allKeyPrevStatus[evtController->_keyCode];

                if (this->onKeyDown && keyStatus.isPressed && !keyPrevStatus.isPressed)
                {
                    this->onKeyDown(evtController->_controller, evtController->_keyCode, event);
                }
                else if (this->onKeyUp && !keyStatus.isPressed && keyPrevStatus.isPressed)
                {
                    this->onKeyUp(evtController->_controller, evtController->_keyCode, event);
                }
                else if (this->onKeyRepeat && keyStatus.isPressed && keyPrevStatus.isPressed)
                {
                    this->onKeyRepeat(evtController->_controller, evtController->_keyCode, event);
                }
            }
            break;
        case EventController::ControllerEventType::AXIS_STATUS_CHANGED:
            {
                if (this->onAxisEvent)
                {
                    this->onAxisEvent(evtController->_controller, evtController->_keyCode, event);
                }
            }
            break;
        default:
            FKASSERT(false, "Invalid EventController type");
            break;
        }
    };

    if (EventListener::init(EventListener::Type::GAME_CONTROLLER, LISTENER_ID, listener))
    {
        return true;
    }
    return false;
}

bool EventListenerController::checkAvailable()
{
    return true;
}


NS_FK_END