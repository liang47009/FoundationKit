

#ifndef FOUNDATIONKIT_STD_WRAPPER_H
#define FOUNDATIONKIT_STD_WRAPPER_H
#include <string>
#include "FoundationKit/FoundationKitMacros.h"

_STD_BEGIN

#if(CPP_TARGET_VERSION < CPP_VERSION_11 )
    typedef unsigned short char16_t;
    typedef unsigned int char32_t;
    typedef basic_string<char16_t, char_traits<char16_t>, allocator<char16_t> > u16string;
    typedef basic_string<char32_t, char_traits<char32_t>, allocator<char32_t> > u32string;
#endif //(__cplusplus < 201103L)
    typedef basic_string < unsigned char, char_traits<unsigned char>, allocator< unsigned char> > ustring;

_STD_END

#endif // #ifndef FOUNDATIONKIT_STD_WRAPPER_H