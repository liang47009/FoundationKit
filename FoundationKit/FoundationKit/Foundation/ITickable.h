#ifndef LOSEMYMIND_ITICKABLE_H
#define LOSEMYMIND_ITICKABLE_H



#include "FoundationKit/GenericPlatformMacros.h"

NS_FK_BEGIN

class ITickable
{
public:
    /**
     * Pure virtual that must be overloaded by the inheriting class.
     *
     * @param deltaTime	App time passed since the last call.
     */
    virtual void tick(float deltaTime) = 0;

    /**
     * Pure virtual that must be overloaded by the inheriting class. It is
     * used to determine whether an object is ready to be ticked. 
     *
     * @return	true if class is ready to be ticked, false otherwise.
     */
    virtual bool isTickable() const = 0;

};


NS_FK_END


#endif // LOSEMYMIND_ITICKABLE_H
