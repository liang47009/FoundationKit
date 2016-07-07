#ifndef LOSEMYMIND_BASIC_SOCKET_HPP
#define LOSEMYMIND_BASIC_SOCKET_HPP
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

NS_FK_BEGIN
namespace network{

template<typename Protocol>
class basic_socket : public socket_base
{
public:
    /// The protocol type.
    typedef Protocol protocol_type;

    /// The socket type
    typedef socket_type  native_handle_type;

    /// The endpoint type.
    typedef typename Protocol::endpoint endpoint_type;
public:

    basic_socket()
        : _native_socket(invalid_socket)
        , _state(0)
        , _open(false)

    {

    }

    basic_socket(native_handle_type new_socket)
        : _native_socket(new_socket)
    {

    }

    /**
    * Virtual destructor
    */
    virtual ~basic_socket()
    {
        this->shutdown(socket_base::shutdown_both);
        std::error_code ec;
        socket_ops::close(_native_socket, _state, true, ec);
        if (ec)
        {

        }
    }

    virtual void poll()
    {

    }

    /**
     * Open the socket using the specified protocol.
     * This function opens the socket so that it will use the specified protocol.
     *
     * @param protocol An object specifying protocol parameters to be used.
     *
     * @throws std::system_error Thrown on failure.
     *
     * @par Example
     * @code
     * ip::tcp::socket socket();
     * socket.open(ip::tcp::v4());
     * @endcode
     */
    void open(const protocol_type& protocol = protocol_type())
    {
        std::error_code ec;
        this->open(protocol, ec);
        throw_error_if(ec, "open");
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
    std::error_code open(const protocol_type& protocol, std::error_code& ec)
    {
        _native_socket = socket_ops::socket(protocol.family(), protocol.type(), protocol.protocol(), ec);
        switch (protocol.type())
        {
        case SOCK_STREAM: _state = socket_ops::stream_oriented; break;
        case SOCK_DGRAM: _state  = socket_ops::datagram_oriented; break;
        default: _state = 0; break;
        }
        if (!ec)
            _open = true;
        return ec;
    }

    /**
     * Assign an existing native socket to the socket.
     * This function opens the socket to hold an existing native socket.
     *
     * @param protocol An object specifying which protocol is to be used.
     *
     * @param native_socket A native socket.
     *
     * @throws std::system_error Thrown on failure.
     */
    void assign(const protocol_type& protocol, const native_handle_type& native_socket)
    {
        asio::error_code ec;
        this->assign(protocol, native_socket, ec);
        throw_error_if(ec, "assign");
    }

    /**
     * Assign an existing native socket to the socket.
     * This function opens the socket to hold an existing native socket.
     *
     * @param protocol An object specifying which protocol is to be used.
     *
     * @param native_socket A native socket.
     *
     * @param ec Set to indicate what error occurred, if any.
     */
    std::error_code assign(const protocol_type& protocol, const native_handle_type& native_socket, std::error_code& ec)
    {
        _native_socket = native_socket;
        switch (protocol.type())
        {
        case SOCK_STREAM:
            _state = socket_ops::stream_oriented;
            break;
        case SOCK_DGRAM:
            _state = socket_ops::datagram_oriented;
            break;
        default:
            _state = 0;
            break;
        }
        _state |= socket_ops::possible_dup;
        _protocol = protocol;
        if (!ec)
            _open = true;
        return ec;
    }

    /// Determine whether the socket is open.
    bool is_open() const
    {
        return _open;
    }

    /**
     * Connect the socket to the specified endpoint.
     * This function is used to connect a socket to the specified remote endpoint.
     * The function call will block until the connection is successfully made or
     * an error occurs.
     *
     * The socket is automatically opened if it is not already open. If the
     * connect fails, and the socket was automatically opened, the socket is
     * not returned to the closed state.
     *
     * @param peer_endpoint The remote endpoint to which the socket will be
     * connected.
     *
     * @throws std::system_error Thrown on failure.
     *
     * @par Example
     * @code
     * ip::tcp::socket socket();
     * ip::tcp::endpoint endpoint(ip::address::from_string("1.2.3.4"), 12345);
     * socket.connect(endpoint);
     * @endcode
     */
    void connect(const endpoint_type& peer_endpoint)
    {
        std::error_code ec;
        this->connect(peer_endpoint, ec);
        throw_error_if(ec, "connect");
    }

    /**
     * Connect the socket to the specified endpoint.
     * This function is used to connect a socket to the specified remote endpoint.
     * The function call will block until the connection is successfully made or
     * an error occurs.
     *
     * The socket is automatically opened if it is not already open. If the
     * connect fails, and the socket was automatically opened, the socket is
     * not returned to the closed state.
     *
     * @param peer_endpoint The remote endpoint to which the socket will be
     * connected.
     *
     * @param ec Set to indicate what error occurred, if any.
     *
     * @par Example
     * @code
     * ip::tcp::socket socket();
     * ip::tcp::endpoint endpoint(ip::address::from_string("1.2.3.4"), 12345);
     * std::error_code ec;
     * socket.connect(endpoint, ec);
     * if (ec)
     * {
     *   // An error occurred.
     * }
     * @endcode
     */
    std::error_code connect(const endpoint_type& peer_endpoint, std::error_code& ec)
    {
        if (!this->is_open())
        {
            this->open(peer_endpoint.protocol(), ec);
            throw_error_if(ec, "connect");
        }
        socket_ops::sync_connect(_native_socket, peer_endpoint.data(), peer_endpoint.size(), ec);
        return ec;
    }

    /**
     * Bind the socket to the given local endpoint.
     * This function binds the socket to the specified endpoint on the local
     * machine.
     *
     * @param endpoint An endpoint on the local machine to which the socket will
     * be bound.
     *
     * @throws std::system_error Thrown on failure.
     *
     * @par Example
     * @code
     * ip::tcp::socket socket();
     * socket.open(ip::tcp::v4());
     * socket.bind(ip::tcp::endpoint(ip::tcp::v4(), 12345));
     * @endcode
     */
    void bind(const endpoint_type& endpoint)
    {
        std::error_code ec;
        this->bind(ec);
        throw_error_if(ec, "bind");
    }

    /**
     * Bind the socket to the given local endpoint.
     * This function binds the socket to the specified endpoint on the local
     * machine.
     *
     * @param endpoint An endpoint on the local machine to which the socket will
     * be bound.
     *
     * @param ec Set to indicate what error occurred, if any.
     *
     * @par Example
     * @code
     * ip::tcp::socket socket();
     * socket.open(ip::tcp::v4());
     * std::error_code ec;
     * socket.bind(ip::tcp::endpoint(ip::tcp::v4(), 12345), ec);
     * if (ec)
     * {
     *   // An error occurred.
     * }
     * @endcode
     */
    std::error_code bind(const endpoint_type& endpoint, std::error_code& ec)
    {
        socket_ops::bind(_native_socket, peer_endpoint.data(), peer_endpoint.size(), ec);
        return ec;
    }


    /**
     * Send some data on the socket.
     * This function is used to send data on the socket. The function
     * call will block until one or more bytes of the data has been sent
     * successfully, or an until error occurs.
     *
     * @param buffers One or more data buffers to be sent on the socket.
     *
     * @param flags Flags specifying how the send call is to be made.
     *
     * @returns The number of bytes sent.
     *
     * @throws std::system_error Thrown on failure.
     *
     * @note The send operation may not transmit all of the data to the peer.
     * Consider using the @ref write function if you need to ensure that all data
     * is written before the blocking operation completes.
     *
     * @par Example
     * To send a single data buffer use the @ref buffer function as follows:
     * @code
     * socket.send(mutable_data(data, size), 0);
     * @endcode
     * See the @ref buffer documentation for information on sending multiple
     * data in one go, and how to use it with arrays ,std::string or
     * std::vector.
     */
    std::size_t send(const mutable_data& buffers, socket_base::message_flags flags = 0)
    {
        std::error_code ec;
        std::size_t len = this->send(buffers, flags, ec);
        throw_error_if(ec, "send");
        return len;
    }

    /**
     * Send some data on the socket.
     * This function is used to send data on the stream socket. The function
     * call will block until one or more bytes of the data has been sent
     * successfully, or an until error occurs.
     *
     * @param buffers One or more data buffers to be sent on the socket.
     *
     * @param flags Flags specifying how the send call is to be made.
     *
     * @param ec Set to indicate what error occurred, if any.
     *
     * @returns The number of bytes sent. Returns 0 if an error occurred.
     *
     * @note The send operation may not transmit all of the data to the peer.
     * Consider using the @ref write function if you need to ensure that all data
     * is written before the blocking operation completes.
     */
    std::size_t send(const mutable_data& buffers, socket_base::message_flags flags, std::error_code& ec)
    {
        if ((_state & socket_ops::stream_oriented))
        {
            const char* buffer = const_cast<mutable_data&>(buffers).c_str();
            return socket_ops::send(_native_socket
                , buffer
                , buffers.size()
                , flags
                , ec);
        }
        else
        {
            std::error_code ec = make_error_code(std::errc::address_family_not_supported);
            throw_error_if(ec, "send");
        }
        return 0;
    }

    /**
     * Receive some data on the socket.
     * This function is used to receive data on the stream socket. The function
     * call will block until one or more bytes of data has been received
     * successfully, or until an error occurs.
     *
     * @param buffers One or more buffers into which the data will be received.
     *
     * @param flags Flags specifying how the receive call is to be made,
     *              Default is 0.
     *
     * @returns The number of bytes received.
     *
     * @throws std::system_error Thrown on failure. An error code of
     * std::error::eof indicates that the connection was closed by the
     * peer.
     *
     * @note The receive operation may not receive all of the requested number of
     * bytes. Consider using the @ref read function if you need to ensure that the
     * requested amount of data is read before the blocking operation completes.
     *
     * @par Example
     * To receive into a single data buffer use the @ref buffer function as
     * follows:
     * @code
     * socket.receive(mutable_data(data, size), 0);
     * @endcode
     * See the @ref buffer documentation for information on receiving into
     * multiple buffers in one go, and how to use it with arrays, boost::array or
     * std::vector.
     */
    std::size_t receive(const mutable_data& buffers, socket_base::message_flags flags = 0)
    {
        std::error_code ec;
        std::size_t result = this->receive(buffers, flags, ec);
        throw_error_if(ec, "receive");
        return result;
    }

    /**
     * Receive some data on a connected socket.
     * This function is used to receive data on the stream socket. The function
     * call will block until one or more bytes of data has been received
     * successfully, or until an error occurs.
     *
     * @param buffers One or more buffers into which the data will be received.
     *
     * @param flags Flags specifying how the receive call is to be made.
     *
     * @param ec Set to indicate what error occurred, if any.
     *
     * @returns The number of bytes received. Returns 0 if an error occurred.
     *
     * @note The receive operation may not receive all of the requested number of
     * bytes. Consider using the @ref read function if you need to ensure that the
     * requested amount of data is read before the blocking operation completes.
     */
    std::size_t receive(const mutable_data& buffers, socket_base::message_flags flags, std::error_code& ec)
    {
        if ((_state & socket_ops::stream_oriented))
        {
            return socket_ops::recv(_native_socket, static_cast<char*>(buffers.data()), buffers.size(), flags, ec);
        }
        else
        {
            std::error_code ec = make_error_code(std::errc::address_family_not_supported);
            throw_error_if(ec, "receive address family not suported.");
        }
        return 0;
    }

    /**
     * Send a datagram to the specified endpoint.
     * This function is used to send a datagram to the specified remote endpoint.
     * The function call will block until the data has been sent successfully or
     * an error occurs.
     *
     * @param buffers One or more data buffers to be sent to the remote endpoint.
     *
     * @param destination The remote endpoint to which the data will be sent.
     *
     * @param flags Flags specifying how the send call is to be made.
     *
     * @returns The number of bytes sent.
     *
     * @throws std::system_error Thrown on failure.
     */
    std::size_t send_to(const mutable_data& buffers
        , const endpoint_type& destination
        , socket_base::message_flags flags = 0)
    {
        std::error_code ec;
        std::size_t result = this->send_to(buffers, destination, flags, ec);
        throw_error_if(ec, "send_to");
        return result;
    }

    /**
     * Send a datagram to the specified endpoint.
     * This function is used to send a datagram to the specified remote endpoint.
     * The function call will block until the data has been sent successfully or
     * an error occurs.
     *
     * @param buffers One or more data buffers to be sent to the remote endpoint.
     *
     * @param destination The remote endpoint to which the data will be sent.
     *
     * @param flags Flags specifying how the send call is to be made.
     *
     * @param ec Set to indicate what error occurred, if any.
     *
     * @returns The number of bytes sent.
     */
    std::size_t send_to(const mutable_data& buffers
        , const endpoint_type& destination
        , socket_base::message_flags flags
        , std::error_code& ec)
    {

        if ((_state & socket_ops::datagram_oriented))
        {
            return socket_ops::sendto(_native_socket
                , buffers.c_str()
                , buffers.size()
                , flags
                , destination.data()
                , destination.size()
                , ec);
        }
        else
        {
            std::error_code ec = make_error_code(std::errc::address_family_not_supported);
            throw_error_if(ec, "send_to");
        }
        return 0;
    }

    /**
     * Receive a datagram with the endpoint of the sender.
     * This function is used to receive a datagram. The function call will block
     * until data has been received successfully or an error occurs.
     *
     * @param buffers One or more buffers into which the data will be received.
     *
     * @param sender_endpoint An endpoint object that receives the endpoint of
     * the remote sender of the datagram.
     *
     * @param flags Flags specifying how the receive call is to be made.
     *              Default is 0.
     * @returns The number of bytes received.
     */
    std::size_t receive_from(const mutable_data& buffers
        , endpoint_type& sender_endpoint
        , socket_base::message_flags flags = 0)
    {
        std::error_code ec;
        std::size_t result = this->receive_from(buffers, sender_endpoint, flags, ec);
        throw_error_if(ec, "receive_from");
        result;
    }

    /**
     * Receive a datagram with the endpoint of the sender.
     * This function is used to receive a datagram. The function call will block
     * until data has been received successfully or an error occurs.
     *
     * @param buffers One or more buffers into which the data will be received.
     *
     * @param sender_endpoint An endpoint object that receives the endpoint of
     * the remote sender of the datagram.
     *
     * @param flags Flags specifying how the receive call is to be made.
     *              Default is 0.
     *
     * @param ec Set to indicate what error occurred, if any.
     *
     * @returns The number of bytes received.
     */
    std::size_t receive_from(const mutable_data& buffers
        , endpoint_type& sender_endpoint
        , socket_base::message_flags flags
        , std::error_code& ec)
    {


        if ((_state & socket_ops::datagram_oriented))
        {
            return socket_ops::recvfrom(_native_socket
                , static_cast<char*>(buffers.data())
                , buffers.size()
                , flags
                , sender_endpoint.data()
                , sender_endpoint.size()
                , ec);
        }
        else
        {
            std::error_code ec = make_error_code(std::errc::address_family_not_supported);
            throw_error_if(ec, "receive_from");
        }
        return 0;
    }

    /**
     * This function puts the socket acceptor into the state where it may accept
     * new connections.
     *
     * @param backlog The maximum length of the queue of pending connections.
     *
     * @throws std::system_error Thrown on failure.
     */
    void listen(int backlog = socket_base::max_connections)
    {
        std::error_code ec;
        this->listen(backlog, ec);
        throw_error_if(ec, "listen");
    }


    /**
     * This function puts the socket acceptor into the state where it may accept
     * new connections.
     *
     * @param backlog The maximum length of the queue of pending connections.
     *
     * @param ec Set to indicate what error occurred, if any.
     *
     * @par Example
     * @code
     * std::error_code ec;
     * socket.listen(network::socket_base::max_connections, ec);
     * if (ec)
     * {
     *   // An error occurred.
     * }
     * @endcode
     */
    std::error_code listen(int backlog, std::error_code& ec)
    {
        return socket_ops::listen(_native_socket, backlog, ec);
    }

    /**
     * Accept a new connection.
     * This function is used to accept a new connection from a peer. The function
     * call will block until a new connection has been accepted successfully or
     * an error occurs.
     *
     * This overload requires that the Protocol template parameter satisfy the
     * AcceptableProtocol type requirements.
     *
     * @param peer_endpoint An endpoint object into which the endpoint of the
     * remote peer will be written.
     *
     * @returns A socket object representing the newly accepted connection.
     *
     * @throws std::system_error Thrown on failure.
     *
     * @par Example
     * @code
     * ...
     * ip::tcp::endpoint endpoint;
     * ip::tcp::socket socket.accept(endpoint);
     * @endcode
     */
    typename Protocol::socket accept(endpoint_type& peer_endpoint)
    {
        std::error_code ec;
        typename Protocol::socket new_socket = this->accept(peer_endpoint, ec);
        throw_error_if(ec, "accept");
        return new_socket;
    }

    /**
     * Accept a new connection.
     * This function is used to accept a new connection from a peer. The function
     * call will block until a new connection has been accepted successfully or
     * an error occurs.
     *
     * This overload requires that the Protocol template parameter satisfy the
     * AcceptableProtocol type requirements.
     *
     * @param peer_endpoint An endpoint object into which the endpoint of the
     * remote peer will be written.
     *
     * @param ec Set to indicate what error occurred, if any.
     *
     * @returns On success, a socket object representing the newly accepted
     * connection. On error, a socket object where is_open() is false.
     *
     * @par Example
     * @code
     * ...
     * ip::tcp::endpoint endpoint;
     * ip::tcp::socket socket.accept(endpoint, ec);
     * if (ec)
     * {
     *   // An error occurred.
     * }
     * @endcode
     */
    typename Protocol::socket accept(endpoint_type& peer_endpoint, std::error_code& ec)
    {
        native_handle_type new_native_socket = socket_ops::accept(_native_socket
            , peer_endpoint.data()
            , peer_endpoint.size()
            , ec);
        typename Protocol::socket new_socket(new_native_socket);
        return new_socket;
    }

    /**
     * Determine the number of bytes available for reading.
     * This function is used to determine the number of bytes that may be read
     * without blocking.
     *
     * @return The number of bytes that may be read without blocking, or 0 if an
     * error occurs.
     *
     * @throws std::system_error Thrown on failure.
     */
    std::size_t available() const
    {
        std::error_code ec;
        std::size_t result = this->available(ec);
        throw_error_if(ec, "available");
        return result;
    }

    /**
     * Determine the number of bytes available for reading.
     * This function is used to determine the number of bytes that may be read
     * without blocking.
     *
     * @param ec Set to indicate what error occurred, if any.
     *
     * @return The number of bytes that may be read without blocking, or 0 if an
     * error occurs.
     */
    std::size_t available(std::error_code& ec) const
    {
        return socket_ops::available(_native_socket, ec);
    }

    /**
     * Wait for the socket to become ready to read, ready to write, or to have
     * pending error conditions.
     * This function is used to perform a blocking wait for a socket to enter
     * a ready to read, write or error condition state.
     *
     * @param w Specifies the desired socket state.
     *
     * @par Example
     * Waiting for a socket to become readable.
     * @code
     * ip::tcp::socket socket();
     * ...
     * socket.wait(ip::tcp::socket::wait_read);
     * @endcode
     */
    void wait(wait_type w)
    {
        std::error_code ec;
        this->wait(w, ec);
        throw_error_if(ec, "wait");
    }

    /**
     * Wait for the socket to become ready to read, ready to write, or to have
     * pending error conditions.
     * This function is used to perform a blocking wait for a socket to enter
     * a ready to read, write or error condition state.
     *
     * @param w Specifies the desired socket state.
     *
     * @param ec Set to indicate what error occurred, if any.
     *
     * @par Example
     * Waiting for a socket to become readable.
     * @code
     * ip::tcp::socket socket();
     * ...
     * std::error_code ec;
     * socket.wait(ip::tcp::socket::wait_read, ec);
     * @endcode
     */
    std::error_code wait(wait_type w, std::error_code& ec)
    {
        switch (w)
        {
        case socket_base::wait_read:
            socket_ops::poll_read(_native_socket, _state, ec);
            break;
        case socket_base::wait_write:
            socket_ops::poll_write(_native_socket, _state, ec);
            break;
        case socket_base::wait_error:
            socket_ops::poll_error(_native_socket, _state, ec);
            break;
        default:
            ec = asio::error::invalid_argument;
            break;
        }
        return ec;
    }

    /**
     * Disable sends or receives on the socket.
     * This function is used to disable send operations, receive operations, or
     * both.
     *
     * @param what Determines what types of operation will no longer be allowed.
     * @throws std::system_error Thrown on failure.
     *
     * @par Example
     * Shutting down the send side of the socket:
     * @code
     * ip::tcp::socket socket();
     * ...
     * socket.shutdown(ip::tcp::socket::shutdown_send);
     * @endcode
     */
    void shutdown(shutdown_type what)
    {
        std::error_code ec;
        this->shutdown(what, ec);
        throw_error_if(ec, "shutdown");
    }

    /**
     * Disable sends or receives on the socket.
     * This function is used to disable send operations, receive operations, or
     * both.
     *
     * @param what Determines what types of operation will no longer be allowed.
     *
     * @param ec Set to indicate what error occurred, if any.
     *
     * @par Example
     * Shutting down the send side of the socket:
     * @code
     * ip::tcp::socket socket();
     * ...
     * std::error_code ec;
     * socket.shutdown(ip::tcp::socket::shutdown_send, ec);
     * if (ec)
     * {
     *   // An error occurred.
     * }
     * @endcode
     */
    std::error_code shutdown(shutdown_type what, std::error_code& ec)
    {
        if (_native_socket != invalid_socket)
        {
            socket_ops::shutdown(_native_socket, what, ec);
            return ec;
        }
        return (ec = make_error_code(std::errc::bad_file_descriptor));
    }

    /**
     * Close the socket.
     * This function is used to close the socket. Any asynchronous send, receive
     * or connect operations will be cancelled immediately, and will complete
     * with the std::errc::operation_canceled error.
     *
     * @throws std::system_error Thrown on failure. Note that, even if
     * the function indicates an error, the underlying descriptor is closed.
     *
     * @note For portable behaviour with respect to graceful closure of a
     * connected socket, call shutdown() before closing the socket.
     */
    void close()
    {
        std::error_code ec;
        this->close(ec);
        throw_error_if(ec, "close");
    }

    /**
     * Close the socket.
     * This function is used to close the socket. Any asynchronous send, receive
     * or connect operations will be cancelled immediately, and will complete
     * with the std::errc::operation_aborted error.
     *
     * @param ec Set to indicate what error occurred, if any. Note that, even if
     * the function indicates an error, the underlying descriptor is closed.
     *
     * @par Example
     * @code
     * ip::tcp::socket socket(io_context);
     * ...
     * std::error_code ec;
     * socket.close(ec);
     * if (ec)
     * {
     *   // An error occurred.
     * }
     * @endcode
     *
     * @note For portable behaviour with respect to graceful closure of a
     * connected socket, call shutdown() before closing the socket.
     */
    std::error_code close(std::error_code& ec)
    {
        if (_native_socket != invalid_socket)
        {
            return socket_ops::close(_native_socket, _state, false, ec);
        }
        return (ec = make_error_code(std::errc::bad_file_descriptor));
    }

    /**
     * Get the local endpoint of the socket.
     * This function is used to obtain the locally bound endpoint of the socket.
     *
     * @returns An object that represents the local endpoint of the socket.
     *
     * @throws std::system_error Thrown on failure.
     *
     * @par Example
     * @code
     * ip::tcp::socket socket();
     * ...
     * ip::tcp::endpoint endpoint = socket.local_endpoint();
     * @endcode
     */
    endpoint_type local_endpoint() const
    {
        std::error_code ec;
        endpoint_type endpoint = this->local_endpoint(ec);
        throw_error_if(ec, "local_endpoint");
        return endpoint;
    }

    /**
     * Get the local endpoint of the socket.
     * This function is used to obtain the locally bound endpoint of the socket.
     *
     * @param ec Set to indicate what error occurred, if any.
     *
     * @returns An object that represents the local endpoint of the socket.
     * Returns a default-constructed endpoint object if an error occurred.
     *
     * @par Example
     * @code
     * ip::tcp::socket socket();
     * ...
     * std::error_code ec;
     * ip::tcp::endpoint endpoint = socket.local_endpoint(ec);
     * if (ec)
     * {
     *   // An error occurred.
     * }
     * @endcode
     */
    endpoint_type local_endpoint(std::error_code& ec) const
    {
        endpoint_type endpoint;
        std::size_t addr_len = endpoint.capacity();
        if (socket_ops::getsockname(_native_socket, endpoint.data(), &addr_len, ec))
            return endpoint_type();
        endpoint.resize(addr_len);
        return endpoint;
    }


    /**
     * Get the remote endpoint of the socket.
     * This function is used to obtain the remote endpoint of the socket.
     *
     * @returns An object that represents the remote endpoint of the socket.
     *
     * @throws std::system_error Thrown on failure.
     *
     * @par Example
     * @code
     * ip::tcp::socket socket();
     * ...
     * ip::tcp::endpoint endpoint = socket.remote_endpoint();
     * @endcode
     */
    endpoint_type remote_endpoint() const
    {
        std::error_code ec;
        endpoint_type endpoint = this->remote_endpoint(ec);
        throw_error_if(ec, "remote_endpoint");
        return endpoint;
    }

    /**
     * Get the remote endpoint of the socket.
     * Get the remote endpoint of the socket.
     * This function is used to obtain the remote endpoint of the socket.
     *
     * @param ec Set to indicate what error occurred, if any.
     *
     * @returns An object that represents the remote endpoint of the socket.
     * Returns a default-constructed endpoint object if an error occurred.
     *
     * @par Example
     * @code
     * ip::tcp::socket socket();
     * ...
     * std::error_code ec;
     * ip::tcp::endpoint endpoint = socket.remote_endpoint(ec);
     * if (ec)
     * {
     *   // An error occurred.
     * }
     * @endcode
     */
    endpoint_type remote_endpoint(std::error_code& ec) const
    {
        endpoint_type endpoint;
        std::size_t addr_len = endpoint.capacity();
        if (socket_ops::getpeername(_native_socket, endpoint.data(), &addr_len, false, ec))
            return endpoint_type();
        endpoint.resize(addr_len);
        return endpoint;
    }

    native_handle_type native_handle()
    {
        return _native_socket;
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
            state_return writeState = has_state(socket_state::writable,ec);
            if (ec) return currentState;
            state_return readState  = has_state(socket_state::readable, ec);
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
};

} // namespace network
NS_FK_END


#endif // LOSEMYMIND_BASIC_SOCKET_HPP




