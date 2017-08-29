/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_NETWORKING_SOCKET_OPS_IPP
#define FOUNDATIONKIT_NETWORKING_SOCKET_OPS_IPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <new>
#include "FoundationKit/GenericPlatformMacros.hpp"
#include "FoundationKit/Base/error_code.hpp"
#include "Networking/detail/socket_ops.hpp"
#include "Networking/socket_types.hpp"

NS_FK_BEGIN
namespace network{
namespace socket_ops
{
    // ======================== Helper Method ============================

    inline void init_buf_iov_base(void*& base, void* addr)
    {
        base = addr;
    }

    template <typename T>
    inline void init_buf_iov_base(T& base, void* addr)
    {
        base = static_cast<T>(addr);
    }

#if (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
    typedef WSABUF buf;
#else // (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
    typedef iovec buf;
#endif // (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)

    void init_buf(buf& b, void* data, size_t size)
    {
#if (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
        b.buf = static_cast<char*>(data);
        b.len = static_cast<u_long>(size);
#else // (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
        init_buf_iov_base(b.iov_base, data);
        b.iov_len = size;
#endif // (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
    }

    void init_buf(buf& b, const void* data, size_t size)
    {
#if (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
        b.buf = static_cast<char*>(const_cast<void*>(data));
        b.len = static_cast<u_long>(size);
#else // (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
        init_buf_iov_base(b.iov_base, const_cast<void*>(data));
        b.iov_len = size;
#endif // (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
    }

    inline void init_msghdr_msg_name(void*& name, socket_addr_type* addr)
    {
        name = addr;
    }

    inline void init_msghdr_msg_name(void*& name, const socket_addr_type* addr)
    {
        name = const_cast<socket_addr_type*>(addr);
    }

    template <typename T>
    inline void init_msghdr_msg_name(T& name, socket_addr_type* addr)
    {
        name = reinterpret_cast<T>(addr);
    }

    template <typename T>
    inline void init_msghdr_msg_name(T& name, const socket_addr_type* addr)
    {
        name = reinterpret_cast<T>(const_cast<socket_addr_type*>(addr));
    }

    inline void clear_last_error()
    {
#if (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
        WSASetLastError(0);
#else
        errno = 0;
#endif
    }

    inline int get_error_code()
    {
#if TARGET_PLATFORM == PLATFORM_WINDOWS || defined(__CYGWIN__)
        return WinErrorCodeToErrc(WSAGetLastError());
#else
        return errno;
#endif
    }

    template <typename ReturnType>
    inline ReturnType error_wrapper(ReturnType return_value, std::error_code& ec)
    {
        ec = std::error_code(get_error_code(), std::generic_category());
        return return_value;
    }


    // ======================== Socket Native Method ============================
    socket_type socket(int af, int type, int protocol, std::error_code& ec)
    {
        socket_type s = error_wrapper(::socket(af, type, protocol), ec);
        if (s == invalid_socket)
            return s;

#if (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
        if (af == NET_OS_DEF(AF_INET6))
        {
            int optval = 0;
            ::setsockopt(s, IPPROTO_IPV6, IPV6_V6ONLY,reinterpret_cast<const char*>(&optval), sizeof(optval));
        }
#elif defined(__MACH__) && defined(__APPLE__) || defined(__FreeBSD__)
        int optval = 1;
        int result = error_wrapper(::setsockopt(s,SOL_SOCKET, SO_NOSIGPIPE, &optval, sizeof(optval)), ec);
        if (result != 0)
        {
            ::close(s);
            return invalid_socket;
        }
#else
        if (s >= 0)
            ec = std::error_code();
#endif
        return s;
    }

    int connect(socket_type s, const socket_addr_type* addr, int addrlen, std::error_code& ec)
    {
        if (s == invalid_socket)
        {
            ec = make_error_code(std::errc::bad_file_descriptor);
            return socket_error_retval;
        }

        clear_last_error();
        int result = error_wrapper(::connect(s, addr, addrlen), ec);
        if (result == 0)
            ec = std::error_code();
#if defined(__linux__)
        else if (ec == std::errc::resource_unavailable_try_again)
            ec = make_error_code(std::errc::no_buffer_space);
#endif // defined(__linux__)
        return result;
    }

    void sync_connect(socket_type s, const socket_addr_type* addr,
        int addrlen, std::error_code& ec)
    {
        // Perform the connect operation.
        socket_ops::connect(s, addr, addrlen, ec);
        if (ec.value() != static_cast<int>(std::errc::operation_in_progress) 
            && ec.value() != static_cast<int>(std::errc::operation_would_block))
        {
            // The connect operation finished immediately.
            return;
        }
        // Wait for socket to become ready.
        if (socket_ops::poll_connect(s, ec) < 0)
            return;

        // Get the error code from the connect operation.
        int connect_error = 0;
        int connect_error_len = sizeof(connect_error);
        if (socket_ops::getsockopt(s, 0, SOL_SOCKET, SO_ERROR,&connect_error, &connect_error_len, ec) == socket_error_retval)
            return;

        // Return the result of the connect operation.
        ec = std::error_code(WinErrorCodeToErrc(connect_error), std::generic_category());
    }

