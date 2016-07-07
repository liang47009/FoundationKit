#ifndef LOSEMYMIND_ASYNC_SOCKET_HPP
#define LOSEMYMIND_ASYNC_SOCKET_HPP
#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <string>
#include <functional>
#include <algorithm>
#include "FoundationKit/GenericPlatformMacros.h"
#include "FoundationKit/Base/Timespan.h"
#include "FoundationKit/Base/DateTime.h"
#include "FoundationKit/Base/error_code.hpp"
#include "FoundationKit/Base/mutable_data.hpp"
#include "FoundationKit/Networking/socket_types.hpp"
#include "FoundationKit/Networking/detail/socket_base.hpp"
#include "FoundationKit/Networking/detail/socket_ops.hpp"
#include "FoundationKit/Networking/basic_socket.hpp"

NS_FK_BEGIN
namespace network{

template<typename Protocol>
class async_socket : public basic_socket<Protocol>
{
public:
    typedef std::function<void()>                                   handler_function;
    typedef std::function<void(const std::error_code)>              async_connect_handler;
    typedef std::function<void(const std::error_code, std::size_t)> async_send_handler;
    typedef std::function<void(const std::error_code, std::size_t)> async_recv_handler;
    typedef std::function<void(const std::error_code, typename Protocol::socket)> async_accept_handler;

public:

    async_socket()
        : basic_socket()
        , _handlerAsyncAccept(nullptr)
        , _handlerAsyncConnect(nullptr)
        , _handlerAsyncRecv(nullptr)
        , _handlerAsyncSend(nullptr)
    {

    }

    async_socket(native_handle_type new_socket)
        : basic_socket(new_socket)
    {

    }

    /**
    * Virtual destructor
    */
    virtual ~async_socket()
    {

    }

    virtual void poll()
    {
        if (_handlerAsyncAccept)  _handlerAsyncAccept();
        if (_handlerAsyncConnect) _handlerAsyncConnect();
        if (_handlerAsyncRecv)    _handlerAsyncRecv();
        if (_handlerAsyncSend)    _handlerAsyncSend();
    }

    /**
     * Start an asynchronous connect.
     * This function is used to asynchronously connect a socket to the specified
     * remote endpoint. The function call always returns immediately.
     *
     * The socket is automatically opened if it is not already open. If the
     * connect fails, and the socket was automatically opened, the socket is
     * not returned to the closed state.
     *
     * @param peer_endpoint The remote endpoint to which the socket will be
     * connected. Copies will be made of the endpoint object as required.
     *
     * @param handler The handler to be called when the connection operation
     * completes. Copies will be made of the handler as required. The function
     * signature of the handler must be:
     * @code void handler(
     *   const std::error_code& error // Result of operation
     * ); @endcode
     * @par Example
     * @code
     * void connect_handler(const std::error_code& error)
     * {
     *   if (!error)
     *   {
     *     // Connect succeeded.
     *   }
     * }
     *
     * ...
     *
     * ip::tcp::socket socket();
     * ip::tcp::endpoint endpoint(ip::address::from_string("1.2.3.4"), 12345);
     * socket.async_connect(endpoint, connect_handler);
     * @endcode
     */
    std::error_code async_connect(const endpoint_type& peer_endpoint, async_connect_handler& handler)
    {
        LOG_ERROR("***** Not Implementation.");
    }

    /**
     * Start an asynchronous send.
     * This function is used to asynchronously send data on the stream socket.
     * The function call always returns immediately.
     *
     * @param buffers One or more data buffers to be sent on the socket. Although
     * the buffers object may be copied as necessary, ownership of the underlying
     * memory blocks is retained by the caller, which must guarantee that they
     * remain valid until the handler is called.
     *
     * @param handler The handler to be called when the send operation completes.
     * Copies will be made of the handler as required. The function signature of
     * the handler must be:
     * @code void handler(
     *   const std::error_code& error,     // Result of operation.
     *   std::size_t sendBytes             // Number of bytes sent.
     * ); @endcode
     *
     * @note The send operation may not transmit all of the data to the peer.
     * Consider using the @ref async_write function if you need to ensure that all
     * data is written before the asynchronous operation completes.
     *
     * @par Example
     * To send a single data buffer use the @ref buffer function as follows:
     * @code
     * socket.async_send(mutable_data(data, size), handler);
     * @endcode
     * See the @ref buffer documentation for information on sending multiple
     * buffers in one go, and how to use it with arrays, std::array or
     * std::vector.
     */
    void async_send(const mutable_data& buffers, async_send_handler& handler)
    {
        this->async_send(buffers, 0, handler);
    }

