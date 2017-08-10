
#include "FoundationKit/Foundation/Size.hpp"

NS_FK_BEGIN

void Size::SetSize(float width, float height)
{
    this->width = width;
    this->height = height;
}

bool Size::Equals(const Size& target) const
{
    return (fabs(this->width - target.width) < FLT_EPSILON)
        && (fabs(this->height - target.height) < FLT_EPSILON);
}



NS_FK_END