    socket_type accept(socket_type s, socket_addr_type* addr, int* addrlen, std::error_code& ec)
    {
        if (s == invalid_socket)
        {
            ec = make_error_code(std::errc::bad_file_descriptor);
            return invalid_socket;
        }
        clear_last_error();
        socket_type new_s = error_wrapper(::accept(s, addr, addrlen), ec);
        if (new_s == invalid_socket)
            return new_s;

#if defined(__MACH__) && defined(__APPLE__) || defined(__FreeBSD__)
        int optval = 1;
        int result = error_wrapper(::setsockopt(new_s,SOL_SOCKET, SO_NOSIGPIPE, &optval, sizeof(optval)), ec);
        if (result != 0)
        {
            ::close(new_s);
            return invalid_socket;
        }
#endif

        ec = std::error_code();
        return new_s;
    }

    int listen(socket_type s, int backlog, std::error_code& ec)
    {
        if (s == invalid_socket)
        {
            ec = make_error_code(std::errc::bad_file_descriptor);
            return socket_error_retval;
        }

        clear_last_error();
        int result = error_wrapper(::listen(s, backlog), ec);
        if (result == 0)
            ec = std::error_code();
        return result;
    }

    int bind(socket_type s, const socket_addr_type* addr, std::size_t addrlen, std::error_code& ec)
    {
        if (s == invalid_socket)
        {
            ec = make_error_code(std::errc::bad_file_descriptor);
            return socket_error_retval;
        }

        clear_last_error();
        int result = error_wrapper(::bind(s, addr,addrlen), ec);
        if (result == 0)
            ec = std::error_code();
        return result;
    }

    signed_size_type recv(socket_type s, char* bufs, int count,
        int flags, std::error_code& ec)
    {
        clear_last_error();
#if (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
        // Receive some data.
        signed_size_type result = error_wrapper(::recv(s, bufs, count, flags), ec);
        if (ec.value() == ERROR_NETNAME_DELETED)
            ec = make_error_code(std::errc::connection_reset);
        else if (ec.value() == ERROR_PORT_UNREACHABLE)
            ec = make_error_code(std::errc::connection_refused);
        if (result >= 0)
            ec = std::error_code();
        return result;
#else // (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
        socket_ops::buf b;
        socket_ops::init_buf(b, bufs, sizeof(count));
        msghdr msg = msghdr();
        msg.msg_iov = &b;
        msg.msg_iovlen = 1;
        signed_size_type result = error_wrapper(::recvmsg(s, &msg, flags), ec);
        if (result >= 0)
            ec = std::error_code();
        return result;
#endif // (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
    }

    signed_size_type recvfrom(socket_type s, char* bufs, int count,
        int flags, socket_addr_type* addr, int* addrlen,
        std::error_code& ec)
    {
        clear_last_error();
#if (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
        // Receive some data.
        signed_size_type result = error_wrapper(::recvfrom(s, bufs, count, flags, addr, addrlen), ec);
        if (ec.value() == ERROR_NETNAME_DELETED)
            ec = std::make_error_code(std::errc::connection_reset);
        else if (ec.value() == ERROR_PORT_UNREACHABLE)
            ec = std::make_error_code(std::errc::connection_refused);
        if (result >= 0)
            ec = std::error_code();
        return result;
#else // (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
        msghdr msg = msghdr();
        init_msghdr_msg_name(msg.msg_name, addr);
        msg.msg_namelen = *addrlen;
        socket_ops::buf b;
        socket_ops::init_buf(b, bufs, sizeof(count));
        msg.msg_iov = &b;
        msg.msg_iovlen = 1;
        signed_size_type result = error_wrapper(::recvmsg(s, &msg, flags), ec);
        *addrlen = msg.msg_namelen;
        if (result >= 0)
            ec = std::error_code();
        return result;
#endif // (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
    }


    signed_size_type recvmsg(socket_type s, char* bufs, int count,
        int in_flags, int& out_flags, std::error_code& ec)
    {
        clear_last_error();
#if (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
        out_flags = 0;
        return socket_ops::recv(s, bufs, count, in_flags, ec);
#else // (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
        msghdr msg = msghdr();
        socket_ops::buf b;
        socket_ops::init_buf(b, bufs, sizeof(count));
        msg.msg_iov = &b;
        msg.msg_iovlen = 1;
        signed_size_type result = error_wrapper(::recvmsg(s, &msg, in_flags), ec);
        if (result >= 0)
        {
            ec = std::error_code();
            out_flags = msg.msg_flags;
        }
        else
            out_flags = 0;
        return result;
#endif //(TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
    }

