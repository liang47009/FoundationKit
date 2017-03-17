/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_TUPLEAPPLY_HPP
#define FOUNDATIONKIT_TUPLEAPPLY_HPP

/**
 Original file from gtk: http://cxx-gtk-utils.sourceforge.net/
 #include <c++-gtk-utils/param.h>
*/
#pragma once
#include <tuple>
#include <cstddef>     // for std::size_t
#include <utility>     // for std::forward and std::move
#include <type_traits> // for std::result_of

// the remainder of this file implements tuple_apply()

namespace TupleHelper {

/*
 MakeIndexList provides a list of tuple indices for any given
 instantiation, comprised in the type of an IndexList object.  For
 example, MakeIndexList<4>::Type is of type IndexList<0, 1, 2, 3>.
 The index values held in the type of the IndexList object are then
 unpacked into std::get().

 IndexList is the base class for MakeIndexList, from which the
 complete MakeIndexList instantiation ending recursion derives in
 order to deliver up the index list for tuple_apply_impl - its Type
 member is itself.  So the inheritance diagram for MakeIndexList<4>
 is:

   IndexList<0, 1, 2, 3>
            |
            |
 MakeIndexList<0, 0, 1, 2, 3>
            |
            |
  MakeIndexList<1, 1, 2, 3>
            |
            |
   MakeIndexList<2, 2, 3>
            |
            |
    MakeIndexList<3, 3>
            |
            |
     MakeIndexList<4>
*/

template<std::size_t... indices>
struct IndexList {
  typedef IndexList<indices...> Type;
};

template<std::size_t count, std::size_t... indices>
struct MakeIndexList: public MakeIndexList<count - 1, count - 1, indices...> {};

// ends recursion - the complete (one from base) class, inheriting
// directly from IndexList
template<std::size_t... indices>
struct MakeIndexList<0, indices...>: public IndexList<indices...> {};

template <class Obj, class Ret, class... FuncArgs, class Tuple,
	  std::size_t... indices, class... OtherArgs>
Ret tuple_apply_impl(Obj& obj,
		     Ret (Obj::* func) (FuncArgs...),
		     Tuple&& t,
		     IndexList<indices...>,
		     OtherArgs&&... args) {
  return (obj.*func)(std::get<indices>(std::forward<Tuple>(t))..., std::forward<OtherArgs>(args)...);
}

template <class Obj, class Ret, class... FuncArgs, class Tuple,
	  std::size_t... indices, class... OtherArgs>
Ret tuple_apply_impl(const Obj& obj,
		     Ret (Obj::* func) (FuncArgs...) const,
		     Tuple&& t,
		     IndexList<indices...>,
		     OtherArgs&&... args) {
  return (obj.*func)(std::get<indices>(std::forward<Tuple>(t))..., std::forward<OtherArgs>(args)...);
}

template <class Ret, class Func, class Tuple,
	  std::size_t... indices, class... OtherArgs>
Ret tuple_apply_impl(Func&& func,
		     Tuple&& t,
		     IndexList<indices...>,
		     OtherArgs&&... args) {
  return func(std::get<indices>(std::forward<Tuple>(t))..., std::forward<OtherArgs>(args)...);
}

} // namespace TupleHelper


/**
 * @defgroup tuple tuple
 *
 * \#include <c++-gtk-utils/param.h>
 *
 * From version 2.2.10, where this library is compiled with gcc >= 4.8
 * or clang >= 3.4, the library uses a backend for relevant
 * closure/callback classes using std::tuple (unless that backend is
 * explicitly disabled with the \--without-tuple configuration option,
 * in which case the implementation used prior to version 2.2.10 is
 * retained).  The tuple backend avoids code duplication and makes for
 * a cleaner implementation, and enables Callback::make() and
 * Callback::make_ref() to take up to 10 bound arguments instead of
 * the previous 5 bound arguments.  For compilers other than gcc and
 * clang which adequately support std::tuple, the tuple backend can be
 * manually selected with the \--with-tuple configuration option.
 * Both backends are ABI compatible, because for closure/callback
 * implementation this library only exports the CallbackArg pure
 * virtual class.
 *
 * Where this library has been compiled with the tuple backend for
 * callback classes, it uses the Cgu::tuple_apply() utility function
 * to expand tuples into their component elements in order to pass
 * those elements to functions as arguments on function calls.
 * Because this utility function is of general use, it is made
 * available by the library more generally in the param.h header.
 *
 * Taking a function with the signature 'int my_func(int, double, int,
 * double)', a tuple could be expanded to apply the my_func() function
 * to it as follows:
 *
 * @code
 *   int my_func(int, double, int, double);
 *   auto tup = make_tuple(2, 3.0, 4, 5.0);
 *   int res = Cgu::tuple_apply(my_func, tup);
 * @endcode
 *
 * Trailing arguments not comprising part of the tuple can also be
 * passed to the function, so in the above example my_func() could be
 * called as follows:
 *
 * @code
 *   int my_func(int, double, int, double);
 *   auto tup = make_tuple(2, 3.0);
 *   int res = Cgu::tuple_apply(my_func, tup, 4, 5.0);
 * @endcode
 *
 * Overloads of the tuple_apply() function are provided which can
 * apply any callable object (including a normal function, a static
 * member function, the return value of std::bind() or a lambda
 * expression) to a tuple, or which can apply a const or non-const
 * non-static member function with object to the tuple.
 */

