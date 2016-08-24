/****************************************************************************
 Copyright (c) 2016 libo All rights reserved.

 losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_STD_H
#define  FOUNDATIONKIT_STD_H

#include "FoundationKit/stdextensions/stdconfig.h"

#if (CPP_TARGET_VERSION >= CPP_VERSION_11)
#include <memory>
#include <functional>
#include <mutex>
#include <thread>
#include <condition_variable>

#define FUN_BIND0(_fun_type,__target__,__selector__) std::bind(&__selector__,__target__)
#define FUN_BIND1(_fun_type,__target__,__selector__) std::bind(&__selector__,__target__, std::placeholders::_1)
#define FUN_BIND2(_fun_type,__target__,__selector__) std::bind(&__selector__,__target__, std::placeholders::_1,std::placeholders::_2)
#define FUN_BIND3(_fun_type,__target__,__selector__) std::bind(&__selector__,__target__, std::placeholders::_1,std::placeholders::_2,std::placeholders::_3)
#define FUN_BIND4(_fun_type,__target__,__selector__) std::bind(&__selector__,__target__, std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4)
#define FUN_BIND6(_fun_type,__target__,__selector__) std::bind(&__selector__,__target__, std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4,std::placeholders::_5)

#else //#if !SUPPORT_CPP11

#include "FoundationKit/stdextensions/memory.h"     // for std::shared_ptr
#include "FoundationKit/stdextensions/functional.h" // for std::function
#include "FoundationKit/stdextensions/tinythread.h" // for std::lock_guard  std::mutex

#define FUN_BIND0(_fun_type,__target__,__selector__, ...) std::function<_fun_type>(__target__, &__selector__)
#define FUN_BIND1(_fun_type,__target__,__selector__, ...) std::function<_fun_type>(__target__, &__selector__)
#define FUN_BIND2(_fun_type,__target__,__selector__, ...) std::function<_fun_type>(__target__, &__selector__)
#define FUN_BIND3(_fun_type,__target__,__selector__, ...) std::function<_fun_type>(__target__, &__selector__)
#define FUN_BIND4(_fun_type,__target__,__selector__, ...) std::function<_fun_type>(__target__, &__selector__)
#define FUN_BIND5(_fun_type,__target__,__selector__, ...) std::function<_fun_type>(__target__, &__selector__)


#endif //#if SUPPORT_CPP11

#endif