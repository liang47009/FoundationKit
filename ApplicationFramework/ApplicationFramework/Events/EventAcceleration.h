#ifndef FoundationKit_EventAcceleration_H
#define FoundationKit_EventAcceleration_H



#include "Event.h"

NS_FK_BEGIN

/** @struct Acceleration
 * The device accelerometer reports values for each axis in units of g-force.
 */
class Acceleration
{
public:
    double x;
    double y;
    double z;
    
    double timestamp;
    
    Acceleration(): x(0), y(0), z(0), timestamp(0) {}
};

/** @class EventAcceleration
 * @brief Accelerometer event.
 */
class EventAcceleration : public Event
{
public:
    /** Constructor.
     *
     * @param acc A given Acceleration.
     */
    explicit EventAcceleration(const Acceleration& acc);
    
private:
    Acceleration _acc;
    friend class EventListenerAcceleration;
};

NS_FK_END

#endif // FoundationKit_EventAcceleration_H

