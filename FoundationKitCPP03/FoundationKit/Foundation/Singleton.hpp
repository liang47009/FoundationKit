/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_SINGLETON_HPP
#define FOUNDATIONKIT_SINGLETON_HPP

#pragma once
#include <stdlib.h>
#include "FoundationKit/GenericPlatformMacros.hpp"
#include "FoundationKit/Base/noncopyable.hpp"

NS_FK_BEGIN

template< typename T, bool destroy_on_exit = true >
class Singleton : noncopyable
{
    typedef T                         instance_type;
    typedef instance_type*            instance_pointer;
    typedef volatile instance_pointer volatile_instance_pointer;
    static volatile_instance_pointer m_instance;
    static void Destroy();
protected:
    Singleton(){};
public:
    inline static instance_pointer GetInstance()
    {
        if(!m_instance)
        {
            m_instance = new instance_type();
            if (destroy_on_exit)
            {
                ::atexit(Destroy);
            }
        }
        return m_instance;
    }

    inline static void DestroyInstance()
    {
        if (!destroy_on_exit)
        {
            Destroy();
        }
    }
};


template< typename T, bool destroy_on_exit >
void Singleton< T, destroy_on_exit>::Destroy()
{
    typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
    T_must_be_complete_type dummy; (void)dummy;
    delete m_instance;
    m_instance = nullptr;
}

template< typename T, bool destroy_on_exit >
typename Singleton< T, destroy_on_exit >::volatile_instance_pointer
    Singleton<T, destroy_on_exit>::m_instance = nullptr;

NS_FK_END

#endif // FOUNDATIONKIT_SINGLETON_HPP
