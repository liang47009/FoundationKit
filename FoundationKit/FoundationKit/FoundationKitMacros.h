
// 这个文件暂时不被使用

#define UNUSED_PARAM(param) do{(void)(param);}while(0)

#ifdef __GNUC__
#define FK_UNUSED __attribute__ ((unused))
#else
#define FK_UNUSED
#endif

//#pragma message(__COMPILE_MSG__ "Show compile message")
#define __COMPILE_STR1__(s) #s
#define __COMPILE_STR2__(s) __COMPILE_STR1__(s)
#define __COMPILE_MSG__ __FILE__ "("__COMPILE_STR2__(__LINE__) "):Warning:" 