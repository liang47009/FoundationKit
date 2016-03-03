

#ifndef FOUNDATIONKIT_STD_WRAPPER_H
#define FOUNDATIONKIT_STD_WRAPPER_H

#include <string>

#ifndef _STD_BEGIN

#if defined(__cplusplus)
#define _STD_BEGIN	namespace std {
#define _STD_END		}
#define _STD	::std::
#else /* __cplusplus */
#define _STD_BEGIN
#define _STD_END
#define _STD
#endif
#endif

_STD_BEGIN

#if (__cplusplus < 201103L) && defined(_MSC_VER) && _MSC_VER < 1600
    typedef unsigned short char16_t;
    typedef unsigned int char32_t;
    typedef basic_string<char16_t, char_traits<char16_t>, allocator<char16_t> > u16string;
    typedef basic_string<char32_t, char_traits<char32_t>, allocator<char32_t> > u32string;
#endif //(__cplusplus < 201103L)
    typedef basic_string < unsigned char, char_traits<unsigned char>, allocator< unsigned char> > ustring;

_STD_END

#endif // #ifndef FOUNDATIONKIT_STD_WRAPPER_H