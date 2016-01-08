

#include "EventKeyboard.h"

NS_FK_BEGIN

EventKeyboard::EventKeyboard(KeyCode keyCode, int keyValue, bool isPressed)
: Event(Type::KEYBOARD)
, _keyCode(keyCode)
, _keyValue(0)
, _isPressed(isPressed)
{}

NS_FK_END