// We need three overloads of the version of this function which takes
// a callable object (two for tuple lvalues and one for tuple rvalues)
// because we cannot have the tuple as a completely deduced type in
// this context - we need to deduce TupleArgs for std::result_of.
// These overloads will work with any callable object, including a
// function pointer.
/**
 * \#include <c++-gtk-utils/param.h>
 *
 * This function expands a tuple and uses the tuple's elements as
 * arguments to a call to a callable object, such as a normal
 * function, a static member function, the return value of std::bind()
 * or a lambda expression.
 *
 * @param func The callable object.
 * @param t The std::tuple object to be expanded.
 * @param args The trailing arguments (if any) to be used in the
 * function call.
 * @return The result of applying func to the tuple and any trailing
 * arguments.
 * @note This function is not available unless either (i) this library
 * is compiled with gcc >= 4.8 or clang >= 3.4, or (ii) the library is
 * compiled by another compiler with appropriate support for C++11
 * tuples using the \--with-tuple configuration option.
 *
 * Since 2.2.10
 * @ingroup tuple
 */
template<class Func,
	 class... TupleArgs,
         class... OtherArgs>
auto tuple_apply(Func&& func,
		 const std::tuple<TupleArgs...>& t,
		 OtherArgs&&... args) -> typename std::result_of<Func(const TupleArgs&..., OtherArgs&&...)>::type {
  typedef typename std::result_of<Func(const TupleArgs&..., OtherArgs&&...)>::type Ret;
  typedef typename TupleHelper::MakeIndexList<sizeof...(TupleArgs)>::Type List;
  return TupleHelper::tuple_apply_impl<Ret>(std::forward<Func>(func),
					    t,
					    List(),
					    std::forward<OtherArgs>(args)...);
}

/**
 * \#include <c++-gtk-utils/param.h>
 *
 * This function expands a tuple and uses the tuple's elements as
 * arguments to a call to a callable object, such as a normal
 * function, a static member function, the return value of std::bind()
 * or a lambda expression.
 *
 * @param func The callable object.
 * @param t The std::tuple object to be expanded.
 * @param args The trailing arguments (if any) to be used in the
 * function call.
 * @return The result of applying func to the tuple and any trailing
 * arguments.
 * @note This function is not available unless either (i) this library
 * is compiled with gcc >= 4.8 or clang >= 3.4, or (ii) the library is
 * compiled by another compiler with appropriate support for C++11
 * tuples using the \--with-tuple configuration option.
 *
 * Since 2.2.10
 * @ingroup tuple
 */
template<class Func,
	 class... TupleArgs,
         class... OtherArgs>
auto tuple_apply(Func&& func,
		 std::tuple<TupleArgs...>& t,
		 OtherArgs&&... args) -> typename std::result_of<Func(TupleArgs&..., OtherArgs&&...)>::type {
  typedef typename std::result_of<Func(TupleArgs&..., OtherArgs&&...)>::type Ret;
  typedef typename TupleHelper::MakeIndexList<sizeof...(TupleArgs)>::Type List;
  return TupleHelper::tuple_apply_impl<Ret>(std::forward<Func>(func),
					    t,
					    List(),
					    std::forward<OtherArgs>(args)...);
}

