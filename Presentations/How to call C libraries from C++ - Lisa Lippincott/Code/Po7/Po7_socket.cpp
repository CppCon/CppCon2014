//
//  Po7_socket.cpp
//  PlusPlus
//
//  Created by Lisa Lippincott on 8/30/14.
//  Released into the public domain by Lisa Lippincott, 2014.
//

#include "Po7_socket.h"
#include "Po7_Invoke.h"

void Po7::SocketDeleter::operator()( pointer p ) const
   {
    // Don't use Invoke in the deleter; this must ignore errors

    int socket = Unwrap( *p );
    
    if ( socket != -1 )
        ::close( socket );
   }

auto Po7::socket( socket_domain_t   domain,
                  socket_type_t     type,
                  socket_protocol_t protocol ) -> unique_socket
   {
    return Invoke( Result< unique_socket >() + FailsWhenFalse(),
                   ::socket,
                   In( domain, type, protocol ),
                   ThrowErrorFromErrno() );
   }

void Po7::close( unique_socket s )
   {
    return Invoke( FailureFlagResult<int>(),
                   ::close,
                   In( std::move( s ) ),
                   ThrowErrorFromErrno() );
   }

void Po7::listen( socket_t socket, int backlog )
   {
    return Invoke( FailureFlagResult<int>(),
                   ::listen,
                   In( socket, backlog ),
                   ThrowErrorFromErrno() );
   }

void Po7::connect( socket_t socket, const sockaddr& address, socklen_t addressLength )
   {
    return Invoke( FailureFlagResult<int>(),
                   ::connect,
                   In( socket, address, addressLength ),
                   ThrowErrorFromErrno() );
   }

void Po7::bind( socket_t socket, const sockaddr& address, socklen_t addressLength )
   {
    return Invoke( FailureFlagResult<int>(),
                   ::bind,
                   In( socket, address, addressLength ),
                   ThrowErrorFromErrno() );
   }

auto Po7::accept( socket_t socket ) -> unique_socket
   {
    return Invoke( Result<unique_socket>() + FailsWhenFalse(),
                   ::accept,
                   In( socket, nullptr, nullptr ),
                   ThrowErrorFromErrno() );
   }

auto Po7::accept( socket_t socket, sockaddr& address, socklen_t& addressLength ) -> unique_socket
   {
    return Invoke( Result<unique_socket>() + FailsWhenFalse(),
                   ::accept,
                   In( socket ),
                   InOut( address, addressLength ),
                   ThrowErrorFromErrno() );
   }

void Po7::getsockname( socket_t socket, sockaddr& address, socklen_t& addressLength )
   {
    return Invoke( FailureFlagResult<int>(),
                   ::getsockname,
                   In( socket ),
                   InOut( address, addressLength ),
                   ThrowErrorFromErrno() );
   }

void Po7::getpeername( socket_t socket, sockaddr& address, socklen_t& addressLength )
   {
    return Invoke( FailureFlagResult<int>(),
                   ::getpeername,
                   In( socket ),
                   InOut( address, addressLength ),
                   ThrowErrorFromErrno() );
   }

namespace
   {
    struct ssize_t_Result
       {
        using ResultType                                                    = ::ssize_t;
        bool CheckForFailure( ::ssize_t s ) const                           { return s == -1; }
        std::tuple<> ThrownParts( ::ssize_t ) const                         { return std::tuple<>(); }
        std::tuple< std::size_t > ReturnedParts( ::ssize_t s ) const        { return std::make_tuple( static_cast<std::size_t>( s ) ); }
       };
   }

std::size_t Po7::send( socket_t socket, const void *buffer, std::size_t length, msg_flags_t flags )
   {
    return Invoke( ssize_t_Result(),
                   ::send,
                   In( socket, buffer, length, flags ),
                   ThrowErrorFromErrno() );
   }

std::size_t Po7::recv( socket_t socket, void *buffer, std::size_t length, msg_flags_t flags )
   {
    return Invoke( ssize_t_Result(),
                   ::recv,
                   In( socket, buffer, length, flags ),
                   ThrowErrorFromErrno() );
   }

void Po7::shutdown( socket_t socket, shutdown_how_t how )
   {
    return Invoke( FailureFlagResult<int>(),
                   ::shutdown,
                   In( socket, how ),
                   ThrowErrorFromErrno() );
   }