    signed_size_type send(socket_type s, const char* bufs, int count,
        int flags, std::error_code& ec)
    {
        clear_last_error();
#if (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
        // Send the data.
        signed_size_type result = error_wrapper(::send(s, bufs, count, flags), ec);
        if (ec.value() == ERROR_NETNAME_DELETED)
            ec = make_error_code(std::errc::connection_reset);
        else if (ec.value() == ERROR_PORT_UNREACHABLE)
            ec = make_error_code(std::errc::connection_refused);
        if (result >= 0)
            ec = std::error_code();
        return result;
#else // (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
        msghdr msg = msghdr();
        socket_ops::buf b;
        socket_ops::init_buf(b, bufs, sizeof(count));
        msg.msg_iov = &b;
        msg.msg_iovlen = 1;
        flags |= MSG_NOSIGNAL;
        signed_size_type result = error_wrapper(::sendmsg(s, &msg, flags), ec);
        if (result >= 0)
            ec = std::error_code();
        return result;
#endif // (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
    }

    signed_size_type sendto(socket_type s, const char* bufs, int count,
        int flags, const socket_addr_type* addr, int addrlen,
        std::error_code& ec)
    {
        clear_last_error();
#if (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
        // Send the data.
        signed_size_type result = error_wrapper(::sendto(s, bufs, count, flags, addr, addrlen), ec);
        if (ec.value() == ERROR_NETNAME_DELETED)
            ec = make_error_code(std::errc::connection_reset);
        else if (ec.value() == ERROR_PORT_UNREACHABLE)
            ec = make_error_code(std::errc::connection_refused);
        if (result >= 0)
            ec = std::error_code();
        return result;
#else // (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
        msghdr msg = msghdr();
        init_msghdr_msg_name(msg.msg_name, addr);
        msg.msg_namelen = addrlen;
        socket_ops::buf b;
        socket_ops::init_buf(b, bufs, sizeof(count));
        msg.msg_iov = &b;
        msg.msg_iovlen = 1;
        flags |= MSG_NOSIGNAL;
        signed_size_type result = error_wrapper(::sendmsg(s, &msg, flags), ec);
        if (result >= 0)
            ec = std::error_code();
        return result;
#endif // (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
    }

    int shutdown(socket_type s, int what, std::error_code& ec)
    {
        if (s == invalid_socket)
        {
            ec = make_error_code(std::errc::bad_file_descriptor);
            return socket_error_retval;
        }
        clear_last_error();
        int result = error_wrapper(::shutdown(s, what), ec);
        if (result == 0)
            ec = std::error_code();
        return result;
    }

    int close(socket_type s, state_type& state, bool destruction, std::error_code& ec)
    {
        int result = 0;
        if (s != invalid_socket)
        {
            // We don't want the destructor to block, so set the socket to linger in
            // the background. If the user doesn't like this behaviour then they need
            // to explicitly close the socket.
            if (destruction && (state & user_set_linger))
            {
                ::linger opt;
                opt.l_onoff = 0;
                opt.l_linger = 0;
                std::error_code ignored_ec;
                socket_ops::setsockopt(s, state, SOL_SOCKET, SO_LINGER, &opt, sizeof(opt), ignored_ec);
            }

            clear_last_error();
#if (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
            result = error_wrapper(::closesocket(s), ec);
#else // (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
            result = error_wrapper(::close(s), ec);
#endif // (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)

            if (result != 0
                && (ec == std::errc::operation_would_block
                || ec == std::errc::resource_unavailable_try_again))
            {
                // According to UNIX Network Programming Vol. 1, it is possible for
                // close() to fail with EWOULDBLOCK under certain circumstances. What
                // isn't clear is the state of the descriptor after this error. The one
                // current OS where this behaviour is seen, Windows, says that the socket
                // remains open. Therefore we'll put the descriptor back into blocking
                // mode and have another attempt at closing it.
#if (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
                ioctl_arg_type arg = 0;
                ::ioctlsocket(s, FIONBIO, &arg);
#else // (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
# if defined(__SYMBIAN32__)
                int flags = ::fcntl(s, F_GETFL, 0);
                if (flags >= 0)
                    ::fcntl(s, F_SETFL, flags & ~O_NONBLOCK);
# else // defined(__SYMBIAN32__)
                ioctl_arg_type arg = 0;
                ::ioctl(s, FIONBIO, &arg);
# endif // defined(__SYMBIAN32__)
#endif // (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
                state &= ~non_blocking;

                clear_last_error();
#if (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
                result = error_wrapper(::closesocket(s), ec);
#else // (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
                result = error_wrapper(::close(s), ec);
#endif // (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
            }
        }

        if (result == 0)
            ec = std::error_code();
        return result;
    }


