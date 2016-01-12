#pragma once
#include "FoundationKit/FoundationKitDefines.h"
NS_FK_BEGIN

/**
 *   Base class of all kinds of events.
 */
class Event
{
public:

	enum class Type
	{
		TOUCH,
		KEYBOARD,
		ACCELERATION,
		MOUSE,
		GAME_CONTROLLER,
		CUSTOM
	};
	
protected:
	/** Constructor */
	explicit Event(Type type);
public:
	/** Destructor */
	virtual ~Event();

	/** Gets the event type */
	inline Type getType() const { return _type; };
	
	/** Stops propagation for current event */
	inline void stopPropagation() { _isStopped = true; };
	
	/** Checks whether the event has been stopped */
	inline bool isStopped() const { return _isStopped; };
	
	/** @brief Gets current target of the event
	 *  @return The target with which the event associates.
	 *  @note It onlys be available when the event listener is associated with node. 
	 *        It returns 0 when the listener is associated with fixed priority.
	 */
	inline void* getCurrentTarget() { return _currentTarget; };
	
protected:
	/** Sets current target */
	inline void setCurrentTarget(void* target) { _currentTarget = target; };
	
	Type _type;     ///< Event type
	
	bool  _isStopped;       ///< whether the event has been stopped.
	void* _currentTarget;  ///< Current target
	
	friend class EventDispatcher;
};

NS_FK_END