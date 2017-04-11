
#include "FoundationKit/Foundation/Size.hpp"

NS_FK_BEGIN

void Size::setSize(float width, float height)
{
    this->width = width;
    this->height = height;
}

bool Size::equals(const Size& target) const
{
    return (fabs(this->width - target.width) < FLT_EPSILON)
        && (fabs(this->height - target.height) < FLT_EPSILON);
}

const Size Size::ZERO = Size(0, 0);


NS_FK_END