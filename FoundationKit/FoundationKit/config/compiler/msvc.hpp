


#   if _MSC_VER < 1310
// Note: Versions up to 7.0 aren't supported.
#     define CURRENT_COMPILER_VERSION 5.0
#   elif _MSC_VER < 1300
#     define CURRENT_COMPILER_VERSION 6.0
#   elif _MSC_VER < 1310
#     define CURRENT_COMPILER_VERSION 7.0
#   elif _MSC_VER < 1400
#     define CURRENT_COMPILER_VERSION 7.1
#   elif _MSC_VER < 1500
#     define CURRENT_COMPILER_VERSION 8.0
#   elif _MSC_VER < 1600
#     define CURRENT_COMPILER_VERSION 9.0
#   elif _MSC_VER < 1700
#     define CURRENT_COMPILER_VERSION 10.0
#   elif _MSC_VER < 1800 
#     define CURRENT_COMPILER_VERSION 11.0
#   elif _MSC_VER < 1900
#     define CURRENT_COMPILER_VERSION 12.0
#   elif _MSC_VER < 2000
#     define CURRENT_COMPILER_VERSION 14.0
#   else
#     define CURRENT_COMPILER_VERSION _MSC_VER
#   endif

#  define CURRENT_COMPILER "Microsoft Visual C++ version " DO_STRINGIZE(CURRENT_COMPILER_VERSION)