/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_ITICKABLE_HPP
#define FOUNDATIONKIT_ITICKABLE_HPP

#include "FoundationKit/GenericPlatformMacros.hpp"

NS_FK_BEGIN

class ITickable
{
public:
   /**
    * Pure virtual that must be overloaded by the inheriting class.
    *
    * @param deltaTime	App time passed since the last call.
    */
    virtual void Tick(float deltaTime) = 0;

   /**
    * Pure virtual that must be overloaded by the inheriting class. It is
    * used to determine whether an object is ready to be ticked.
    *
    * @return	true if class is ready to be ticked, false otherwise.
    */
    virtual bool IsTickable() const = 0;

};


NS_FK_END

#endif // END OF FOUNDATIONKIT_ITICKABLE_HPP
