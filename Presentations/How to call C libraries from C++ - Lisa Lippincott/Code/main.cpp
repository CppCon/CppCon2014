//
//  main.cpp
//  PlusPlus
//
//  Created by Lisa Lippincott on 7/2/14.
//  Released into the public domain by Lisa Lippincott, 2014.
//

#include "Po7_socket.h"
#include "Po7_in.h"

#include <iostream>

#include <arpa/inet.h>

void GreetAndEcho( const std::string& greeting );
void GreetAndEchoTheHardWay( const std::string& greeting );

void GreetAndEcho( const std::string& greeting )
   {
    auto listener = Po7::socket< Po7::af_inet6 >( Po7::sock_stream, Po7::socket_protocol_t() );
    Po7::listen( *listener, 1 );
    std::cout << "Listening on " << Po7::Make< std::string >( Po7::getsockname( *listener ) )  << "\n";
    
    auto acceptedTuple     = Po7::accept( *listener );
    auto& connectedSocket  = std::get<0>( acceptedTuple );
    auto& connectedAddress = std::get<1>( acceptedTuple );
    std::cout << "Accepted a connection from " << Po7::Make< std::string >( connectedAddress ) << "\n"; 
    
    Po7::close( std::move( listener ) );
    
    Po7::send( *connectedSocket, greeting );
    
    char buffer[ 256 ];
    while ( std::size_t receivedLength = Po7::recv( *connectedSocket, buffer ) )
        std::cout.write( buffer, static_cast< std::streamsize >( receivedLength ) );
    
    Po7::close( std::move( connectedSocket ) );
   }

void GreetAndEchoTheHardWay( const std::string& greeting )
   {
    int listener = socket( AF_INET6, SOCK_STREAM, 0 );
    if ( listener == -1 )
        throw std::system_error( errno, std::system_category() );
    
    try
       {
        if ( listen( listener, 1 ) )
            throw std::system_error( errno, std::system_category() );
        
        sockaddr_in6 listeningAddress;
        socklen_t listeningAddressLength = sizeof( listeningAddress );
        if ( getsockname( listener, reinterpret_cast< sockaddr * >( &listeningAddress ), &listeningAddressLength ) )
            throw std::system_error( errno, std::system_category() );
        
        char listeningAddressString[ INET6_ADDRSTRLEN ];
        if ( !inet_ntop( AF_INET6, &listeningAddress.sin6_addr, listeningAddressString, sizeof( listeningAddressString ) ) )
            throw std::system_error( errno, std::system_category() );
        
        std::cout << "Listening on [" << listeningAddressString  << "]:" << ntohs( listeningAddress.sin6_port ) << "\n";

        sockaddr_in6 connectedAddress;
        socklen_t connectedAddressLength = sizeof( connectedAddress );
        int connected = accept( listener, reinterpret_cast< sockaddr * >( &connectedAddress ), &connectedAddressLength );
        if ( connected == -1 )
            throw std::system_error( errno, std::system_category() );
    
        try
           {
            char connectedAddressString[ INET6_ADDRSTRLEN ];
            if ( !inet_ntop( AF_INET6, &connectedAddress.sin6_addr, connectedAddressString, sizeof( connectedAddressString ) ) )
                throw std::system_error( errno, std::system_category() );

            std::cout << "Accepted a connection from [" << connectedAddressString << "]:" << ntohs( connectedAddress.sin6_port ) << "\n";

            int listenerBeingClosed = listener;
            listener = -1;
            if ( close( listenerBeingClosed ) )
                throw std::system_error( errno, std::system_category() );
            
            if ( send( connected, greeting.data(), greeting.size(), 0 ) == -1 )
                throw std::system_error( errno, std::system_category() );
            
            char buffer[ 256 ];
            
            while ( true )
               {
                ssize_t receivedLength = recv( connected, buffer, sizeof(buffer), 0 );
                if ( receivedLength == -1 )
                    throw std::system_error( errno, std::system_category() );
                
                if ( receivedLength == 0 )
                    break;

                std::cout.write( buffer, static_cast< std::streamsize >( receivedLength ) );
               }

            int connectedBeingClosed = connected;
            connected = -1;
            if ( close( connectedBeingClosed ) )
                throw std::system_error( errno, std::system_category() );
           }
        catch ( ... )
           {
            if ( connected != -1 )
                close( connected );
            throw;
           }
       }
    catch ( ... )
       {
        if ( listener != -1 )
            close( listener );
        throw;
       }
   }

static std::string CurrentExceptionString()
   {
    try { throw; }
    catch ( const std::exception& e )       { return e.what(); }
    catch ( ... )                           { return "[unknown]"; }
   }

int main()
   {
    try
       {
        GreetAndEcho( "Hello, world!\n" );
        return 0;
       }
    catch ( ... )
       {
        std::cerr << "Exiting with unknown exception: " << CurrentExceptionString() << std::endl;
        return 1;
       }
   }

