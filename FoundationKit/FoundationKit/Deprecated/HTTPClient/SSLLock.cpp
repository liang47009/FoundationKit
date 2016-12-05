#include <vector>
#include <mutex>
#include <thread>
#include <sstream>
#include "SSLLock.h"

#ifndef CRYPTO_LOCK
# define CRYPTO_LOCK 1
#endif // !CRYPTO_LOCK

extern "C"
{
    extern int CRYPTO_num_locks(void);
    extern void CRYPTO_set_locking_callback(void(*func) (int mode, int type, const char *file, int line));
    extern int CRYPTO_THREADID_set_callback(void(*func) (THREAD_ID *));
    extern void CRYPTO_THREADID_set_numeric(THREAD_ID *id, unsigned long val);
}

NS_FK_BEGIN
namespace network{

static std::vector<std::mutex> _sslMutex(CRYPTO_num_locks());
SSLLock::SSLLock()
{
    CRYPTO_THREADID_set_callback(ssl_threadid_function_callback);
    CRYPTO_set_locking_callback(ssl_lock_callback);
}

SSLLock::~SSLLock()
{
    CRYPTO_THREADID_set_callback(nullptr);
    CRYPTO_set_locking_callback(nullptr);
}


void SSLLock::ssl_threadid_function_callback(THREAD_ID* id)
{
    CRYPTO_THREADID_set_numeric(id, (unsigned long)pthread_self());

}

void SSLLock::ssl_lock_callback(int mode, int index, const char *, int)
{
    if (mode & CRYPTO_LOCK)  
        _sslMutex[index].lock();
    else                        
        _sslMutex[index].unlock();
}

//==========================================================

#include <pthread.h>

static pthread_mutex_t *lockarray;

SSLPthreadLock::SSLPthreadLock()
{
    lockarray = (pthread_mutex_t *)OPENSSL_malloc(CRYPTO_num_locks() *sizeof(pthread_mutex_t));
    for (int i = 0; i < CRYPTO_num_locks(); i++)
    {
        pthread_mutex_init(&(lockarray[i]), NULL);
    }
    CRYPTO_THREADID_set_callback(ssl_threadid_function_callback);
    CRYPTO_set_locking_callback(ssl_lock_callback);
}

SSLPthreadLock::~SSLPthreadLock()
{
    CRYPTO_THREADID_set_callback(nullptr);
    CRYPTO_set_locking_callback(nullptr);

    for (int i = 0; i < CRYPTO_num_locks(); i++)
    {
        pthread_mutex_destroy(&(lockarray[i]));
    }
    OPENSSL_free(lockarray);
}


void SSLPthreadLock::ssl_threadid_function_callback(THREAD_ID* id)
{
    CRYPTO_THREADID_set_numeric(id, (unsigned long)pthread_self());
}

void SSLPthreadLock::ssl_lock_callback(int mode, int index, const char *, int)
{
    if (mode & CRYPTO_LOCK) 
    {
        pthread_mutex_lock(&(lockarray[type]));
    }
    else 
    {
        pthread_mutex_unlock(&(lockarray[type]));
    }
}





} // namespace network

NS_FK_END




