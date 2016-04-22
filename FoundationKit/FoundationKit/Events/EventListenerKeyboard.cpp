

#include "EventListenerKeyboard.h"


NS_FK_BEGIN

const std::string EventListenerKeyboard::LISTENER_ID = "losemymind.libo.KeyboardListener";

bool EventListenerKeyboard::checkAvailable()
{
	if (onKeyPressed == nullptr && onKeyReleased == nullptr && onKeyChar == nullptr)
	{
		return false;
	}
	
	return true;
}

EventListenerKeyboard::Pointer EventListenerKeyboard::create()
{
	auto ret = new (std::nothrow) EventListenerKeyboard();
	if (ret && ret->init())
	{
		
	}
	else
	{
		SAFE_DELETE(ret);
	}
	return EventListenerKeyboard::Pointer(ret);
}

EventListenerKeyboard::EventListenerKeyboard()
: onKeyPressed(nullptr)
, onKeyReleased(nullptr)
, onKeyChar(nullptr)
{
}

bool EventListenerKeyboard::init()
{
	auto listener = [this](Event* event){
		auto keyboardEvent = static_cast<EventKeyboard*>(event);
		if (keyboardEvent->_isPressed)
		{
			if (onKeyPressed != nullptr)
				onKeyPressed(keyboardEvent->_keyCode, keyboardEvent->_keyValue, event);
		}
		else
		{
			if (onKeyReleased != nullptr)
				onKeyReleased(keyboardEvent->_keyCode, keyboardEvent->_keyValue, event);
		}
	};
	return EventListener::init(Type::KEYBOARD, LISTENER_ID, listener);
}

NS_FK_END
