//
//  Po7_socket.h
//  PlusPlus
//
//  Created by Lisa Lippincott on 8/30/14.
//  Released into the public domain by Lisa Lippincott, 2014.
//

#ifndef PO7_SOCKET_H
#define PO7_SOCKET_H

#include "Po7_Basics.h"
#include "Po7_is_sockaddr.h"

#include "bufferlike.h"

#include <type_traits>

#include <unistd.h>
#include <sys/socket.h>

namespace Po7
   {
    // socket_t represents a socket in any domain
        struct SocketTag
           {
            constexpr int operator()() const            { return -1; }
            static const bool hasEquality               = true;
            static const bool hasComparison             = true;
           };

        using socket_t = PlusPlus::Boxed< SocketTag >;

    // socket_in_domain represents a socket in a specific domain
        template < socket_domain_t >
        struct SocketInDomainTag
           {
            constexpr int operator()() const            { return -1; }
            static const bool hasEquality               = true;
            static const bool hasComparison             = true;
            constexpr operator SocketTag() const        { return SocketTag(); }
           };

        template < socket_domain_t domain >
        using socket_in_domain = PlusPlus::Boxed< SocketInDomainTag<domain> >;

        template < socket_domain_t domain >
        socket_in_domain< domain > domain_cast( socket_t s )
           {
            return Wrap< socket_in_domain< domain > >( Unwrap( s ) );
           }



    // unique_socket refers to a socket, and represents the obligation to close it
        struct SocketDeleter
           {
            using pointer = PlusPlus::PointerToValue< socket_t >;
            void operator()( pointer s ) const;
           };

        using unique_socket = std::unique_ptr< const socket_t, SocketDeleter >;

    // unique_socket_in_domain refers to a socket with a domain, and represents the obligation to close it
        template < socket_domain_t domain >
        struct SocketInDomainDeleter
           {
            using pointer = PlusPlus::PointerToValue< socket_in_domain<domain> >;
            void operator()( pointer s ) const      { SocketDeleter()( s ); }
            operator SocketDeleter() const          { return SocketDeleter(); }
           };

        template < socket_domain_t domain >
        using unique_socket_in_domain = std::unique_ptr< const socket_in_domain<domain>, SocketInDomainDeleter<domain> >;

        template < socket_domain_t domain >
        unique_socket_in_domain< domain > domain_cast( unique_socket s )
           {
            return Seize< unique_socket_in_domain< domain > >( domain_cast<domain>( Release( s ) ) );
           }

        
        
    // socket_type_t is the second paremeter to socket()
        enum class socket_type_t: int {};
        template <> struct Wrapper< socket_type_t >: PlusPlus::EnumWrapper< socket_type_t > {};

        const socket_type_t sock_stream    = socket_type_t( SOCK_STREAM );
        const socket_type_t sock_dgram     = socket_type_t( SOCK_DGRAM );
        const socket_type_t sock_seqpacket = socket_type_t( SOCK_SEQPACKET );
        #ifdef SOCK_RAW
            const socket_type_t sock_raw   = socket_type_t( SOCK_RAW );
        #endif

    // socket_protocol_t is the third paremeter to socket()
        enum class socket_protocol_t: int;
        template <> struct Wrapper< socket_protocol_t >: PlusPlus::EnumWrapper< socket_protocol_t > {};
    
    // socket() creates sockets, and close() gets rid of them.
    // Calling close allows any final errors to be thrown.
        unique_socket socket( socket_domain_t   domain,
                              socket_type_t     type,
                              socket_protocol_t protocol );
        
        void close( unique_socket s );

    // The template form of socket produces a socket typed to its domain, making address operations easier.
        template < socket_domain_t domain >
        unique_socket_in_domain< domain > socket( socket_type_t type, socket_protocol_t protocol )
           {
            return domain_cast< domain >( socket( domain, type, protocol ) );
           }
    
    

    // Listen is surprisingly simple
        void listen( socket_t, int backlog );


    // socklen_t is a perfectly cromulent integral type
        using ::socklen_t;
    
    
    // bind, connect, accept, getsockname, and getpeername are callable with a sockaddr and a length in their basic form.
        void bind( socket_t, const sockaddr&, socklen_t );
        void connect( socket_t, const sockaddr&, socklen_t );
        unique_socket accept( socket_t );
        unique_socket accept( socket_t, sockaddr&, socklen_t& );
        void getsockname( socket_t, sockaddr&, socklen_t& );
        void getpeername( socket_t, sockaddr&, socklen_t& );
    
    // bind, connect, accept, getsockname, and getpeername are also callable with domain-typed sockets and addresses.
        template < socket_domain_t domain >
        void bind( socket_in_domain<domain> s, const sockaddr_type<domain>& a )
           {
            bind( s, sockaddr_cast< const sockaddr& >( a ), sizeof( a ) );
           }

        template < socket_domain_t domain >
        void connect( socket_in_domain<domain> s, const sockaddr_type<domain>& a )
           {
            connect( s, sockaddr_cast< const sockaddr& >( a ), sizeof( a ) );
           }

        template < socket_domain_t domain >
        auto accept( socket_in_domain<domain> s )
        -> std::tuple< unique_socket_in_domain<domain>, sockaddr_type<domain> >
           {
            sockaddr_type< domain > address;
            socklen_t addressLength = sizeof( address );
            sockaddr& genericAddress = sockaddr_cast< sockaddr& >( address );
            
            unique_socket accepted  = accept( s, genericAddress, addressLength );
            
            if ( Wrap<socket_domain_t>( genericAddress.sa_family ) != domain )
                throw std::domain_error( "Socket not in the expected domain" );
            
            return std::make_tuple( domain_cast<domain>( std::move( accepted ) ), address );
           }
        
        template < socket_domain_t domain >
        auto getsockname( socket_in_domain<domain> s )
        -> sockaddr_type< domain >
           {
            sockaddr_type< domain > address;
            socklen_t addressLength = sizeof( address );
            sockaddr& genericAddress = sockaddr_cast< sockaddr& >( address );

            getsockname( s, genericAddress, addressLength );

            if ( Wrap<socket_domain_t>( genericAddress.sa_family ) != domain )
                throw std::domain_error( "Socket not in the expected domain" );
            
            return address;
           }
        
        template < socket_domain_t domain >
        auto getpeername( socket_in_domain<domain> s )
        -> sockaddr_type< domain >
           {
            sockaddr_type< domain > address;
            socklen_t addressLength = sizeof( address );
            sockaddr& genericAddress = sockaddr_cast< sockaddr& >( address );

            getpeername( s, genericAddress, addressLength );
            
            if ( Wrap<socket_domain_t>( genericAddress.sa_family ) != domain )
                throw std::domain_error( "Socket not in the expected domain" );
            
            return address;
           }

    // msg_flags_t is a parameter to send() and recv()
        struct MessageFlagsTag
           {
            constexpr int operator()() const                { return 0; }
            static const bool hasEquality                   = true;
            static const bool hasComparison                 = true;
            static const bool hasBitwise                    = true;
           };

        using msg_flags_t = PlusPlus::Boxed< MessageFlagsTag >;

        const msg_flags_t msg_eor      = msg_flags_t( MSG_EOR );
        const msg_flags_t msg_oob      = msg_flags_t( MSG_OOB );
        const msg_flags_t msg_peek     = msg_flags_t( MSG_PEEK );
        const msg_flags_t msg_waitall  = msg_flags_t( MSG_WAITALL );

    // send and recv send and receive the data
        std::size_t send( socket_t, const void *buffer, std::size_t length, msg_flags_t = msg_flags_t() );
        std::size_t recv( socket_t,       void *buffer, std::size_t length, msg_flags_t = msg_flags_t() );

        template < class Buffer >
        auto send( socket_t s, const Buffer& b, msg_flags_t f = msg_flags_t() )
        -> typename std::enable_if< PlusPlus::stdish::is_bufferlike<Buffer>::value, std::size_t >::type
           {
            return send( s, PlusPlus::stdish::bufferlike_data( b ), PlusPlus::stdish::bufferlike_size( b ), f );
           }
        
        template < class Buffer >
        auto recv( socket_t s, Buffer& b, msg_flags_t f = msg_flags_t() )
        -> typename std::enable_if< PlusPlus::stdish::is_bufferlike<Buffer>::value, std::size_t >::type
           {
            return recv( s, PlusPlus::stdish::bufferlike_data( b ), PlusPlus::stdish::bufferlike_size( b ), f );
           }
        


    // shutdown_how_t is a parameter to shutdown()
        enum class shutdown_how_t: int {};
        template <> struct Wrapper< shutdown_how_t >: PlusPlus::EnumWrapper< shutdown_how_t > {};

        const shutdown_how_t shut_rd   = shutdown_how_t( SHUT_RD );
        const shutdown_how_t shut_wr   = shutdown_how_t( SHUT_WR );
        const shutdown_how_t shut_rdwr = shutdown_how_t( SHUT_RDWR );

    // shutdown ends communication on a socket
        void shutdown( socket_t, shutdown_how_t );
   }

#endif
