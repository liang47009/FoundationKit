

#include "EventAcceleration.h"

NS_FK_BEGIN

EventAcceleration::EventAcceleration(const Acceleration& acc)
: Event(Type::ACCELERATION)
, _acc(acc)
{
}

NS_FK_END