    /**
     * Start an asynchronous send.
     * This function is used to asynchronously send data on the stream socket.
     * The function call always returns immediately.
     *
     * @param buffers One or more data buffers to be sent on the socket. Although
     * the buffers object may be copied as necessary, ownership of the underlying
     * memory blocks is retained by the caller, which must guarantee that they
     * remain valid until the handler is called.
     *
     * @param flags Flags specifying how the send call is to be made.
     *
     * @param handler The handler to be called when the send operation completes.
     * Copies will be made of the handler as required. The function signature of
     * the handler must be:
     * @code void handler(
     *   const std::error_code& error, // Result of operation.
     *   std::size_t sendBytes           // Number of bytes sent.
     * ); @endcode
     *
     * @note The send operation may not transmit all of the data to the peer.
     * Consider using the @ref async_write function if you need to ensure that all
     * data is written before the asynchronous operation completes.
     *
     * @par Example
     * To send a single data buffer use the @ref buffer function as follows:
     * @code
     * socket.async_send(mutable_data(data, size), 0, handler);
     * @endcode
     * See the @ref buffer documentation for information on sending multiple
     * buffers in one go, and how to use it with arrays, std::array or
     * std::vector.
     */
    void async_send(const mutable_data& buffers, socket_base::message_flags flags, async_send_handler& handler)
    {
        if ((_state & socket_ops::stream_oriented))
        {
            LOG_ERROR("***** Not Implementation.");
        }
        else
        {
            std::error_code ec = make_error_code(std::errc::address_family_not_supported);
            throw_error_if(ec, "async_send");
        }
    }

    /**
     * Start an asynchronous receive.
     * This function is used to asynchronously receive data from the stream
     * socket. The function call always returns immediately.
     *
     * @param buffers One or more buffers into which the data will be received.
     * Although the buffers object may be copied as necessary, ownership of the
     * underlying memory blocks is retained by the caller, which must guarantee
     * that they remain valid until the handler is called.
     *
     * @param handler The handler to be called when the receive operation
     * completes. Copies will be made of the handler as required. The function
     * signature of the handler must be:
     * @code void handler(
     *   const std::error_code& error,  // Result of operation.
     *   std::size_t bytes_transferred  // Number of bytes received.
     * ); @endcode
     *
     * @note The receive operation may not receive all of the requested number of
     * bytes. Consider using the @ref async_read function if you need to ensure
     * that the requested amount of data is received before the asynchronous
     * operation completes.
     *
     * @par Example
     * To receive into a single data buffer use the @ref buffer function as
     * follows:
     * @code
     * socket.async_receive(mutable_data(data, size), handler);
     * @endcode
     * See the @ref buffer documentation for information on receiving into
     * multiple buffers in one go, and how to use it with arrays, boost::array or
     * std::vector.
     */
    void async_receive(const mutable_data& buffers, async_recv_handler& handler)
    {
        
        this->async_receive(buffers, 0, handler);
    }

    /**
     * Start an asynchronous receive.
     * This function is used to asynchronously receive data from the stream
     * socket. The function call always returns immediately.
     *
     * @param buffers One or more buffers into which the data will be received.
     * Although the buffers object may be copied as necessary, ownership of the
     * underlying memory blocks is retained by the caller, which must guarantee
     * that they remain valid until the handler is called.
     *
     * @param flags Flags specifying how the receive call is to be made.
     *
     * @param handler The handler to be called when the receive operation
     * completes. Copies will be made of the handler as required. The function
     * signature of the handler must be:
     * @code void handler(
     *   const std::error_code& error,     // Result of operation.
     *   std::size_t bytes_transferred     // Number of bytes received.
     * ); @endcode
     *
     * @note The receive operation may not receive all of the requested number of
     * bytes. Consider using the @ref async_read function if you need to ensure
     * that the requested amount of data is received before the asynchronous
     * operation completes.
     *
     * @par Example
     * To receive into a single data buffer use the @ref buffer function as
     * follows:
     * @code
     * socket.async_receive(mutable_data(data, size), 0, handler);
     * @endcode
     * See the @ref buffer documentation for information on receiving into
     * multiple buffers in one go, and how to use it with arrays, boost::array or
     * std::vector.
     */
    void async_receive(const mutable_data& buffers, socket_base::message_flags flags, async_recv_handler& handler)
    {
        if ((_state & socket_ops::stream_oriented))
        {
            _handlerAsyncRecv = [=]()
            {
                std::error_code ec;
                if (getConnectionState(ec) == connection_state::Connected)
                {
                    if (this->available(ec) > 0)
                    {
                        std::size_t readBytes = this->receive(buffers, flags, ec);
                        handler(ec, readBytes);
                    }
                    else if (ec)
                    {
                        handler(ec, 0);
                    }
                }
                else if (ec)
                {
                    handler(ec, 0);
                }
            }
        }
        else
        {
            std::error_code ec = make_error_code(std::errc::address_family_not_supported);
            throw_error_if(ec, "saync_send");
        }
    }

