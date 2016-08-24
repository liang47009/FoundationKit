

#include "EventKeyboard.h"

NS_FK_BEGIN

EventKeyboard::EventKeyboard(KeyCode keyCode, int keyValue, bool isPressed)
: Event(Type::KEYBOARD)
, _keyCode(keyCode)
, _keyValue(keyValue)
, _isPressed(isPressed)
{}

EventKeyboard::EventKeyboard()
    :EventKeyboard(KeyCode::KEY_NONE, -1, false)
{}

const EventKeyboard::KeyCode EventKeyboard::keyCode()const
{
    return _keyCode;
}

const int EventKeyboard::keyValue()const
{
    return _keyValue;
}

const bool EventKeyboard::isPressed()const
{
    return _isPressed;
}

NS_FK_END
