

#include "FoundationKit/FoundationMacros.h"


typedef struct CRYPTO_THREADID  THREAD_ID;

NS_FK_BEGIN

namespace network{

class SSLLock
{
    static void ssl_threadid_function_callback(THREAD_ID* id);
    static void ssl_lock_callback(int, int, const char *, int);
public:
    SSLLock();
    ~SSLLock();
};

class SSLPthreadLock
{
    static void ssl_threadid_function_callback(THREAD_ID* id);
    static void ssl_lock_callback(int, int, const char *, int);

public:
    SSLPthreadLock();
    ~SSLPthreadLock();
};


} // namespace network{

NS_FK_END

