#pragma once

#include <memory>
#include <mutex>
#include "AndroidJNIHelper.h"
#include "FoundationKit/FoundationKitDefines.h"

NS_FK_BEGIN



namespace Android
{

    class RefCountedBase
    {
        virtual void _destroy() = 0;
        virtual void  _delete_this() = 0;
    public:
        RefCountedBase()
            :_count(1)
        {
        }
        inline void retain() const
        {
            std::lock_guard<std::mutex> lock(_refCountedMutex);
            ++_count;
        }
        void release()
        {
            std::lock_guard<std::mutex> lock(_refCountedMutex);
            if (--_count == 0)
            {
                _destroy();
                _delete_this();
            }
        }
        unsigned int getReferenceCount() const
        {
            std::lock_guard<std::mutex> lock(_refCountedMutex);
            return _count;
        }

    private:
        mutable size_t _count;
        static std::mutex _refCountedMutex;
    };




    template<typename _Dx>
    class RefCounted : public RefCountedBase
    {
    public:

        RefCounted(_Dx _Dt)
            : RefCountedBase()
            , _dtor(_Dt)
        {
        }
    private:
        void _destroy()
        {
            _dtor();
        }

        void  _delete_this()
        {
            delete this;
        }

        _Dx _dtor;

    };

} //namespace Android


NS_FK_END

