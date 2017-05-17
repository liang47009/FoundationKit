#ifndef LOSEMYMIND_LANGUAGEFEATURES_H
#define LOSEMYMIND_LANGUAGEFEATURES_H

/// Macro for determining if GCC version is >= than X.Y
#if defined(__GNUC__)
#define CHECK_GCC_VERSION(MAJOR, MINOR) \
        (__GNUC__ > MAJOR || (__GNUC__ == MAJOR && __GNUC_MINOR__ >= MINOR))
#else
#define CHECK_GCC_VERSION(MAJOR, MINOR) 0
#endif

/// Macro for determining if there are sufficient C++0x/C++11 features
#ifdef __INTEL_COMPILER
#ifdef __INTEL_CXX11_MODE__
#define STDLIB_HAS_CXX11 1
#endif
#elif defined(__clang__)
#ifdef _LIBCPP_VERSION
#define STDLIB_HAS_CXX11 1
#endif
#elif defined(__GXX_EXPERIMENTAL_CXX0X__) || (__cplusplus > 199711L)
#define HAS_CXX11 1
#elif defined(_MSC_VER)
#if (_MSC_VER >= 1700)
#define STDLIB_HAS_CXX11 1
#endif
#endif

#if defined(__GNUC__) && !CHECK_GCC_VERSION(4, 4)
// ICC uses GCC's standard library headers, so even if the ICC version
// is recent enough for C++11, the GCC version might not be.
#undef STDLIB_HAS_CXX11
#endif

#if __cplusplus < 201103L
    #define noexcept
    #define constexpr
#endif


#endif // LOSEMYMIND_LANGUAGEFEATURES_H