    int setsockopt(socket_type s, state_type& state, int level, int optname,
        const void* optval, int optlen, std::error_code& ec)
    {
        if (s == invalid_socket)
        {
            ec = make_error_code(std::errc::bad_file_descriptor);
            return socket_error_retval;
        }

        if (level == custom_socket_option_level && optname == always_fail_option)
        {
            ec = make_error_code(std::errc::invalid_argument);
            return socket_error_retval;
        }

        if (level == custom_socket_option_level
            && optname == enable_connection_aborted_option)
        {
            if (*static_cast<const int*>(optval))
                state |= enable_connection_aborted;
            else
                state &= ~enable_connection_aborted;
            ec = std::error_code();
            return 0;
        }

        if (level == SOL_SOCKET && optname == SO_LINGER)
            state |= user_set_linger;

        clear_last_error();
        int result = error_wrapper(::setsockopt(s, level, optname, reinterpret_cast<const char*>(optval), optlen), ec);
        if (result == 0)
        {
            ec = std::error_code();

#if defined(__MACH__) && defined(__APPLE__) || defined(__NetBSD__) || defined(__FreeBSD__) || defined(__OpenBSD__)
            // To implement portable behaviour for SO_REUSEADDR with UDP sockets we
            // need to also set SO_REUSEPORT on BSD-based platforms.
            if ((state & datagram_oriented) && level == SOL_SOCKET && optname == SO_REUSEADDR)
            {
                ::setsockopt(s, SOL_SOCKET, SO_REUSEPORT, reinterpret_cast<const char*>(optval), optlen);
            }
#endif
        }
        return result;
    }



    int getsockopt(socket_type s, state_type state, int level, int optname,
        void* optval, int* optlen, std::error_code& ec)
    {
        if (s == invalid_socket)
        {
            ec = make_error_code(std::errc::bad_file_descriptor);
            return socket_error_retval;
        }

        if (level == custom_socket_option_level && optname == always_fail_option)
        {
            ec = make_error_code(std::errc::invalid_argument);
            return socket_error_retval;
        }

        if (level == custom_socket_option_level
            && optname == enable_connection_aborted_option)
        {
            *static_cast<int*>(optval) = (state & enable_connection_aborted) ? 1 : 0;
            ec = std::error_code();
            return 0;
        }

#if (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
        clear_last_error();
        int tmp_optlen = *optlen;
        int result = error_wrapper(::getsockopt(s, level, optname, (char*)optval, &tmp_optlen), ec);
        *optlen = tmp_optlen;
        if (result != 0 && level == IPPROTO_IPV6 && optname == IPV6_V6ONLY && ec.value() == WSAENOPROTOOPT)
        {
            // Dual-stack IPv4/v6 sockets, and the IPV6_V6ONLY socket option, are only
            // supported on Windows Vista and later. To simplify program logic we will
            // fake success of getting this option and specify that the value is
            // non-zero (i.e. true). This corresponds to the behavior of IPv6 sockets
            // on Windows platforms pre-Vista.
            *static_cast<int*>(optval) = 1;
            ec = std::error_code();
        }
        if (result == 0)
            ec = std::error_code();
        return result;
#else // (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
        clear_last_error();
        int tmp_optlen = *optlen;
        int result = error_wrapper(::getsockopt(s, level, optname, (char*)optval, &tmp_optlen), ec);
        *optlen = tmp_optlen;
#if defined(__linux__)
        if (result == 0 && level == SOL_SOCKET && *optlen == sizeof(int) && (optname == SO_SNDBUF || optname == SO_RCVBUF))
        {
            // On Linux, setting SO_SNDBUF or SO_RCVBUF to N actually causes the kernel
            // to set the buffer size to N*2. Linux puts additional stuff into the
            // buffers so that only about half is actually available to the application.
            // The retrieved value is divided by 2 here to make it appear as though the
            // correct value has been set.
            *static_cast<int*>(optval) /= 2;
        }
#endif // defined(__linux__)
        if (result == 0)
            ec = std::error_code();
        return result;
#endif // (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
    }


    int getpeername(socket_type s, socket_addr_type* addr,
        int* addrlen, bool cached, std::error_code& ec)
    {
        if (s == invalid_socket)
        {
            ec = make_error_code(std::errc::bad_file_descriptor);
            return socket_error_retval;
        }

#if (TARGET_PLATFORM == PLATFORM_WINDOWS)|| defined(__CYGWIN__)
        if (cached)
        {
            // Check if socket is still connected.
            DWORD connect_time = 0;
            int connect_time_len = sizeof(connect_time);
            if (socket_ops::getsockopt(s, 0, SOL_SOCKET, SO_CONNECT_TIME,
                &connect_time, &connect_time_len, ec) == socket_error_retval)
            {
                return socket_error_retval;
            }
            if (connect_time == 0xFFFFFFFF)
            {
                ec = make_error_code(std::errc::not_connected);
                return socket_error_retval;
            }

            // The cached value is still valid.
            ec = std::error_code();
            return 0;
        }
#else // (TARGET_PLATFORM == PLATFORM_WINDOWS)|| defined(__CYGWIN__)
        (void)cached;
#endif // (TARGET_PLATFORM == PLATFORM_WINDOWS)|| defined(__CYGWIN__)

        clear_last_error();
        int result = error_wrapper(::getpeername(s, addr, addrlen), ec);
        if (result == 0)
            ec = std::error_code();
        return result;
    }

