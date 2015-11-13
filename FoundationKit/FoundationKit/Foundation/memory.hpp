
#ifndef FoundationKit_memory_H
#define FoundationKit_memory_H
#if defined(_MSC_VER)
# pragma once
#endif

void * aligned_malloc (int size, int alignment) 
{
    void* ptr = malloc(size + alignment);
    if (ptr)
    {
        void* aligned = (void*)(((long)ptr + alignment) & ~(alignment - 1));
        ((void**)aligned)[-1] = ptr;
        return aligned;
    }
    else
        return NULL;
}


void *aligned_free(void *paligned)
{
    delete [ ]paligned;
}



/**
 * @code
 *   struct MyStruct
 *   {
 *      int a;
 *      double b;
 *   };
 *   auto ptr = AlignedStorage<MyStruct>::create();
 *   auto ptr0 = AlignedStorage<MyStruct>::get(ptr);
 *   AlignedStorage<MyStruct>::destroy(ptr);
 * @endcode
 */
template<typename T>
struct AlignedStorage
{
    using type = typename std::aligned_storage<sizeof(T), std::alignment_of<T>::value>::type;
    template<typename... Args>
    static type create(Args&&... args)
    {
        type  _ptr;
        new(&_ptr)T(std::forward<Args>(args)...);
        return _ptr;
    }
    static void destroy(type& ptr)
    {
        reinterpret_cast<T*>(&ptr)->~T();
    }
    
    static T* get(type& ptr)
    {
        return reinterpret_cast<T*>(&ptr);
    }
};


#endif // #ifndef FoundationKit_memory_H



