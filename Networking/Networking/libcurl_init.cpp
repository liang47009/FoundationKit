//#ifndef LOSEMYMIND_LIBCURL_INIT_IPP
//#define LOSEMYMIND_LIBCURL_INIT_IPP
//# pragma once
#include <string>
#include <cassert>
#include "libcurl_init.hpp"

namespace
{
    /**
     * A callback that libcurl will use to allocate memory
     *
     * @param Size size of allocation in bytes
     * @return Pointer to memory chunk or NULL if failed
     */
    void* custom_curl_malloc(size_t size)
    {
        return ::malloc(size);
    }

    /**
     * A callback that libcurl will use to free memory
     *
     * @param Ptr pointer to memory chunk (may be NULL)
     */
    void custom_curl_free(void* ptr)
    {
        ::free(ptr);
    }

    /**
     * A callback that libcurl will use to reallocate memory
     *
     * @param Ptr pointer to existing memory chunk (may be NULL)
     * @param Size size of allocation in bytes
     * @return Pointer to memory chunk or NULL if failed
     */
    void* custom_curl_realloc(void* ptr, size_t size)
    {
        return ::realloc(ptr, size);
    }

    /**
     * A callback that libcurl will use to duplicate a string
     *
     * @param ZeroTerminatedString pointer to string (ANSI or UTF-8, but this does not matter in this case)
     * @return Pointer to a copy of string
     */
    char* custom_curl_strdup(const char * zeroTerminatedString)
    {
        char * pCopy = NULL;
        if (zeroTerminatedString)
        {
            size_t strLen = strlen(zeroTerminatedString);
            pCopy = reinterpret_cast<char*>(::malloc(strLen + 1));
            if (pCopy)
            {
                ::strcpy(pCopy, zeroTerminatedString);
                assert(strcmp(pCopy, zeroTerminatedString) == 0);
            }
        }
        return pCopy;
    }

    /**
     * A callback that libcurl will use to allocate zero-initialized memory
     *
     * @param NumElems number of elements to allocate (may be 0, then NULL should be returned)
     * @param ElemSize size of each element in bytes (may be 0)
     * @return Pointer to memory chunk, filled with zeroes or NULL if failed
     */
    void* custom_curl_calloc(size_t numElems, size_t elemSize)
    {
        void* ret = nullptr;
        const size_t size = numElems * elemSize;
        if (size)
        {
            ret = ::malloc(size);

            if (ret)
            {
                memset(ret, 0, size);
            }
        }
        return ret;
    }
}

NS_FK_BEGIN
libcurl_init_base::Result libcurl_init::_result;
void libcurl_init_base::startup(Result& result)
{
    result._initCode = curl_global_init_mem(CURL_GLOBAL_ALL, custom_curl_malloc, custom_curl_free, custom_curl_realloc, custom_curl_strdup, custom_curl_calloc);
}

void libcurl_init_base::cleanup()
{
    curl_global_cleanup();
}

NS_FK_END


//#endif // LOSEMYMIND_LIBCURL_INIT_IPP
