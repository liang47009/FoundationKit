

// Default to a header-only implementation. The user must specifically request
// separate compilation by defining either ASIO_SEPARATE_COMPILATION or
// ASIO_DYN_LINK (as a DLL/shared library implies separate compilation).

#if !defined(NETWORK_HEADER_ONLY)
#  if !defined(NETWORK_DYNAMIC_LINK)
#   define NETWORK_HEADER_ONLY 1
#  endif // !defined(ASIO_DYN_LINK)
#endif // !defined(ASIO_HEADER_ONLY)


#if defined(NETWORK_HEADER_ONLY)
# define NETWORK_API inline
#else // defined(ASIO_HEADER_ONLY)
# if defined(_MSC_VER) || defined(__BORLANDC__) || defined(__CODEGEARC__)
// We need to import/export our code only if the user has specifically asked
// for it by defining ASIO_DYN_LINK.
#  if defined(NETWORK_DYNAMIC_LINK)
// Export if this is our own source, otherwise import.
#   if defined(NETWORK_SOURCE)
#    define NETWORK_API DLL_EXPORT
#   else // defined(NETWORK_SOURCE)  
#    define NETWORK_API DLL_IMPORT
#   endif // defined(NETWORK_SOURCE)
#  endif // defined(NETWORK_DYNAMIC_LINK)
# endif // defined(_MSC_VER) || defined(__BORLANDC__) || defined(__CODEGEARC__)
#endif // defined(NETWORK_HEADER_ONLY)
