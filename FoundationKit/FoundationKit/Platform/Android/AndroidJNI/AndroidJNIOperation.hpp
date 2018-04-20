/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_ANDROIDJNIOPERATION_HPP
#define FOUNDATIONKIT_ANDROIDJNIOPERATION_HPP

#include "FoundationKit/GenericPlatformMacros.hpp"
#if PLATFORM_ANDROID

#include <memory>
#include <string>
#include <stdarg.h>
#include <jni.h>
#include "AndroidJNI.hpp"
//#include "AndroidFoundation.hpp"

NS_FK_BEGIN
namespace android 
{
    //Compile Time String
    template <char... Chars>
    struct FixedChars
    {
        static const char * value()
        {
            static const char char_array[sizeof...(Chars)] = { Chars... };
            return char_array;
        }
    };

    template <class L, class R>
    struct FixedChars2;

    template <char... LC, char... RC>
    struct FixedChars2< FixedChars<LC...>, FixedChars<RC...> >
    {
        using Result = FixedChars < LC..., RC... >;
    };

    template <typename ...FC>
    struct CombineFixedChars;

    template <typename FC>
    struct CombineFixedChars < FC >
    {
        using Result = FC;
    };

    template <typename FC1, typename ...FC>
    struct CombineFixedChars < FC1, FC... >
    {
        using Result = typename FixedChars2<FC1, typename CombineFixedChars<FC...>::Result>::Result;
    };

