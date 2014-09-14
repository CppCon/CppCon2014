//
//  Po7_in.cpp
//  PlusPlus
//
//  Created by Lisa Lippincott on 8/31/14.
//  Released into the public domain by Lisa Lippincott, 2014.
//

#include "Po7_in.h"
#include "Po7_inet.h"

#include <istream>
#include <ostream>
#include <string>
#include <arpa/inet.h>

// in_port_t

auto Po7::MakeAnything( ThingToMake< in_port_t >, in_port_number number ) -> in_port_t
   {
    return Wrap< in_port_t >( htons( number ) );
   }

auto Po7::MakeAnything( ThingToMake< in_port_number >, in_port_t port ) -> in_port_number
   {
    return ntohs( Unwrap( port ) );
   }

auto Po7::MakeAnything( ThingToMake< in_port_t >, const std::string& s ) -> in_port_t
   {
    unsigned long number = std::stoul( s );
    
    if ( number > std::numeric_limits< in_port_number >::max() )
        throw std::range_error( "Port number too large" );
    
    return Make<in_port_t>( static_cast< in_port_number >( number ) );
   }

auto Po7::MakeAnything( ThingToMake< std::string >, in_port_t port ) -> std::string
   {
    return std::to_string( Make< in_port_number >( port ) );
   }

std::ostream& Po7::operator<<( std::ostream& stream, in_port_t port )
   {
    return stream << Make<std::string>( port );
   }

std::istream& Po7::operator>>( std::istream& stream, in_port_t& port )
   {
    std::string s;
    stream >> s;
    if ( stream )
        port = Make< in_port_t >( s );
    return stream;
   }




// in_addr_t

auto Po7::MakeAnything( ThingToMake< in_addr_t >, in_addr_number number ) -> in_addr_t
   {
    return Wrap< in_addr_t >( htonl( number ) );
   }

auto Po7::MakeAnything( ThingToMake< in_addr_number >, in_addr_t address ) -> in_addr_number
   {
    return ntohl( Unwrap( address ) );
   }

auto Po7::MakeAnything( ThingToMake< in_addr_t >, const std::string& s ) -> in_addr_t
   {
    return inet_pton< af_inet >( s.c_str() );
   }

auto Po7::MakeAnything( ThingToMake< std::string >, in_addr_t address ) -> std::string
   {
    char text[ inet_addr_string_length< af_inet >::value ];
    inet_ntop< af_inet >( address, text );
    return std::string( text );
   }

std::ostream& Po7::operator<<( std::ostream& stream, in_addr_t address )
   {
    return stream << Make<std::string>( address );
   }

std::istream& Po7::operator>>( std::istream& stream, in_addr_t& address )
   {
    std::string s;
    stream >> s;
    if ( stream )
        address = Make< in_addr_t >( s );
    return stream;
   }



// in_addr

auto Po7::MakeAnything( ThingToMake< in_addr >, in_addr_t address ) -> in_addr
   {
    in_addr result;
    memset( &result, 0, sizeof( result ) );

    result.s_addr = Unwrap( address );

    return result;
   }

auto Po7::MakeAnything( ThingToMake< in_addr >, const std::string& s ) -> in_addr
   {
    return Make< in_addr >( Make< in_addr_t >( s ) );
   }

auto Po7::MakeAnything( ThingToMake< std::string >, const in_addr& address ) -> std::string
   {
    return Make< std::string >( Wrap< in_addr_t >( address.s_addr ) );
   }



// sockaddr_in

sockaddr_in Po7::MakeAnything( ThingToMake< sockaddr_in >, const in_addr& address, in_port_t port )
   {
    sockaddr_in result;
    memset( &result, 0, sizeof( result ) );
    
    result.sin_addr = address;
    result.sin_port = Unwrap( port );
    
    return result;
   }

sockaddr_in Po7::MakeAnything( ThingToMake< sockaddr_in >, in_addr_t address, in_port_t port )
   {
    return Make< sockaddr_in >( Make< in_addr >( address ), port );
   }

sockaddr_in Po7::MakeAnything( ThingToMake< sockaddr_in >, const std::string& text )
   {
    std::string::const_iterator afterLastColon = std::find( text.rbegin(), text.rend(), ':' ).base();
    if ( afterLastColon == text.begin() )
        throw std::domain_error( "Invalid IP4 socket address" );
    
    return Make< sockaddr_in >( Make< in_addr_t >( std::string( text.begin(),   afterLastColon - 1 ) ),
                                Make< in_port_t >( std::string( afterLastColon, text.end() ) ) );
   }

std::string Po7::MakeAnything( ThingToMake< std::string >, const sockaddr_in& address )
   {
    return Make< std::string >( address.sin_addr )
           + ":"
           + Make< std::string >( Wrap< in_port_t >( address.sin_port ) );
   }



// in6_addr

in6_addr Po7::MakeAnything( ThingToMake< in6_addr >, const std::string& s )
   {
    return inet_pton< af_inet6 >( s.c_str() );
   }

std::string Po7::MakeAnything( ThingToMake< std::string >, const ::in6_addr& address )
   {
    char text[ inet_addr_string_length< af_inet6 >::value ];
    inet_ntop< af_inet6 >( address, text );
    return std::string( text );
   }



// sockaddr_in6

sockaddr_in6 Po7::MakeAnything( ThingToMake< sockaddr_in6 >, const in6_addr& address, in_port_t port )
   {
    sockaddr_in6 result;
    memset( &result, 0, sizeof( result ) );
    
    result.sin6_addr = address;
    result.sin6_port = Unwrap( port );
    
    return result;
   }

sockaddr_in6 Po7::MakeAnything( ThingToMake< sockaddr_in6 >, const std::string& text )
   {
    std::string::const_iterator afterLastColon = std::find( text.rbegin(), text.rend(), ':' ).base();
    
    if ( afterLastColon - text.begin() < 3
         || text.front() != '['
         || *(afterLastColon - 2) != ']' )
        throw std::domain_error( "Invalid IP6 socket address" );
    
    return Make< sockaddr_in6 >( Make< in6_addr  >( std::string( text.begin() + 1, afterLastColon - 2 ) ),
                                 Make< in_port_t >( std::string( afterLastColon,   text.end() ) ) );
   }

std::string Po7::MakeAnything( ThingToMake< std::string >, const sockaddr_in6& address )
   {
    return "["
           + Make< std::string >( address.sin6_addr )
           + "]:"
           + Make< std::string >( Wrap< in_port_t >( address.sin6_port ) );
   }
