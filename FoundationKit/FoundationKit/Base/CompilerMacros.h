

//#if defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
#ifdef __GNUC__
#define __deprecated     __attribute__((deprecated))
#define __unused         __attribute__((unused))
#define __align(n)       __attribute__((aligned(n)))
#define __align_max      __attribute__((aligned))
#define __packed         __attribute__((packed))

#elif _MSC_VER
#define __deprecated __declspec(deprecated)
#define __unused
#define __align(n)   __declspec(align(n))
#define __align_max
#define __packed

#else
#define __deprecated
#define __unused
#define __align(n)
#define __align_max
#define __packed
#endif


#define __unused_arg(arg) do{(void)(arg);}while(0)