 typedef void* jvoid; // make it possible to return void

#define JNI_CALL(parameters, check_exception, function)                                                 \
	JNIEnv* env = AndroidJNI::GetJNIEnv();                                                              \
	function

#define JNI_CALL_RETURN(type, parameters, check_exception, function)                                    \
	JNIEnv* env = AndroidJNI::GetJNIEnv();                                                              \
	type JNI_CALL_result = function;                                                                    \
	return JNI_CALL_result;

#define JNI_CALL_DECLARE(type, result, parameters, check_exception, function)                           \
	JNIEnv* env = AndroidJNI::GetJNIEnv();                                                              \
	type result = function;


#if defined(__NDK_FPABI__)
#	define	JNITL_FUNCTION_ATTRIBUTES __NDK_FPABI__
#else
#	define	JNITL_FUNCTION_ATTRIBUTES
#endif

template <typename JT, typename RT,
	RT(JNITL_FUNCTION_ATTRIBUTES JNIEnv::* CallMethodOP)(jobject, jmethodID, va_list),
	RT(JNITL_FUNCTION_ATTRIBUTES JNIEnv::* CallNonvirtualMethodOP)(jobject, jclass, jmethodID, va_list),
	RT(JNITL_FUNCTION_ATTRIBUTES JNIEnv::* CallStaticMethodOP)(jclass, jmethodID, va_list)
>
class MethodOps
{
public:
	static JT CallMethod(jobject object, jmethodID id, ...)
	{
		va_list args;
		va_start(args, id);
		JNI_CALL_DECLARE(JT, result, object && id, true, static_cast<JT>((env->*CallMethodOP)(object, id, args)));
		va_end(args);
		return result;
	}
	static JT CallNonVirtualMethod(jobject object, jclass clazz, jmethodID id, ...)
	{
		va_list args;
		va_start(args, id);
		JNI_CALL_DECLARE(JT, result, object && clazz && id, true, static_cast<JT>((env->*CallNonvirtualMethodOP)(object, clazz, id, args)));
		va_end(args);
		return result;
	}
	static JT CallStaticMethod(jclass clazz, jmethodID id, ...)
	{
		va_list args;
		va_start(args, id);
		JNI_CALL_DECLARE(JT, result, clazz && id, true, static_cast<JT>((env->*CallStaticMethodOP)(clazz, id, args)));
		va_end(args);
		return result;
	}
};

template <typename JT, typename RT,
	RT(JNIEnv::* GetFieldOP)(jobject, jfieldID),
	void (JNIEnv::* SetFieldOP)(jobject, jfieldID, RT),
	RT(JNIEnv::* GetStaticFieldOP)(jclass, jfieldID),
	void (JNIEnv::* SetStaticFieldOP)(jclass, jfieldID, RT)
>
class FieldOps
{
public:
	static JT GetField(jobject object, jfieldID id)
	{
		JNI_CALL_RETURN(JT, object && id, true, static_cast<JT>((env->*GetFieldOP)(object, id)));
	}
	static void SetField(jobject object, jfieldID id, const RT& value)
	{
		JNI_CALL(object && id, true, (env->*SetFieldOP)(object, id, value));
	}
	static JT GetStaticField(jclass clazz, jfieldID id)
	{
		JNI_CALL_RETURN(JT, clazz && id, true, static_cast<JT>((env->*GetStaticFieldOP)(clazz, id)));
	}
	static void SetStaticField(jclass clazz, jfieldID id, const RT& value)
	{
		JNI_CALL(clazz && id, true, (env->*SetStaticFieldOP)(clazz, id, value));
	}
};

template <typename JT, typename RT,
	RT(JNITL_FUNCTION_ATTRIBUTES JNIEnv::* GetFieldOP)(jobject, jfieldID),
	void (JNITL_FUNCTION_ATTRIBUTES JNIEnv::* SetFieldOP)(jobject, jfieldID, RT),
	RT(JNITL_FUNCTION_ATTRIBUTES JNIEnv::* GetStaticFieldOP)(jclass, jfieldID),
	void (JNITL_FUNCTION_ATTRIBUTES JNIEnv::* SetStaticFieldOP)(jclass, jfieldID, RT)
>
class FloatFieldOps
{
public:
	static JT GetField(jobject object, jfieldID id)
	{
		JNI_CALL_RETURN(JT, object && id, true, static_cast<JT>((env->*GetFieldOP)(object, id)));
	}
	static void SetField(jobject object, jfieldID id, const RT& value)
	{
		JNI_CALL(object && id, true, (env->*SetFieldOP)(object, id, value));
	}
	static JT GetStaticField(jclass clazz, jfieldID id)
	{
		JNI_CALL_RETURN(JT, clazz && id, true, static_cast<JT>((env->*GetStaticFieldOP)(clazz, id)));
	}
	static void SetStaticField(jclass clazz, jfieldID id, const RT& value)
	{
		JNI_CALL(clazz && id, true, (env->*SetStaticFieldOP)(clazz, id, value));
	}
};

template <typename RT, typename RAT,
	RAT(JNIEnv::* NewArrayOP)(jsize),
	RT*  (JNIEnv::* GetArrayElementsOP)(RAT, jboolean*),
	void (JNIEnv::* ReleaseArrayElementsOP)(RAT, RT*, jint),
	void (JNIEnv::* GetArrayRegionOP)(RAT, jsize, jsize, RT*),
	void (JNIEnv::* SetArrayRegionOP)(RAT, jsize, jsize, const RT*)
>
class ArrayOps
{
public:
	static RAT NewArray(jsize size)
	{
		JNI_CALL_RETURN(RAT, true, true, static_cast<RAT>((env->*NewArrayOP)(size)));
	}
	static RT* GetArrayElements(RAT array, jboolean* isCopy = NULL)
	{
		JNI_CALL_RETURN(RT*, array, true, static_cast<RT*>((env->*GetArrayElementsOP)(array, isCopy)));
	}
	static void ReleaseArrayElements(RAT array, RT* elements, jint mode = 0)
	{
		JNI_CALL(array && elements, true, (env->*ReleaseArrayElementsOP)(array, elements, mode));
	}
	static void GetArrayRegion(RAT array, jsize start, jsize len, RT* buffer)
	{
		JNI_CALL(array && buffer, true, (env->*GetArrayRegionOP)(array, start, len, buffer));
	}
	static void SetArrayRegion(RAT array, jsize start, jsize len, RT* buffer)
	{
		JNI_CALL(array && buffer, true, (env->*SetArrayRegionOP)(array, start, len, buffer));
	}
};

template <typename JT, typename RT,
	RT(JNITL_FUNCTION_ATTRIBUTES JNIEnv::* CallMethodOP)(jobject, jmethodID, va_list),
	RT(JNITL_FUNCTION_ATTRIBUTES JNIEnv::* CallNonvirtualMethodOP)(jobject, jclass, jmethodID, va_list),
	RT(JNITL_FUNCTION_ATTRIBUTES JNIEnv::* CallStaticMethodOP)(jclass, jmethodID, va_list),
	RT(JNIEnv::* GetFieldOP)(jobject, jfieldID),
	void (JNIEnv::* SetFieldOP)(jobject, jfieldID, RT),
	RT(JNIEnv::* GetStaticFieldOP)(jclass, jfieldID),
	void (JNIEnv::* SetStaticFieldOP)(jclass, jfieldID, RT)
>
class Object_Op :
	public MethodOps<JT, RT, CallMethodOP, CallNonvirtualMethodOP, CallStaticMethodOP>,
	public FieldOps<JT, RT, GetFieldOP, SetFieldOP, GetStaticFieldOP, SetStaticFieldOP>
{ };

template <typename RT, typename RAT,
	RT(JNITL_FUNCTION_ATTRIBUTES JNIEnv::* CallMethodOP)(jobject, jmethodID, va_list),
	RT(JNITL_FUNCTION_ATTRIBUTES JNIEnv::* CallNonvirtualMethodOP)(jobject, jclass, jmethodID, va_list),
	RT(JNITL_FUNCTION_ATTRIBUTES JNIEnv::* CallStaticMethodOP)(jclass, jmethodID, va_list),
	RT(JNIEnv::* GetFieldOP)(jobject, jfieldID),
	void (JNIEnv::* SetFieldOP)(jobject, jfieldID, RT),
	RT(JNIEnv::* GetStaticFieldOP)(jclass, jfieldID),
	void (JNIEnv::* SetStaticFieldOP)(jclass, jfieldID, RT),
	RAT(JNIEnv::* NewArrayOP)(jsize),
	RT*  (JNIEnv::* GetArrayElementsOP)(RAT, jboolean*),
	void (JNIEnv::* ReleaseArrayElementsOP)(RAT, RT*, jint),
	void (JNIEnv::* GetArrayRegionOP)(RAT, jsize, jsize, RT*),
	void (JNIEnv::* SetArrayRegionOP)(RAT, jsize, jsize, const RT*)
>
class Primitive_Op :
	public MethodOps<RT, RT, CallMethodOP, CallNonvirtualMethodOP, CallStaticMethodOP>,
	public FieldOps<RT, RT, GetFieldOP, SetFieldOP, GetStaticFieldOP, SetStaticFieldOP>,
	public ArrayOps<RT, RAT, NewArrayOP, GetArrayElementsOP, ReleaseArrayElementsOP, GetArrayRegionOP, SetArrayRegionOP>
{ };

template <typename RT, typename RAT,
	RT(JNITL_FUNCTION_ATTRIBUTES JNIEnv::* CallMethodOP)(jobject, jmethodID, va_list),
	RT(JNITL_FUNCTION_ATTRIBUTES JNIEnv::* CallNonvirtualMethodOP)(jobject, jclass, jmethodID, va_list),
	RT(JNITL_FUNCTION_ATTRIBUTES JNIEnv::* CallStaticMethodOP)(jclass, jmethodID, va_list),
	RT(JNITL_FUNCTION_ATTRIBUTES JNIEnv::* GetFieldOP)(jobject, jfieldID),
	void (JNITL_FUNCTION_ATTRIBUTES JNIEnv::* SetFieldOP)(jobject, jfieldID, RT),
	RT(JNITL_FUNCTION_ATTRIBUTES JNIEnv::* GetStaticFieldOP)(jclass, jfieldID),
	void (JNITL_FUNCTION_ATTRIBUTES JNIEnv::* SetStaticFieldOP)(jclass, jfieldID, RT),
	RAT(JNIEnv::* NewArrayOP)(jsize),
	RT*  (JNIEnv::* GetArrayElementsOP)(RAT, jboolean*),
	void (JNIEnv::* ReleaseArrayElementsOP)(RAT, RT*, jint),
	void (JNIEnv::* GetArrayRegionOP)(RAT, jsize, jsize, RT*),
	void (JNIEnv::* SetArrayRegionOP)(RAT, jsize, jsize, const RT*)
>
class FloatPrimitive_Op :
	public MethodOps<RT, RT, CallMethodOP, CallNonvirtualMethodOP, CallStaticMethodOP>,
	public FloatFieldOps<RT, RT, GetFieldOP, SetFieldOP, GetStaticFieldOP, SetStaticFieldOP>,
	public ArrayOps<RT, RAT, NewArrayOP, GetArrayElementsOP, ReleaseArrayElementsOP, GetArrayRegionOP, SetArrayRegionOP>
{ };


#define JNITL_DEF_OP_LIST(t) \
	&JNIEnv::Call##t##MethodV, \
	&JNIEnv::CallNonvirtual##t##MethodV, \
	&JNIEnv::CallStatic##t##MethodV, \
	&JNIEnv::Get##t##Field, \
	&JNIEnv::Set##t##Field, \
	&JNIEnv::GetStatic##t##Field, \
	&JNIEnv::SetStatic##t##Field

#define JNITL_DEF_PRIMITIVE_OP_LIST(t) \
	JNITL_DEF_OP_LIST(t), \
	&JNIEnv::New##t##Array, \
	&JNIEnv::Get##t##ArrayElements, \
	&JNIEnv::Release##t##ArrayElements, \
	&JNIEnv::Get##t##ArrayRegion, \
	&JNIEnv::Set##t##ArrayRegion


// it defaults to jobject
template<typename T>
class Op : public Object_Op<T, jobject, JNITL_DEF_OP_LIST(Object)> {};

#define JNITL_DEF_PRIMITIVE_OP(jt,t) \
	template <> \
	class Op<jt> : public Primitive_Op<jt,jt##Array, \
		JNITL_DEF_PRIMITIVE_OP_LIST(t) \
	> {};

#define JNITL_DEF_FLOAT_PRIMITIVE_OP(jt,t) \
	template <> \
	class Op<jt> : public FloatPrimitive_Op<jt,jt##Array, \
		JNITL_DEF_PRIMITIVE_OP_LIST(t) \
	> {};

// specialization for primitives
JNITL_DEF_PRIMITIVE_OP(jboolean, Boolean)
JNITL_DEF_PRIMITIVE_OP(jint, Int)
JNITL_DEF_PRIMITIVE_OP(jshort, Short)
JNITL_DEF_PRIMITIVE_OP(jbyte, Byte)
JNITL_DEF_PRIMITIVE_OP(jlong, Long)
JNITL_DEF_PRIMITIVE_OP(jchar, Char)
JNITL_DEF_FLOAT_PRIMITIVE_OP(jfloat, Float)
JNITL_DEF_FLOAT_PRIMITIVE_OP(jdouble, Double)

#undef JNITL_FUNCTION_ATTRIBUTES
#undef JNITL_DEF_FLOAT_PRIMITIVE_OP
#undef JNITL_DEF_PRIMITIVE_OP
#undef JNITL_DEF_PRIMITIVE_OP_LIST
#undef JNITL_DEF_OP_LIST

// void requires a specialization.
template <>
class Op<jvoid>
{
public:
	static jvoid CallMethod(jobject object, jmethodID id, ...)
	{
		va_list args;
		va_start(args, id);
		JNI_CALL(object && id, true, env->CallVoidMethodV(object, id, args));
		va_end(args);
		return 0;
	}
	static jvoid CallStaticMethod(jclass clazz, jmethodID id, ...)
	{
		va_list args;
		va_start(args, id);
		JNI_CALL(clazz && id, true, env->CallStaticVoidMethodV(clazz, id, args));
		va_end(args);
		return 0;
	}
};

} // namespace android

NS_FK_END
#endif //#if PLATFORM_ANDROID

#endif // END OF FOUNDATIONKIT_ANDROIDJNIOPERATION_HPP