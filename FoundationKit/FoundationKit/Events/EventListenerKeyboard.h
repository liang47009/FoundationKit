#pragma once
#include "EventListener.h"
#include "EventKeyboard.h"

NS_FK_BEGIN

class Event;

class EventListenerKeyboard : public EventListener
{
public:
	typedef std::shared_ptr<EventListenerKeyboard>    Pointer;

	static const std::string LISTENER_ID;
	
	static EventListenerKeyboard::Pointer create();
	
	virtual bool checkAvailable() override;
	
   /**
	*@brief Keyboard event delegate
	*@param EventKeyboard::KeyCode  The key code.
	*@param int The key value.
	*@param Event*  The keyboard event object.
	*/
	typedef std::function<void(EventKeyboard::KeyCode, int, Event*)> KeyboardEventDelegate;
	KeyboardEventDelegate onKeyPressed;
	KeyboardEventDelegate onKeyReleased;
	KeyboardEventDelegate onKeyChar;
protected:
	EventListenerKeyboard();
	bool init();
};

NS_FK_END
