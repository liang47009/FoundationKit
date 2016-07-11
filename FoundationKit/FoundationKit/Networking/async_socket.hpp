#ifndef LOSEMYMIND_ASYNC_SOCKET_HPP
#define LOSEMYMIND_ASYNC_SOCKET_HPP
#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <string>
#include <functional>
#include <algorithm>
#include <vector>
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
    typedef std::function<bool()>                                   handle_method_type;
    typedef std::function<void(const std::error_code)>              async_connect_handler;
    typedef std::function<void(const std::error_code, std::size_t)> async_send_handler;
    typedef std::function<void(const std::error_code, std::size_t)> async_recv_handler;
    typedef std::function<void(const std::error_code, typename Protocol::socket)> async_accept_handler;

public:

    async_socket()
        : basic_socket()
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

    /**
     * Open the socket using the specified protocol.
     * This function opens the socket so that it will use the specified protocol.
     *
     * @param protocol An object specifying which protocol is to be used.
     *
     * @param ec Set to indicate what error occurred, if any.
     *
     * @par Example
     * @code
     * ip::tcp::socket socket();
     * std::error_code ec;
     * socket.open(ip::tcp::v4(), ec);
     * if (ec)
     * {
     *   // An error occurred.
     * }
     * @endcode
     */
    virtual std::error_code open(const protocol_type& protocol, std::error_code& ec)
    {
        _native_socket = socket_ops::socket(protocol.family(), protocol.type(), protocol.protocol(), ec);
        switch (protocol.type())
        {
        case SOCK_STREAM: _state = socket_ops::stream_oriented; break;
        case SOCK_DGRAM: _state = socket_ops::datagram_oriented; break;
        default: _state = 0; break;
        }
        if (_native_socket != invalid_socket && !ec)
        {
            socket_ops::set_non_blocking(_native_socket, _state, true, ec);
            _open = true;
        }
        return ec;
    }

    virtual void poll()
    {
        auto iter = _handleMethods.begin();
        for (iter; iter != _handleMethods.end(); ++iter)
        {
            bool bRemove = (*iter)();
            if (bRemove)
            {
                iter = _handleMethods.erase(iter);
            }
        }
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
        std::error_code ec;
        if (!(_state & socket_ops::non_blocking))
        {
            set_non_blocking(_native_socket, _state, true, ec);
            if (ec)
            {
                handler(ec);
                return ec;
            }
        }

        _handleMethods.push_back([=]()
        {
            std::error_code connectEc;
            bool isConnected = non_blocking_connect(_native_socket, connectEc);
            if (isConnected)
            {
                handler(connectEc);
            }
            return isConnected;
        });
        return ec;
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
            _handleMethods.push_back([=]()
            {
                std::error_code ec;
                if (getConnectionState(ec) == connection_state::Connected)
                {
                    if (this->available(ec) > 0)
                    {
                        std::size_t readBytes = this->receive(buffers, flags, ec);
                        handler(ec, readBytes);
                        return true;
                    }
                    else if (ec)
                    {
                        handler(ec, 0);
                        return true;
                    }
                }
                else if (ec)
                {
                    handler(ec, 0);
                    return true;
                }
                return false;
            });
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
        _handleMethods.push_back([=]()
        {
            std::error_code ec;
           // async_accept_handler
            if (has_pending_accept(ec))
            {
                auto new_socket = accept(peer_endpoint, ec);
                handler(ec, new_socket);
                return true;
            }
            else if (ec)
            {
                typename Protocol::socket new_socket(invalid_socket);
                handler(ec, new_socket);
                return true;
            }
            return false;
        });
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

    /**
     * Queries the socket to determine if there is a pending accept
     * @param bHasPendingConnection out parameter indicating whether a connection is pending or not
     * @return true if successful, false otherwise
     *
     * @par Example
     * @code
     * ip::tcp::socket socket();
     * ...
     * std::error_code ec;
     * socket.has_pending_accept(ec);
     * if (ec)
     * {
     *   // An error occurred.
     * }
     * @endcode
     */
    bool has_pending_accept(std::error_code& ec)
    {
        bool bHasPendingConnection = false;
        // make sure socket has no error state
        if (has_state(socket_state::haserror, ec) == state_return::No)
        {
            // get the read state
            state_return State = has_state(socket_state::readable, ec);
            // turn the result into the outputs
            bHasPendingConnection = State == state_return::Yes;
        }
        return bHasPendingConnection;
    }

    bool has_state(socket_state state, std::error_code& ec)
    {
        // Check the status of the state
        int32 SelectStatus = 0;
        switch (state)
        {
        case socket_state::readable:
            SelectStatus = poll_read(_native_socket, _state, ec);
            break;

        case socket_state::writable:
            SelectStatus = poll_write(_native_socket, _state, ec);
            break;

        case socket_state::haserror:
            SelectStatus = poll_error(_native_socket, _state, ec);
            break;
        }

        // if the select returns a positive number,
        // the socket had the state, 
        // 0 means didn't have it, 
        // and negative is API error condition (not socket's error state)
        return SelectStatus > 0 ? state_return::Yes :
            SelectStatus == 0 ? state_return::No :
            state_return::EncounteredError;
    }

    connection_state getConnectionState(std::error_code& ec)
    {
        connection_state currentState = connection_state::ConnectionError;

        // look for an existing error
        if (has_state(socket_state::haserror, ec) == state_return::No)
        {
            // get the write state
            state_return writeState = has_state(socket_state::writable, ec);
            if (ec) return currentState;
            state_return readState = has_state(socket_state::readable, ec);
            if (ec) return currentState;
            // translate yes or no (error is already set)
            if (writeState == state_return::Yes || readState == state_return::Yes)
            {
                currentState = connection_state::Connected;
            }
            else if (writeState == ESocketBSDReturn::No && readState == ESocketBSDReturn::No)
            {
                currentState = connection_state::NotConnected;
            }
        }
        return currentState;
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

    std::vector<handle_method_type> _handleMethods;
};

} // namespace network
NS_FK_END


#endif // LOSEMYMIND_ASYNC_SOCKET_HPP