    int getsockname(socket_type s, socket_addr_type* addr,
        int* addrlen, std::error_code& ec)
    {
        if (s == invalid_socket)
        {
            ec = make_error_code(std::errc::bad_file_descriptor);
            return socket_error_retval;
        }

        clear_last_error();
        int result = error_wrapper(::getsockname(s, addr, addrlen), ec);
        if (result == 0)
            ec = std::error_code();
        return result;
    }


    bool set_non_blocking(socket_type s, state_type& state, bool value, std::error_code& ec)
    {
        if (s == invalid_socket)
        {
            ec = make_error_code(std::errc::bad_file_descriptor);
            return false;
        }

        clear_last_error();
#if (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
        ioctl_arg_type arg = (value ? 1 : 0);
        int result = error_wrapper(::ioctlsocket(s, FIONBIO, &arg), ec);
#else
        ioctl_arg_type arg = (value ? 1 : 0);
        int result = error_wrapper(::ioctl(s, FIONBIO, &arg), ec);
#endif
        if (result >= 0)
        {
            ec = std::error_code();
            if (value)
                state |= non_blocking;
            else
            {
                // Clearing the user-set non-blocking mode always overrides any
                // internally-set non-blocking flag. Any subsequent asynchronous
                // operations will need to re-enable non-blocking I/O.
                state &= ~non_blocking;
            }
            return true;
        }

        return false;
    }


    int socketpair(int af, int type, int protocol, socket_type sv[2], std::error_code& ec)
    {
#if (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
        (void)(af);
        (void)(type);
        (void)(protocol);
        (void)(sv);
        ec = make_error_code(std::errc::operation_not_supported);
        return socket_error_retval;
#else
        clear_last_error();
        int result = error_wrapper(::socketpair(af, type, protocol, sv), ec);
        if (result == 0)
            ec = std::error_code();
        return result;
#endif
    }

    bool sockatmark(socket_type s, std::error_code& ec)
    {
        if (s == invalid_socket)
        {
            ec = make_error_code(std::errc::bad_file_descriptor);
            return false;
        }

#if defined(SIOCATMARK)
        ioctl_arg_type value = 0;
# if (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
        int result = error_wrapper(::ioctlsocket(s, SIOCATMARK, &value), ec);
# else // (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
        int result = error_wrapper(::ioctl(s, SIOCATMARK, &value), ec);
# endif // (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
        if (result == 0)
            ec = std::error_code();
# if defined(ENOTTY)
        if (ec.value() == ENOTTY)
            ec = make_error_code(std::errc::not_a_socket);
# endif // defined(ENOTTY)
#else // defined(SIOCATMARK)
        int value = error_wrapper(::sockatmark(s), ec);
        if (value != -1)
            ec = std::error_code();
#endif // defined(SIOCATMARK)

        return ec ? false : value != 0;
    }

    size_t available(socket_type s, std::error_code& ec)
    {
        if (s == invalid_socket)
        {
            ec = make_error_code(std::errc::bad_file_descriptor);
            return 0;
        }

        ioctl_arg_type value = 0;
#if (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
        int result = error_wrapper(::ioctlsocket(s, FIONREAD, &value), ec);
#else // (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
        int result = error_wrapper(::ioctl(s, FIONREAD, &value), ec);
#endif // (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
        if (result == 0)
            ec = std::error_code();
#if defined(ENOTTY)
        if (ec.value() == ENOTTY)
            ec = make_error_code(std::errc::not_a_socket);
#endif // defined(ENOTTY)

        return ec ? static_cast<size_t>(0) : static_cast<size_t>(value);
    }

    int ioctl(socket_type s, state_type& state, int cmd,
        ioctl_arg_type* arg, std::error_code& ec)
    {
        if (s == invalid_socket)
        {
            ec = make_error_code(std::errc::bad_file_descriptor);
            return socket_error_retval;
        }

        clear_last_error();
#if (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
        int result = error_wrapper(::ioctlsocket(s, cmd, arg), ec);
#elif defined(__MACH__) && defined(__APPLE__) || defined(__NetBSD__) || defined(__FreeBSD__) || defined(__OpenBSD__)
        int result = error_wrapper(::ioctl(s,static_cast<unsigned int>(cmd), arg), ec);
#else
        int result = error_wrapper(::ioctl(s, cmd, arg), ec);
#endif
        if (result >= 0)
        {
            ec = std::error_code();

            // When updating the non-blocking mode we always perform the ioctl syscall,
            // even if the flags would otherwise indicate that the socket is already in
            // the correct state. This ensures that the underlying socket is put into
            // the state that has been requested by the user. If the ioctl syscall was
            // successful then we need to update the flags to match.
            if (cmd == static_cast<int>(FIONBIO))
            {
                if (*arg)
                {
                    state |= non_blocking;
                }
                else
                {
                    // Clearing the non-blocking mode always overrides any internally-set
                    // non-blocking flag. Any subsequent asynchronous operations will need
                    // to re-enable non-blocking I/O.
                    state &= ~non_blocking;
                }
            }
        }

        return result;
    }

