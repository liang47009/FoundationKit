

#include "EventKeyboard.h"

NS_FK_BEGIN

EventKeyboard::EventKeyboard(KeyCode keyCode, bool isPressed)
: Event(Type::KEYBOARD)
, _keyCode(keyCode)
, _isPressed(isPressed)
{}

NS_FK_END
