/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_IDENTITY_TYPE_HPP
#define FOUNDATIONKIT_IDENTITY_TYPE_HPP

#include "FoundationKit/Base/function_traits.hpp"
/**
@brief This macro allows to wrap the specified type expression within extra
round parenthesis so the type can be passed as a single macro parameter even if
it contains commas (not already wrapped within round parenthesis).

@Params
@Param{parenthesized_type,
The type expression to be passed as macro parameter wrapped by a single set
of round parenthesis <c>(...)</c>.
This type expression can contain an arbitrary number of commas.
}
@EndParams

This macro works on any C++03 compiler (it does not use variadic macros).

This macro must be prefixed by <c>typename</c> when used within templates.
Note that the compiler will not be able to automatically determine function
template parameters when they are wrapped with this macro (these parameters
need to be explicitly specified when calling the function template).

On some compilers (like GCC), using this macro on abstract types requires to
add and remove a reference to the specified type.
*/
/* must NOT prefix this with `::` to work with parenthesized syntax */ 
#define IDENTITY_TYPE(parenthesized_type) FoundationKit::function_traits< void parenthesized_type >::argument<0>::type;

#endif // END OF FOUNDATIONKIT_IDENTITY_TYPE_HPP