    int select(int nfds, fd_set* readfds, fd_set* writefds,
        fd_set* exceptfds, timeval* timeout, std::error_code& ec)
    {
        clear_last_error();
#if (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
        if (!readfds && !writefds && !exceptfds && timeout)
        {
            DWORD milliseconds = timeout->tv_sec * 1000 + timeout->tv_usec / 1000;
            if (milliseconds == 0)
                milliseconds = 1; // Force context switch.
            ::Sleep(milliseconds);
            ec = std::error_code();
            return 0;
        }

        // The select() call allows timeout values measured in microseconds, but the
        // system clock (as wrapped by boost::posix_time::microsec_clock) typically
        // has a resolution of 10 milliseconds. This can lead to a spinning select
        // reactor, meaning increased CPU usage, when waiting for the earliest
        // scheduled timeout if it's less than 10 milliseconds away. To avoid a tight
        // spin we'll use a minimum timeout of 1 millisecond.
        if (timeout && timeout->tv_sec == 0
            && timeout->tv_usec > 0 && timeout->tv_usec < 1000)
            timeout->tv_usec = 1000;
#endif // (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)

#if defined(__hpux) && defined(__SELECT)
        timespec ts;
        ts.tv_sec = timeout ? timeout->tv_sec : 0;
        ts.tv_nsec = timeout ? timeout->tv_usec * 1000 : 0;
        return error_wrapper(::pselect(nfds, readfds,
            writefds, exceptfds, timeout ? &ts : 0, 0), ec);
#else
        int result = error_wrapper(::select(nfds, readfds,
            writefds, exceptfds, timeout), ec);
        if (result >= 0)
            ec = std::error_code();
        return result;
#endif
    }

    int poll_read(socket_type s, state_type state, std::error_code& ec)
    {
        if (s == invalid_socket)
        {
            ec = make_error_code(std::errc::bad_file_descriptor);
            return socket_error_retval;
        }

#if (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)|| defined(__SYMBIAN32__)
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(s, &fds);
        timeval zero_timeout;
        zero_timeout.tv_sec = 0;
        zero_timeout.tv_usec = 0;
        timeval* timeout = (state & non_blocking) ? &zero_timeout : 0;
        clear_last_error();
        int result = error_wrapper(::select(s + 1, &fds, 0, 0, timeout), ec);

#else // (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)|| defined(__SYMBIAN32__)
        pollfd fds;
        fds.fd = s;
        fds.events = POLLIN;
        fds.revents = 0;
        int timeout = (state & non_blocking) ? 0 : -1;
        clear_last_error();
        int result = error_wrapper(::poll(&fds, 1, timeout), ec);

#endif // (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)|| defined(__SYMBIAN32__)

        if (result == 0)
            ec = (state & non_blocking)
            ? make_error_code(std::errc::operation_would_block) : std::error_code();
        else if (result > 0)
            ec = std::error_code();
        return result;
    }

    int poll_write(socket_type s, state_type state, std::error_code& ec)
    {
        if (s == invalid_socket)
        {
            ec = make_error_code(std::errc::bad_file_descriptor);
            return socket_error_retval;
        }

#if (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)|| defined(__SYMBIAN32__)
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(s, &fds);
        timeval zero_timeout;
        zero_timeout.tv_sec = 0;
        zero_timeout.tv_usec = 0;
        timeval* timeout = (state & non_blocking) ? &zero_timeout : 0;
        clear_last_error();
        int result = error_wrapper(::select(s + 1, 0, &fds, 0, timeout), ec);

#else // (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)|| defined(__SYMBIAN32__)
        pollfd fds;
        fds.fd = s;
        fds.events = POLLOUT;
        fds.revents = 0;
        int timeout = (state & non_blocking) ? 0 : -1;
        clear_last_error();
        int result = error_wrapper(::poll(&fds, 1, timeout), ec);

#endif // (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)|| defined(__SYMBIAN32__)

        if (result == 0)
            ec = (state & non_blocking)
            ? make_error_code(std::errc::operation_would_block) : std::error_code();
        else if (result > 0)
            ec = std::error_code();
        return result;
    }

    int poll_error(socket_type s, state_type state, std::error_code& ec)
    {
        if (s == invalid_socket)
        {
            ec = make_error_code(std::errc::bad_file_descriptor);
            return socket_error_retval;
        }

#if (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)|| defined(__SYMBIAN32__)
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(s, &fds);
        timeval zero_timeout;
        zero_timeout.tv_sec = 0;
        zero_timeout.tv_usec = 0;
        timeval* timeout = (state & non_blocking) ? &zero_timeout : 0;
        clear_last_error();
        int result = error_wrapper(::select(s + 1, 0, 0, &fds, timeout), ec);

#else // (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)|| defined(__SYMBIAN32__)
        pollfd fds;
        fds.fd = s;
        fds.events = POLLPRI | POLLERR | POLLHUP;
        fds.revents = 0;
        int timeout = (state & non_blocking) ? 0 : -1;
        clear_last_error();
        int result = error_wrapper(::poll(&fds, 1, timeout), ec);

#endif // (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)|| defined(__SYMBIAN32__)
        if (result >= 0)
            ec = std::error_code();
        return result;
    }

