

#include "FoundationKit/GenericPlatformMacros.h"

#if TARGET_PLATFORM == PLATFORM_ANDROID

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <memory>
#include <vector>
#include "FoundationKit/Networking/socket_types.hpp"
#include "FoundationKit/Base/error_code.hpp"

NS_FK_BEGIN

class epoll_reactor
{
public:
    typedef std::shared_ptr<epoll_reactor> pointer;
    typedef std::vector<struct epoll_event> EventList;
    // The hint to pass to epoll_create to size its data structures.
    enum { epoll_size = 20000 };

    epoll_reactor()
        : _epollfd(do_epoll_create())
    {
    }

    ~epoll_reactor()
    {
        if (_epollfd != -1)
            close(_epollfd);
    }

    void poll()
    {
        int numEvents = epoll_wait(_epollfd
            , &*_eventList.begin()
            , static_cast<int>(_eventList.size())
            , -1);


    }

 
private:

    int do_epoll_create()
    {
#if defined(EPOLL_CLOEXEC)
        int fd = epoll_create1(EPOLL_CLOEXEC);
#else // defined(EPOLL_CLOEXEC)
        int fd = -1;
        errno = EINVAL;
#endif // defined(EPOLL_CLOEXEC)

        if (fd == -1 && (errno == EINVAL || errno == ENOSYS))
        {
            fd = epoll_create(epoll_size);
            if (fd != -1)
                ::fcntl(fd, F_SETFD, FD_CLOEXEC);
        }

        if (fd == -1)
        {
            std::error_code ec(errno,std::system_category());
            throw_error_if(ec, "epoll");
        }

        return fd;
    }

private:
    int                 _epollfd;
    EventList           _eventList;
};   

NS_FK_END


#endif



