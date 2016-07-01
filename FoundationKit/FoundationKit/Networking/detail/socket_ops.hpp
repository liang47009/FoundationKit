#ifndef LOSEMYMIND_SOCKET_OPS_HPP
#define LOSEMYMIND_SOCKET_OPS_HPP

#include <memory>
#include "FoundationKit/GenericPlatformMacros.h"
#include "FoundationKit/Networking/socket_types.hpp"
#include "FoundationKit/Base/error_code.hpp"

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


    socket_type socket(int af, int type, int protocol,std::error_code& ec);

    int connect(socket_type s, const socket_addr_type* addr,
        int addrlen, std::error_code& ec);

    void sync_connect(socket_type s, const socket_addr_type* addr,
        int addrlen, std::error_code& ec);

    socket_type accept(socket_type s, socket_addr_type* addr, 
        int* addrlen, std::error_code& ec);

    int listen(socket_type s, int backlog, std::error_code& ec);

    signed_size_type recv(socket_type s, char* bufs, 
        int len, int flags, std::error_code& ec);

    signed_size_type recvfrom(socket_type s, char* bufs, 
        int count, int flags, socket_addr_type* addr, 
        int* addrlen, std::error_code& ec);

    signed_size_type recvmsg(socket_type s, char* bufs, 
        int count, int in_flags, 
        int& out_flags, std::error_code& ec);

    signed_size_type send(socket_type s, const char* bufs, 
        int count, int flags, std::error_code& ec);

    signed_size_type sendto(socket_type s, const char* bufs,
        int count, int flags, const socket_addr_type* addr, 
        int addrlen, std::error_code& ec);

    int shutdown(socket_type s, int what, std::error_code& ec);

    int close(socket_type s, state_type& state,bool destruction, std::error_code& ec);

    int setsockopt(socket_type s, state_type& state,
        int level, int optname, const void* optval,
        int optlen, std::error_code& ec);

    int getsockopt(socket_type s, state_type state,
        int level, int optname, void* optval,
        int* optlen, std::error_code& ec);

    int getpeername(socket_type s, socket_addr_type* addr,
        int* addrlen, bool cached, std::error_code& ec);

    int getsockname(socket_type s, socket_addr_type* addr,
        int* addrlen, std::error_code& ec);

    bool set_non_blocking(socket_type s,state_type& state, bool value, std::error_code& ec);

    int socketpair(int af, int type, int protocol, socket_type sv[2], std::error_code& ec);

    bool sockatmark(socket_type s, std::error_code& ec);

    size_t available(socket_type s, std::error_code& ec);

    int ioctl(socket_type s, state_type& state,
        int cmd, ioctl_arg_type* arg, std::error_code& ec);

    int select(int nfds, fd_set* readfds, fd_set* writefds,
        fd_set* exceptfds, timeval* timeout, std::error_code& ec);

    int poll_read(socket_type s,
        state_type state, std::error_code& ec);

    int poll_write(socket_type s,
        state_type state, std::error_code& ec);

    int poll_error(socket_type s,
        state_type state, std::error_code& ec);

    int poll_connect(socket_type s, std::error_code& ec);

    const char* inet_ntop(int af, const void* src, char* dest,
        size_t length, unsigned long scope_id, std::error_code& ec);

    int inet_pton(int af, const char* src, void* dest,
        unsigned long* scope_id, std::error_code& ec);

    int gethostname(char* name,
        int namelen, std::error_code& ec);

    u_long_type network_to_host_long(u_long_type value);
    u_long_type host_to_network_long(u_long_type value);
    u_short_type network_to_host_short(u_short_type value);
    u_short_type host_to_network_short(u_short_type value);

} // namespace socket_ops
} // namespace network
NS_FK_END

#include "FoundationKit/Networking/detail/impl/socket_ops.ipp"

#endif // LOSEMYMIND_SOCKET_OPS_HPP