    /**
     * Start an asynchronous send.
     * This function is used to asynchronously send a datagram to the specified
     * remote endpoint. The function call always returns immediately.
     *
     * @param buffers One or more data buffers to be sent to the remote endpoint.
     * Although the buffers object may be copied as necessary, ownership of the
     * underlying memory blocks is retained by the caller, which must guarantee
     * that they remain valid until the handler is called.
     *
     * @param destination The remote endpoint to which the data will be sent.
     * Copies will be made of the endpoint as required.
     *
     * @param handler The handler to be called when the send operation completes.
     * Copies will be made of the handler as required. The function signature of
     * the handler must be:
     * @code void handler(
     *   const std::error_code& error, // Result of operation.
     *   std::size_t bytes_transferred  // Number of bytes sent.
     * );
     * @endcode
     *
     * @par Example
     * To send a single data buffer use the @ref buffer function as follows:
     * @code
     * ip::udp::endpoint destination(
     *     ip::address::from_string("1.2.3.4"), 12345);
     * socket.async_send_to(
     *     mutable_data(data, size), destination, handler);
     * @endcode
     * See the @ref buffer documentation for information on sending multiple
     * buffers in one go, and how to use it with arrays, std::array or
     * std::vector.
     */
    void async_send_to(const mutable_data& buffers
        , const endpoint_type& destination
        , async_send_handler& handler)
    {
        this->async_send_to(buffers, destination, 0, handler);
    }


    /**
     * Start an asynchronous send.
     * This function is used to asynchronously send a datagram to the specified
     * remote endpoint. The function call always returns immediately.
     *
     * @param buffers One or more data buffers to be sent to the remote endpoint.
     * Although the buffers object may be copied as necessary, ownership of the
     * underlying memory blocks is retained by the caller, which must guarantee
     * that they remain valid until the handler is called.
     *
     * @param flags Flags specifying how the send call is to be made.
     *
     * @param destination The remote endpoint to which the data will be sent.
     * Copies will be made of the endpoint as required.
     *
     * @param handler The handler to be called when the send operation completes.
     * Copies will be made of the handler as required. The function signature of
     * the handler must be:
     * @code void handler(
     *   const std::error_code& error,  // Result of operation.
     *   std::size_t bytes_transferred  // Number of bytes sent.
     * );
     * @endcode
     */
    void async_send_to(const mutable_data& buffers
        , const endpoint_type& destination
        , socket_base::message_flags flags
        , async_send_handler& handler)
    {
        if ((_state & socket_ops::datagram_oriented))
        {
            LOG_ERROR("***** Not Implementation.");
        }
        else
        {
            std::error_code ec = make_error_code(std::errc::address_family_not_supported);
            throw_error_if(ec, "async_send_to");
        }
    }

    /**
     * Start an asynchronous receive.
     * This function is used to asynchronously receive a datagram. The function
     * call always returns immediately.
     *
     * @param buffers One or more buffers into which the data will be received.
     * Although the buffers object may be copied as necessary, ownership of the
     * underlying memory blocks is retained by the caller, which must guarantee
     * that they remain valid until the handler is called.
     *
     * @param sender_endpoint An endpoint object that receives the endpoint of
     * the remote sender of the datagram. Ownership of the sender_endpoint object
     * is retained by the caller, which must guarantee that it is valid until the
     * handler is called.
     *
     * @param handler The handler to be called when the receive operation
     * completes. Copies will be made of the handler as required. The function
     * signature of the handler must be:
     * @code void handler(
     *   const std::error_code& error, // Result of operation.
     *   std::size_t bytes_transferred           // Number of bytes received.
     * ); @endcode
     *
     * @par Example
     * To receive into a single data buffer use the @ref buffer function as
     * follows:
     * @code socket.async_receive_from(
     *     mutable_data(data, size), sender_endpoint, handler);
     * @endcode
     * See the @ref buffer documentation for information on receiving into
     * multiple buffers in one go, and how to use it with arrays, std::array or
     * std::vector.
     */
    void async_receive_from(const mutable_data& buffers
        , endpoint_type& sender_endpoint
        , async_recv_handler& handler)
    {
        this->async_receive_from(buffers, sender_endpoint, 0, handler);
    }

