/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_NETWORKING_SOCKET_OPS_HPP
#define FOUNDATIONKIT_NETWORKING_SOCKET_OPS_HPP

#include <memory>
#include "FoundationKit/GenericPlatformMacros.hpp"
#include "FoundationKit/Base/error_code.hpp"
#include "Networking/socket_types.hpp"
#include "Networking/detail/config.hpp"

NS_FK_BEGIN
namespace network{
namespace socket_ops
{
    enum
    {
        // The user wants a non-blocking socket.
        non_blocking = 1,

        // User wants connection_aborted errors, which are disabled by default.
        enable_connection_aborted = 2,

        // The user set the linger option. Needs to be checked when closing.
        user_set_linger = 4,

        // The socket is stream-oriented.
        stream_oriented = 8,

        // The socket is datagram-oriented.
        datagram_oriented = 16,

        // The socket may have been dup()-ed.
        possible_dup = 32

    };
    typedef unsigned char state_type;


    NETWORK_API socket_type socket(int af, int type, int protocol, std::error_code& ec);

    NETWORK_API int connect(socket_type s, const socket_addr_type* addr,
        int addrlen, std::error_code& ec);

    NETWORK_API void sync_connect(socket_type s, const socket_addr_type* addr,
        int addrlen, std::error_code& ec);

    NETWORK_API socket_type accept(socket_type s, socket_addr_type* addr,
        int* addrlen, std::error_code& ec);

    NETWORK_API int listen(socket_type s, int backlog, std::error_code& ec);

    NETWORK_API int bind(socket_type s, const socket_addr_type* addr, std::size_t addrlen, std::error_code& ec);

    NETWORK_API signed_size_type recv(socket_type s, char* bufs,
        int len, int flags, std::error_code& ec);

    NETWORK_API signed_size_type recvfrom(socket_type s, char* bufs,
        int count, int flags, socket_addr_type* addr, 
        int* addrlen, std::error_code& ec);

    NETWORK_API signed_size_type recvmsg(socket_type s, char* bufs,
        int count, int in_flags, 
        int& out_flags, std::error_code& ec);

    NETWORK_API signed_size_type send(socket_type s, const char* bufs,
        int count, int flags, std::error_code& ec);

    NETWORK_API signed_size_type sendto(socket_type s, const char* bufs,
        int count, int flags, const socket_addr_type* addr, 
        int addrlen, std::error_code& ec);

    NETWORK_API int shutdown(socket_type s, int what, std::error_code& ec);

    NETWORK_API int close(socket_type s, state_type& state, bool destruction, std::error_code& ec);

    NETWORK_API int setsockopt(socket_type s, state_type& state,
        int level, int optname, const void* optval,
        int optlen, std::error_code& ec);

    NETWORK_API int getsockopt(socket_type s, state_type state,
        int level, int optname, void* optval,
        int* optlen, std::error_code& ec);

    NETWORK_API int getpeername(socket_type s, socket_addr_type* addr,
        int* addrlen, bool cached, std::error_code& ec);

    NETWORK_API int getsockname(socket_type s, socket_addr_type* addr,
        int* addrlen, std::error_code& ec);

    NETWORK_API bool set_non_blocking(socket_type s, state_type& state, bool value, std::error_code& ec);

    NETWORK_API int socketpair(int af, int type, int protocol, socket_type sv[2], std::error_code& ec);

    NETWORK_API bool sockatmark(socket_type s, std::error_code& ec);

    NETWORK_API size_t available(socket_type s, std::error_code& ec);

    NETWORK_API int ioctl(socket_type s, state_type& state,
        int cmd, ioctl_arg_type* arg, std::error_code& ec);

    NETWORK_API int select(int nfds, fd_set* readfds, fd_set* writefds,
        fd_set* exceptfds, timeval* timeout, std::error_code& ec);

    NETWORK_API int poll_read(socket_type s, state_type state, std::error_code& ec);

    NETWORK_API int poll_write(socket_type s, state_type state, std::error_code& ec);

    NETWORK_API int poll_error(socket_type s, state_type state, std::error_code& ec);

    NETWORK_API int poll_connect(socket_type s, std::error_code& ec);

    NETWORK_API const char* inet_ntop(int af, const void* src, char* dest,
        size_t length, unsigned long scope_id, std::error_code& ec);

    NETWORK_API int inet_pton(int af, const char* src, void* dest,
        unsigned long* scope_id, std::error_code& ec);

    NETWORK_API int gethostname(char* name,
        int namelen, std::error_code& ec);

    NETWORK_API u_long_type network_to_host_long(u_long_type value);
    NETWORK_API u_long_type host_to_network_long(u_long_type value);
    NETWORK_API u_short_type network_to_host_short(u_short_type value);
    NETWORK_API u_short_type host_to_network_short(u_short_type value);

#if (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
    typedef WSABUF buf;
#else // defined(ASIO_WINDOWS) || defined(__CYGWIN__)
    typedef iovec buf;
#endif // defined(ASIO_WINDOWS) || defined(__CYGWIN__)

    NETWORK_API void init_buf(buf& b, void* data, size_t size);

    NETWORK_API void init_buf(buf& b, const void* data, size_t size);

} // namespace socket_ops
} // namespace network
NS_FK_END

#include "Networking/detail/socket_ops.ipp"


#endif // END OF FOUNDATIONKIT_NETWORKING_SOCKET_OPS_HPP
