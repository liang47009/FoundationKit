#ifndef FoundationKit_Touch_H
#define FoundationKit_Touch_H


#include "FoundationKit/Base/Vector2.h"
#include <memory>

NS_FK_BEGIN


/** @class Touch
 * @brief Encapsulates the Touch information, such as touch point, id and so on,
 and provides the methods that commonly used.
 */
class Touch
{
public:
    typedef std::shared_ptr<Touch>  Pointer;
    /** 
     * Dispatch mode, how the touches are dispathced.
     */
    enum class DispatchMode 
    {
        ALL_AT_ONCE, /** All at once. */
        ONE_BY_ONE,  /** One by one. */
    };

    /** Constructor.
     */
    Touch() 
        : _id(0),
        _startPointCaptured(false)
    {}

    /** Returns the current touch location coordinates.
     *  need covert to OpenGL
     *
     * @return The current touch location coordinates.
     */
    Vector2 getLocation() const;
    /** Returns the previous touch location  coordinates.
     *  need covert to OpenGL
     *
     * @return The previous touch location coordinates.
     */
    Vector2 getPreviousLocation() const;
    /** Returns the start touch location  coordinates.
     *  need covert to OpenGL
     *
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
    
    /** Set the touch infomation. It always used to monitor touch event.
     *
     * @param id A given id
     * @param x A given x coordinate.
     * @param y A given y coordinate.
     */
    void setTouchInfo(int id, float x, float y)
    {
        _id = id;
        _prevPoint = _point;
        _point.x   = x;
        _point.y   = y;
        if (!_startPointCaptured)
        {
            _startPoint = _point;
            _startPointCaptured = true;
            _prevPoint = _point;
        }
    }
    /** Get touch id.
     * @js getId
     * @lua getId
     *
     * @return The id of touch.
     */
    int getID() const
    {
        return _id;
    }

private:
    int _id;
    bool _startPointCaptured;
    Vector2 _startPoint;
    Vector2 _point;
    Vector2 _prevPoint;
};


NS_FK_END

#endif // FoundationKit_Touch_H