    int poll_connect(socket_type s, std::error_code& ec)
    {
        if (s == invalid_socket)
        {
            ec = make_error_code(std::errc::bad_file_descriptor);
            return socket_error_retval;
        }

#if (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)|| defined(__SYMBIAN32__)
        fd_set write_fds;
        FD_ZERO(&write_fds);
        FD_SET(s, &write_fds);
        fd_set except_fds;
        FD_ZERO(&except_fds);
        FD_SET(s, &except_fds);
        clear_last_error();
        int result = error_wrapper(::select(
            s + 1, 0, &write_fds, &except_fds, 0), ec);
        if (result >= 0)
            ec = std::error_code();
        return result;

#else // (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)|| defined(__SYMBIAN32__)
        pollfd fds;
        fds.fd = s;
        fds.events = POLLOUT;
        fds.revents = 0;
        clear_last_error();
        int result = error_wrapper(::poll(&fds, 1, -1), ec);
        if (result >= 0)
            ec = std::error_code();
        return result;
#endif // (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)|| defined(__SYMBIAN32__)
    }

    const char* inet_ntop(int af, const void* src, char* dest, size_t length,
        unsigned long scope_id, std::error_code& ec)
    {
        clear_last_error();
#if (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
        if (af != NET_OS_DEF(AF_INET) && af != NET_OS_DEF(AF_INET6))
        {
            ec = make_error_code(std::errc::address_family_not_supported);
            return 0;
        }

        union
        {
            socket_addr_type base;
            sockaddr_storage_type storage;
            sockaddr_in4_type v4;
            sockaddr_in6_type v6;
        } address;
        DWORD address_length;
        if (af == NET_OS_DEF(AF_INET))
        {
            address_length = sizeof(sockaddr_in4_type);
            address.v4.sin_family = NET_OS_DEF(AF_INET);
            address.v4.sin_port = 0;
            memcpy(&address.v4.sin_addr, src, sizeof(in4_addr_type));
        }
        else // AF_INET6
        {
            address_length = sizeof(sockaddr_in6_type);
            address.v6.sin6_family = NET_OS_DEF(AF_INET6);
            address.v6.sin6_port = 0;
            address.v6.sin6_flowinfo = 0;
            address.v6.sin6_scope_id = scope_id;
            memcpy(&address.v6.sin6_addr, src, sizeof(in6_addr_type));
        }

        DWORD string_length = static_cast<DWORD>(length);
#if defined(_MSC_VER) && (_MSC_VER >= 1800)
        LPWSTR string_buffer = (LPWSTR)_alloca(length * sizeof(WCHAR));
        int result = error_wrapper(::WSAAddressToStringW(&address.base,
            address_length, 0, string_buffer, &string_length), ec);
        ::WideCharToMultiByte(CP_ACP, 0, string_buffer, -1,
            dest, static_cast<int>(length), 0, 0);
#else
        int result = error_wrapper(::WSAAddressToStringA(
            &address.base, address_length, 0, dest, &string_length), ec);
#endif

        // Windows may set error code on success.
        if (result != socket_error_retval)
            ec = std::error_code();

        // Windows may not set an error code on failure.
        else if (result == socket_error_retval && !ec)
            ec = make_error_code(std::errc::invalid_argument);

        return result == socket_error_retval ? 0 : dest;
#else // (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
        const char* result = error_wrapper(::inet_ntop(
            af, src, dest, static_cast<int>(length)), ec);
        if (result == 0 && !ec)
            ec = make_error_code(std::errc::invalid_argument);
        if (result != 0 && af == NET_OS_DEF(AF_INET6) && scope_id != 0)
        {
            using namespace std; // For strcat and sprintf.
            char if_name[IF_NAMESIZE + 1] = "%";
            const in6_addr_type* ipv6_address = static_cast<const in6_addr_type*>(src);
            bool is_link_local = ((ipv6_address->s6_addr[0] == 0xfe)
                && ((ipv6_address->s6_addr[1] & 0xc0) == 0x80));
            bool is_multicast_link_local = ((ipv6_address->s6_addr[0] == 0xff)
                && ((ipv6_address->s6_addr[1] & 0x0f) == 0x02));
            if ((!is_link_local && !is_multicast_link_local)
                || if_indextoname(static_cast<unsigned>(scope_id), if_name + 1) == 0)
                sprintf(if_name + 1, "%lu", scope_id);
            strcat(dest, if_name);
        }
        return result;
#endif // (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
    }

