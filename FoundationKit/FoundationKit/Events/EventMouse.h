#ifndef FoundationKit_EventMouse_H
#define FoundationKit_EventMouse_H


#include "FoundationKit/Events/Event.h"
#include "FoundationKit/Math/Vector2.h"

#define MOUSE_BUTTON_LEFT       0
#define MOUSE_BUTTON_RIGHT      1
#define MOUSE_BUTTON_MIDDLE     2
#define MOUSE_BUTTON_4          3
#define MOUSE_BUTTON_5          4
#define MOUSE_BUTTON_6          5
#define MOUSE_BUTTON_7          6
#define MOUSE_BUTTON_8          7

NS_FK_BEGIN

class EventMouse : public Event
{
public:
    /**
    * MouseEventType Different types of MouseEvent.
    */
    enum class MouseEventType
    {
        MOUSE_NONE,
        MOUSE_DOWN,
        MOUSE_UP,
        MOUSE_MOVE,
        MOUSE_SCROLL,
    };

    /** Constructor.
     *
     * @param mouseEventCode A given mouse event type.
     * @js ctor
     */
    explicit EventMouse(MouseEventType mouseEventCode);

    /** Set mouse scroll data.
     * 
     * @param scrollX The scroll data of x axis.
     * @param scrollY The scroll data of y axis.
     */
    inline void setScrollData(float scrollX, float scrollY) { _scrollX = scrollX; _scrollY = scrollY; };
    /** Get mouse scroll data of x axis.
     * 
     * @return The scroll data of x axis.
     */
    inline float getScrollX() const { return _scrollX; };
    /** Get mouse scroll data of y axis.
     *
     * @return The scroll data of y axis.
     */
    inline float getScrollY() const { return _scrollY; };

    /** Set the cursor position.
     *
     * @param x The x coordinate of cursor position.
     * @param y The y coordinate of cursor position.
     */
    inline void setCursorPosition(float x, float y) { 
        _x = x;
        _y = y;
        _prevPoint = _point;
        _point.x = x;
        _point.y = y;
        if (!_startPointCaptured)
        {
            _startPoint = _point;
            _startPointCaptured = true;
        }
    }

    /** Set mouse button.
     * 
     * @param button a given mouse button.
     */
    inline void setMouseButton(int button) { _mouseButton = button; };
    /** Get mouse button.
     *
     * @return The mouse button.
     */
    inline int getMouseButton() const { return _mouseButton; };
    /** Get the cursor position of x axis.
     *
     * @return The x coordinate of cursor position.
     */
    inline float getCursorX() const { return _x; };
    /** Get the cursor position of y axis.
     *
     * @return The y coordinate of cursor position.
     */
    inline float getCursorY() const { return _y; };

    /** Returns the current touch location  coordinates.
     *  Need convert to OpenGL point
     * @return The current touch location   coordinates.
     */
    Vector2 getLocation() const;
    /** Returns the previous touch location coordinates.
     *  Need convert to OpenGL point 
     * @return The previous touch location coordinates.
     */
    Vector2 getPreviousLocation() const;
    /** Returns the start touch location   coordinates.
     *  Need convert to OpenGL point
     * @return The start touch location  coordinates.
     */
    Vector2 getStartLocation() const;
    /** Returns the delta of 2 current touches locations in screen coordinates.
     *
     * @return The delta of 2 current touches locations in screen coordinates.
     */
    Vector2 getDelta() const;
    /** Returns the current touch location in screen coordinates.
     *
     * @return The current touch location in screen coordinates.
     */
    Vector2 getLocationInView() const;
    /** Returns the previous touch location in screen coordinates.
     *
     * @return The previous touch location in screen coordinates.
     */
    Vector2 getPreviousLocationInView() const;
    /** Returns the start touch location in screen coordinates.
     *
     * @return The start touch location in screen coordinates.
     */
    Vector2 getStartLocationInView() const;


private:
    MouseEventType _mouseEventType;
    int _mouseButton;
    float _x;
    float _y;
    float _scrollX;
    float _scrollY;

    bool _startPointCaptured;
    Vector2 _startPoint;
    Vector2 _point;
    Vector2 _prevPoint;

    friend class EventListenerMouse;
};

NS_FK_END

#endif // FoundationKit_EventMouse_H

