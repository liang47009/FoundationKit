/****************************************************************************
 Copyright (c) 2015 libo All rights reserved.

 losemymind.libo@gmail.com

****************************************************************************/

#ifndef LOSEMYMIND_FOUNDATIONMACROS_H
#define LOSEMYMIND_FOUNDATIONMACROS_H
#pragma once

#include "GenericPlatformMacros.h"

#define FUN_BIND_0(__selector__,__target__, ...) std::bind(&__selector__,__target__, ##__VA_ARGS__)
#define FUN_BIND_1(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, ##__VA_ARGS__)
#define FUN_BIND_2(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, ##__VA_ARGS__)
#define FUN_BIND_3(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, ##__VA_ARGS__)

#define UNUSED_ARG(arg) do{(void)(arg);}while(0)

#define SAFE_DELETE(p)           do { delete (p); (p) = nullptr; } while(0)
#define SAFE_DELETE_ARRAY(p)     do { if(p) { delete[] (p); (p) = nullptr; } } while(0)
#define SAFE_FREE(p)             do { if(p) { free(p); (p) = nullptr; } } while(0)
#define SAFE_RELEASE(p)          do { if(p) { (p)->release(); } } while(0)
#define SAFE_RELEASE_NULL(p)     do { if(p) { (p)->release(); (p) = nullptr; } } while(0)
#define SAFE_RETAIN(p)           do { if(p) { (p)->retain(); } } while(0)
#define BREAK_IF(cond)           if(cond) break


/** @def PROPERTY_DEFINE
 * It is used to declare a protected variable.
 * We can use getter to read the variable, and use the setter to change the variable.
 *
 * @param varType     The type of variable.
 * @param varName     Variable name.
 * @param funName     "get + funName" will be the name of the getter.
 *                    "set + funName" will be the name of the setter.
 * @warning   The getter and setter are public virtual functions, you should rewrite them first.
 *            The variables and methods declared after PROPERTY_DEFINE are all public.
 *            If you need protected or private, please declare.
 */
#define PROPERTY_DEFINE(varType, varName, funName)\
protected: varType varName;\
public: virtual varType get##funName(void) const;\
public: virtual void set##funName(varType var);

/** @def PROPERTY_DEFINE_REF
 * It is used to declare a protected variable.
 * We can use getter to read the variable, and use the setter to change the variable.
 *
 * @param varType     The type of variable.
 * @param varName     Variable name.
 * @param funName     "get + funName" will be the name of the getter.
 *                    "set + funName" will be the name of the setter.
 * @warning   The getter and setter are public virtual functions, you should rewrite them first.
 *            The variables and methods declared after PROPERTY_DEFINE_REF are all public.
 *            If you need protected or private, please declare.
 */
#define PROPERTY_DEFINE_REF(varType, varName, funName)\
protected: varType varName;\
public: virtual const varType& get##funName(void) const;\
public: virtual void set##funName(const varType& var);

/** @def PROPERTY_DEFINE_READONLY
 * It is used to declare a protected variable.
 * We can use getter to read the variable, and use the setter to change the variable.
 *
 * @param varType     The type of variable.
 * @param varName     Variable name.
 * @param funName     "get + funName" will be the name of the getter.
 * @warning   The getter and setter are public virtual functions, you should rewrite them first.
 *            The variables and methods declared after PROPERTY_DEFINE_READONLY are all public.
 *            If you need protected or private, please declare.
 */
#define PROPERTY_DEFINE_READONLY(varType, varName, funName)\
protected: varType varName;\
public: virtual varType get##funName(void) const;

/** @def PROPERTY_DEFINE_REF_READONLY
 * It is used to declare a protected variable.
 * We can use getter to read the variable, and use the setter to change the variable.
 *
 * @param varType     The type of variable.
 * @param varName     Variable name.
 * @param funName     "get + funName" will be the name of the getter.
 * @warning   The getter and setter are public virtual functions, you should rewrite them first.
 *            The variables and methods declared after PROPERTY_DEFINE_REF_READONLY are all public.
 *            If you need protected or private, please declare.
 */
#define PROPERTY_DEFINE_REF_READONLY(varType, varName, funName)\
protected: varType varName;\
public: virtual const varType& get##funName(void) const;


/** @def PROPERTY_SYNTHESIZE
 * It is used to declare a protected variable.
 * We can use getter to read the variable, and use the setter to change the variable.
 *
 * @param varType     The type of variable.
 * @param varName     Variable name.
 * @param funName     "get + funName" will be the name of the getter.
 *                    "set + funName" will be the name of the setter.
 * @warning   The getter and setter are public inline functions.
 *            The variables and methods declared after PROPERTY_SYNTHESIZE are all public.
 *            If you need protected or private, please declare.
 */
#define PROPERTY_SYNTHESIZE(varType, varName, funName)\
protected: varType varName;\
public: virtual varType get##funName(void) const { return varName; }\
public: virtual void set##funName(varType var){ varName = var; }

/** @def PROPERTY_SYNTHESIZE_REF
 * It is used to declare a protected variable.
 * We can use getter to read the variable, and use the setter to change the variable.
 *
 * @param varType     The type of variable.
 * @param varName     Variable name.
 * @param funName     "get + funName" will be the name of the getter.
 *                    "set + funName" will be the name of the setter.
 * @warning   The getter and setter are public inline functions.
 *            The variables and methods declared after PROPERTY_SYNTHESIZE_REF are all public.
 *            If you need protected or private, please declare.
 */
#define PROPERTY_SYNTHESIZE_REF(varType, varName, funName)\
protected: varType varName;\
public: virtual const varType& get##funName(void) const { return varName; }\
public: virtual void set##funName(const varType& var){ varName = var; }

/** @def PROPERTY_SYNTHESIZE_READONLY
 * It is used to declare a protected variable.
 * We can use getter to read the variable, and use the setter to change the variable.
 *
 * @param varType     The type of variable.
 * @param varName     Variable name.
 * @param funName     "get + funName" will be the name of the getter.
 * @warning   The getter and setter are public inline functions.
 *            The variables and methods declared after PROPERTY_SYNTHESIZE_READONLY are all public.
 *            If you need protected or private, please declare.
 */
#define PROPERTY_SYNTHESIZE_READONLY(varType, varName, funName)\
protected: varType varName;\
public: virtual varType get##funName(void) const { return varName; }

/** @def PROPERTY_SYNTHESIZE_REF_READONLY
 * It is used to declare a protected variable.
 * We can use getter to read the variable, and use the setter to change the variable.
 *
 * @param varType     The type of variable.
 * @param varName     Variable name.
 * @param funName     "get + funName" will be the name of the getter.
 * @warning   The getter and setter are public inline functions.
 *            The variables and methods declared after PROPERTY_SYNTHESIZE_REF_READONLY are all public.
 *            If you need protected or private, please declare.
 */
#define PROPERTY_SYNTHESIZE_REF_READONLY(varType, varName, funName)\
protected: varType varName;\
public: virtual const varType& get##funName(void) const { return varName; }











#endif // LOSEMYMIND_FOUNDATIONMACROS_H