    int inet_pton(int af, const char* src, void* dest,
        unsigned long* scope_id, std::error_code& ec)
    {
        clear_last_error();
#if (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
        using namespace std; // For memcpy and strcmp.

        if (af != NET_OS_DEF(AF_INET) && af != NET_OS_DEF(AF_INET6))
        {
            ec = make_error_code(std::errc::address_family_not_supported);
            return -1;
        }

        union
        {
            socket_addr_type base;
            sockaddr_storage_type storage;
            sockaddr_in4_type v4;
            sockaddr_in6_type v6;
        } address;
        int address_length = sizeof(sockaddr_storage_type);
#if defined(_MSC_VER) && (_MSC_VER >= 1800)
        int num_wide_chars = static_cast<int>(strlen(src)) + 1;
        LPWSTR wide_buffer = (LPWSTR)_alloca(num_wide_chars * sizeof(WCHAR));
        ::MultiByteToWideChar(CP_ACP, 0, src, -1, wide_buffer, num_wide_chars);
        int result = error_wrapper(::WSAStringToAddressW(
            wide_buffer, af, 0, &address.base, &address_length), ec);
#else
        int result = error_wrapper(::WSAStringToAddressA(
            const_cast<char*>(src), af, 0, &address.base, &address_length), ec);
#endif

        if (af == NET_OS_DEF(AF_INET))
        {
            if (result != socket_error_retval)
            {
                memcpy(dest, &address.v4.sin_addr, sizeof(in4_addr_type));
                ec = std::error_code();
            }
            else if (strcmp(src, "255.255.255.255") == 0)
            {
                static_cast<in4_addr_type*>(dest)->s_addr = INADDR_NONE;
                ec = std::error_code();
            }
        }
        else // AF_INET6
        {
            if (result != socket_error_retval)
            {
                memcpy(dest, &address.v6.sin6_addr, sizeof(in6_addr_type));
                if (scope_id)
                    *scope_id = address.v6.sin6_scope_id;
                ec = std::error_code();
            }
        }

        // Windows may not set an error code on failure.
        if (result == socket_error_retval && !ec)
            ec = make_error_code(std::errc::invalid_argument);

        if (result != socket_error_retval)
            ec = std::error_code();

        return result == socket_error_retval ? -1 : 1;
#else // (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
        using namespace std; // For strchr, memcpy and atoi.

        // On some platforms, inet_pton fails if an address string contains a scope
        // id. Detect and remove the scope id before passing the string to inet_pton.
        const bool is_v6 = (af == NET_OS_DEF(AF_INET6));
        const char* if_name = is_v6 ? strchr(src, '%') : 0;
        char src_buf[max_addr_v6_str_len + 1];
        const char* src_ptr = src;
        if (if_name != 0)
        {
            if (if_name - src > max_addr_v6_str_len)
            {
                ec = make_error_code(std::errc::invalid_argument);
                return 0;
            }
            memcpy(src_buf, src, if_name - src);
            src_buf[if_name - src] = 0;
            src_ptr = src_buf;
        }

        int result = error_wrapper(::inet_pton(af, src_ptr, dest), ec);
        if (result <= 0 && !ec)
            ec = make_error_code(std::errc::invalid_argument);
        if (result > 0 && is_v6 && scope_id)
        {
            using namespace std; // For strchr and atoi.
            *scope_id = 0;
            if (if_name != 0)
            {
                in6_addr_type* ipv6_address = static_cast<in6_addr_type*>(dest);
                bool is_link_local = ((ipv6_address->s6_addr[0] == 0xfe)
                    && ((ipv6_address->s6_addr[1] & 0xc0) == 0x80));
                bool is_multicast_link_local = ((ipv6_address->s6_addr[0] == 0xff)
                    && ((ipv6_address->s6_addr[1] & 0x0f) == 0x02));
                if (is_link_local || is_multicast_link_local)
                    *scope_id = if_nametoindex(if_name + 1);
                if (*scope_id == 0)
                    *scope_id = atoi(if_name + 1);
            }
        }
        return result;
#endif // (TARGET_PLATFORM == PLATFORM_WINDOWS) || defined(__CYGWIN__)
    }

    int gethostname(char* name, int namelen, std::error_code& ec)
    {
        clear_last_error();
        int result = error_wrapper(::gethostname(name, namelen), ec);
# if (TARGET_PLATFORM == PLATFORM_WINDOWS)
        if (result == 0)
            ec = std::error_code();
# endif // (TARGET_PLATFORM == PLATFORM_WINDOWS)
        return result;
    }


    u_long_type network_to_host_long(u_long_type value)
    {
        return ntohl(value);
    }

    u_long_type host_to_network_long(u_long_type value)
    {
        return htonl(value);
    }

    u_short_type network_to_host_short(u_short_type value)
    {
        return ntohs(value);
    }

    u_short_type host_to_network_short(u_short_type value)
    {
        return htons(value);
    }

} // namespace socket_ops
} // namespace network
NS_FK_END

#endif // END OF FOUNDATIONKIT_NETWORKING_SOCKET_OPS_IPP
