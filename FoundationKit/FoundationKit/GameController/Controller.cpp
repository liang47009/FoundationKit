
#include "Controller.h"

#if (FK_TARGET_PLATFORM == FK_PLATFORM_ANDROID || FK_TARGET_PLATFORM == FK_PLATFORM_IOS)

#include "FoundationKit/Events/EventDispatcher.h"
#include "FoundationKit/Events/EventController.h"


NS_FK_BEGIN

std::vector<Controller*> Controller::s_allController;

Controller* Controller::getControllerByTag(int tag)
{
    for (auto controller:Controller::s_allController)
    {
        if (controller->_controllerTag == tag)
        {
            return controller;
        }
    }
    return nullptr;
}

void Controller::init()
{
    for (int key = Key::JOYSTICK_LEFT_X; key < Key::KEY_MAX; ++key)
    {
        _allKeyStatus[key].isPressed = false;
        _allKeyStatus[key].value = 0.0f;

        _allKeyPrevStatus[key].isPressed = false;
        _allKeyPrevStatus[key].value = 0.0f;
    }

    _eventDispatcher = EventDispatcher::getInstance();
    _connectEvent = new (std::nothrow) EventController(EventController::ControllerEventType::CONNECTION, this, false);
    _keyEvent = new (std::nothrow) EventController(EventController::ControllerEventType::BUTTON_STATUS_CHANGED, this, 0);
    _axisEvent = new (std::nothrow) EventController(EventController::ControllerEventType::AXIS_STATUS_CHANGED, this, 0);
}

const Controller::KeyStatus& Controller::getKeyStatus(int keyCode)
{
    if (_allKeyStatus.find(keyCode) == _allKeyStatus.end())
    {
        _allKeyStatus[keyCode].isPressed = false;
        _allKeyStatus[keyCode].value = 0.0f;
    }

    return _allKeyStatus[keyCode];
}

void Controller::onConnected()
{
    _connectEvent->setConnectStatus(true);
    _eventDispatcher->dispatchEvent(_connectEvent);
}

void Controller::onDisconnected()
{
    _connectEvent->setConnectStatus(false);
    _eventDispatcher->dispatchEvent(_connectEvent);

    delete this;
}

void Controller::onButtonEvent(int keyCode, bool isPressed, float value, bool isAnalog)
{
    _allKeyPrevStatus[keyCode] = _allKeyStatus[keyCode];
    _allKeyStatus[keyCode].isPressed = isPressed;
    _allKeyStatus[keyCode].value = value;
    _allKeyStatus[keyCode].isAnalog = isAnalog;

    _keyEvent->setKeyCode(keyCode);
    _eventDispatcher->dispatchEvent(_keyEvent);
}

void Controller::onAxisEvent(int axisCode, float value, bool isAnalog)
{
    _allKeyPrevStatus[axisCode] = _allKeyStatus[axisCode];
    _allKeyStatus[axisCode].value = value;
    _allKeyStatus[axisCode].isAnalog = isAnalog;

    _axisEvent->setKeyCode(axisCode);
    _eventDispatcher->dispatchEvent(_axisEvent);
}


NS_FK_END

#endif // #if (FK_TARGET_PLATFORM == FK_PLATFORM_ANDROID)