/**
 * \#include <c++-gtk-utils/param.h>
 *
 * This function expands a tuple and uses the tuple's elements as
 * arguments to a call to a callable object, such as a normal
 * function, a static member function, the return value of std::bind()
 * or a lambda expression.
 *
 * @param func The callable object.
 * @param t The std::tuple object to be expanded.
 * @param args The trailing arguments (if any) to be used in the
 * function call.
 * @return The result of applying func to the tuple and any trailing
 * arguments.
 * @note This function is not available unless either (i) this library
 * is compiled with gcc >= 4.8 or clang >= 3.4, or (ii) the library is
 * compiled by another compiler with appropriate support for C++11
 * tuples using the \--with-tuple configuration option.
 *
 * Since 2.2.10
 * @ingroup tuple
 */
template<class Func,
	 class... TupleArgs,
         class... OtherArgs>
auto tuple_apply(Func&& func,
		 std::tuple<TupleArgs...>&& t,
		 OtherArgs&&... args) -> typename std::result_of<Func(TupleArgs&&..., OtherArgs&&...)>::type {
  typedef typename std::result_of<Func(TupleArgs&&..., OtherArgs&&...)>::type Ret;
  typedef typename TupleHelper::MakeIndexList<sizeof...(TupleArgs)>::Type List;
  return TupleHelper::tuple_apply_impl<Ret>(std::forward<Func>(func),
					    std::move(t),
					    List(),
					    std::forward<OtherArgs>(args)...);
}

/**
 * \#include <c++-gtk-utils/param.h>
 *
 * This function expands a tuple and uses the tuple's elements as
 * arguments to a function call to a non-static class member function.
 *
 * @param obj The object whose member function is to be called.
 * @param func The non-static member function to be called.
 * @param t The std::tuple object to be expanded.
 * @param args The trailing arguments (if any) to be used in the
 * function call.
 * @return The result of applying func to the tuple and any trailing
 * arguments.
 * @note This function is not available unless either (i) this library
 * is compiled with gcc >= 4.8 or clang >= 3.4, or (ii) the library is
 * compiled by another compiler with appropriate support for C++11
 * tuples using the \--with-tuple configuration option.
 *
 * Since 2.2.10
 * @ingroup tuple
 */
template<class Obj, class Ret, class... FuncArgs, class Tuple, class... OtherArgs>
Ret tuple_apply(Obj& obj,
		Ret (Obj::* func) (FuncArgs...),
		Tuple&& t,
		OtherArgs&&... args) {
  typedef typename std::remove_reference<Tuple>::type TupleType;
  static_assert(sizeof...(FuncArgs) == std::tuple_size<TupleType>::value + sizeof...(OtherArgs),
  		"The arity of the function passed to tuple_apply() and the "
  		"arguments passed to it do not match");
  typedef typename TupleHelper::MakeIndexList<std::tuple_size<TupleType>::value>::Type List;
  return TupleHelper::tuple_apply_impl(obj,
				       func,
				       std::forward<Tuple>(t),
				       List(),
				       std::forward<OtherArgs>(args)...);
}

/**
 *
 * This function expands a tuple and uses the tuple's elements as
 * arguments to a function call to a non-static class member function.
 *
 * @param obj The object whose member function is to be called.
 * @param func The non-static member function to be called.
 * @param t The std::tuple object to be expanded.
 * @param args The trailing arguments (if any) to be used in the
 * function call.
 * @return The result of applying func to the tuple and any trailing
 * arguments.
 * @note This function is not available unless either (i) this library
 * is compiled with gcc >= 4.8 or clang >= 3.4, or (ii) the library is
 * compiled by another compiler with appropriate support for C++11
 * tuples using the \--with-tuple configuration option.
 *
 * Since 2.2.10
 * @ingroup tuple
 */
template<class Obj, class Ret, class... FuncArgs, class Tuple, class... OtherArgs>
Ret tuple_apply(const Obj& obj,
		Ret (Obj::* func) (FuncArgs...) const,
		Tuple&& t,
		OtherArgs&&... args) {
  typedef typename std::remove_reference<Tuple>::type TupleType;
  static_assert(sizeof...(FuncArgs) == std::tuple_size<TupleType>::value + sizeof...(OtherArgs),
  		"The arity of the function passed to tuple_apply() and the "
  		"arguments passed to it do not match");
  typedef typename TupleHelper::MakeIndexList<std::tuple_size<TupleType>::value>::Type List;
  return TupleHelper::tuple_apply_impl(obj,
				       func,
				       std::forward<Tuple>(t),
				       List(),
				       std::forward<OtherArgs>(args)...);
}

#endif // FOUNDATIONKIT_TUPLEAPPLY_HPP