    /**
     * Start an asynchronous receive.
     * This function is used to asynchronously receive a datagram. The function
     * call always returns immediately.
     *
     * @param buffers One or more buffers into which the data will be received.
     * Although the buffers object may be copied as necessary, ownership of the
     * underlying memory blocks is retained by the caller, which must guarantee
     * that they remain valid until the handler is called.
     *
     * @param sender_endpoint An endpoint object that receives the endpoint of
     * the remote sender of the datagram. Ownership of the sender_endpoint object
     * is retained by the caller, which must guarantee that it is valid until the
     * handler is called.
     *
     * @param flags Flags specifying how the receive call is to be made.
     *
     * @param handler The handler to be called when the receive operation
     * completes. Copies will be made of the handler as required. The function
     * signature of the handler must be:
     * @code void handler(
     *   const std::error_code& error, // Result of operation.
     *   std::size_t bytes_transferred // Number of bytes received.
     * ); @endcode
     */
    void async_receive_from(const mutable_data& buffers
        , endpoint_type& sender_endpoint
        , socket_base::message_flags flags
        , async_recv_handler& handler)
    {
        if ((_state & socket_ops::datagram_oriented))
        {
            LOG_ERROR("***** Not Implementation.");
        }
        else
        {
            std::error_code ec = make_error_code(std::errc::address_family_not_supported);
            throw_error_if(ec, "async_receive_from");
        }
        
    }

    /**
     * Start an asynchronous accept.
     * This function is used to asynchronously accept a new connection. The
     * function call always returns immediately.
     *
     * This overload requires that the Protocol template parameter satisfy the
     * AcceptableProtocol type requirements.
     *
     * @param peer_endpoint An endpoint object into which the endpoint of the
     * remote peer will be written. Ownership of the peer_endpoint object is
     * retained by the caller, which must guarantee that it is valid until the
     * handler is called.
     *
     * @param handler The handler to be called when the accept operation
     * completes. Copies will be made of the handler as required. The function
     * signature of the handler must be:
     * @code void handler(
     *   const std::error_code& error, // Result of operation.
     *   typename Protocol::socket peer // On success, the newly accepted socket.
     * ); @endcode
     *
     * @par Example
     * @code
     * void accept_handler(const std::error_code& error,network::ip::tcp::socket peer)
     * {
     *   if (!error)
     *   {
     *     // Accept succeeded.
     *   }
     * }
     *
     * ...
     * network::ip::tcp::endpoint endpoint;
     * ip::tcp::socket socket.accept(endpoint, accept_handler);
     * @endcode
     */
    void async_accept(endpoint_type& peer_endpoint, async_accept_handler& handler)
    {
        _handlerAsyncAccept = [=]()
        {
            std::error_code ec;
           // async_accept_handler
            if (has_pending_accept(ec))
            {
                auto new_socket = accept(peer_endpoint, ec);
                handler(ec, new_socket);
            }
            else if (ec)
            {
                typename Protocol::socket new_socket(invalid_socket);
                handler(ec, new_socket);
            }
        }
    }

    /**
     * Asynchronously wait for the socket to become ready to read, ready to
     * write, or to have pending error conditions.
     *
     * This function is used to perform an asynchronous wait for a socket to enter
     * a ready to read, write or error condition state.
     *
     * @param w Specifies the desired socket state.
     *
     * @param handler The handler to be called when the wait operation completes.
     * Copies will be made of the handler as required. The function signature of
     * the handler must be:
     * @code void handler(
     *   const std::error_code& error // Result of operation
     * ); @endcode
     *
     * @par Example
     * @code
     * void wait_handler(const std::error_code& error)
     * {
     *   if (!error)
     *   {
     *     // Wait succeeded.
     *   }
     * }
     *
     * ...
     *
     * ip::tcp::socket socket();
     * ...
     * socket.async_wait(ip::tcp::socket::wait_read, wait_handler);
     * @endcode
     */
    void async_wait(wait_type w, std::function<void(std::error_code)>& handler)
    {
        LOG_ERROR("***** Not Implementation.");
    }

    explicit operator bool() const
    {
        return (_native_socket != invalid_socket);
    }
private:

    /// Holds the BSD socket object. */
    native_handle_type      _native_socket;

    /// The socket state type
    socket_ops::state_type  _state;

    /// The ip address type (ipv4/ipv6).
    Protocol                _protocol;

    /// The socket is open ?
    bool                    _open;

    handler_function        _handlerAsyncAccept;
    handler_function        _handlerAsyncConnect;
    handler_function        _handlerAsyncRecv;
    handler_function        _handlerAsyncSend;
};

} // namespace network
NS_FK_END


#endif // LOSEMYMIND_ASYNC_SOCKET_HPP




