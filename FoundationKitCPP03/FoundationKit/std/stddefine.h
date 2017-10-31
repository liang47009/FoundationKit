/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_STDDEFINE_H
#define FOUNDATIONKIT_STDDEFINE_H

#define _XKEYCHECK_H // disable windows xkeycheck.h

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif

#ifndef _NOEXCEPT
#define _NOEXCEPT
#endif

#ifndef nullptr
#define nullptr NULL
#endif

#ifndef final
#define final
#endif


typedef unsigned short char16_t;
typedef unsigned int char32_t;

#include <string>
#include <cassert>
#include <functional>
namespace std
{
    typedef basic_string<char16_t, char_traits<char16_t>, allocator<char16_t> > u16string;



    // FUNCTION _Hash_seq
    inline size_t _Hash_seq(const unsigned char *_First, size_t _Count)
    {	// FNV-1a hash function for bytes in [_First, _First+_Count)
#if defined(_M_X64) || defined(_LP64) || defined(__x86_64) || defined(_WIN64)
        //static_assert(sizeof(size_t) == 8, "This code is for 64-bit size_t.");
        assert(sizeof(size_t) == 8);
        const size_t _FNV_offset_basis = 14695981039346656037ULL;
        const size_t _FNV_prime = 1099511628211ULL;

#else /* defined(_M_X64), etc. */
        //static_assert(sizeof(size_t) == 4, "This code is for 32-bit size_t.");
        assert(sizeof(size_t) == 4);
        const size_t _FNV_offset_basis = 2166136261U;
        const size_t _FNV_prime = 16777619U;
#endif /* defined(_M_X64), etc. */

        size_t _Val = _FNV_offset_basis;
        for (size_t _Next = 0; _Next < _Count; ++_Next)
        {	// fold in another byte
            _Val ^= (size_t)_First[_Next];
            _Val *= _FNV_prime;
        }

#if defined(_M_X64) || defined(_LP64) || defined(__x86_64) || defined(_WIN64)
        //static_assert(sizeof(size_t) == 8, "This code is for 64-bit size_t.");
        assert(sizeof(size_t) == 8);
        _Val ^= _Val >> 32;

#else /* defined(_M_X64), etc. */
        //static_assert(sizeof(size_t) == 4, "This code is for 32-bit size_t.");
        assert(sizeof(size_t) == 4);
#endif /* defined(_M_X64), etc. */

        return (_Val);
    }

    // TEMPLATE STRUCT _Bitwise_hash
    template<class _Kty>
    struct _Bitwise_hash
        : public unary_function<_Kty, size_t>
    {	// hash functor for plain old data
        size_t operator()(const _Kty& _Keyval) const
        {	// hash _Keyval to size_t value by pseudorandomizing transform
            return (_Hash_seq((const unsigned char *)&_Keyval, sizeof (_Kty)));
        }
    };

    // TEMPLATE STRUCT hash
    template<class _Kty>
    struct hash
        : public _Bitwise_hash<_Kty>
    {	// hash functor for enums
        //static const bool _Value = __is_enum(_Kty);
        //static_assert(_Value, "The C++ Standard doesn't provide a hash for this type.");
    };
    template<>
    struct hash<bool>
        : public _Bitwise_hash<bool>
    {	// hash functor for bool
    };

    template<>
    struct hash<char>
        : public _Bitwise_hash<char>
    {	// hash functor for char
    };

    template<>
    struct hash<signed char>
        : public _Bitwise_hash<signed char>
    {	// hash functor for signed char
    };

    template<>
    struct hash<unsigned char>
        : public _Bitwise_hash<unsigned char>
    {	// hash functor for unsigned char
    };

    template<>
    struct hash<wchar_t>
        : public _Bitwise_hash<wchar_t>
    {	// hash functor for wchar_t
    };

    template<>
    struct hash<short>
        : public _Bitwise_hash<short>
    {	// hash functor for short
    };

    template<>
    struct hash<unsigned short>
        : public _Bitwise_hash<unsigned short>
    {	// hash functor for unsigned short
    };

    template<>
    struct hash<int>
        : public _Bitwise_hash<int>
    {	// hash functor for int
    };

    template<>
    struct hash<unsigned int>
        : public _Bitwise_hash<unsigned int>
    {	// hash functor for unsigned int
    };

    template<>
    struct hash<long>
        : public _Bitwise_hash<long>
    {	// hash functor for long
    };

    template<>
    struct hash<unsigned long>
        : public _Bitwise_hash<unsigned long>
    {	// hash functor for unsigned long
    };

    template<>
    struct hash<long long>
        : public _Bitwise_hash<long long>
    {	// hash functor for long long
    };

    template<>
    struct hash<unsigned long long>
        : public _Bitwise_hash<unsigned long long>
    {	// hash functor for unsigned long long
    };

    template<>
    struct hash<float>
        : public _Bitwise_hash<float>
    {	// hash functor for float
        typedef float _Kty;
        typedef _Bitwise_hash<_Kty> _Mybase;

        size_t operator()(const _Kty& _Keyval) const
        {	// hash _Keyval to size_t value by pseudorandomizing transform
            return (_Mybase::operator()(
                _Keyval == 0 ? 0 : _Keyval)); // map -0 to 0
        }
    };

    template<>
    struct hash<double>
        : public _Bitwise_hash<double>
    {	// hash functor for double
        typedef double _Kty;
        typedef _Bitwise_hash<_Kty> _Mybase;

        size_t operator()(const _Kty& _Keyval) const
        {	// hash _Keyval to size_t value by pseudorandomizing transform
            return (_Mybase::operator()(
                _Keyval == 0 ? 0 : _Keyval)); // map -0 to 0
        }
    };

    template<>
    struct hash<long double>
        : public _Bitwise_hash<long double>
    {	// hash functor for long double
        typedef long double _Kty;
        typedef _Bitwise_hash<_Kty> _Mybase;

        size_t operator()(const _Kty& _Keyval) const
        {	// hash _Keyval to size_t value by pseudorandomizing transform
            return (_Mybase::operator()(
                _Keyval == 0 ? 0 : _Keyval)); // map -0 to 0
        }
    };

    template<class _Ty>
    struct hash<_Ty *>
        : public _Bitwise_hash<_Ty *>
    {	// hash functor for _Ty *
    };
}

#endif // FOUNDATIONKIT_STDDEFINE_H