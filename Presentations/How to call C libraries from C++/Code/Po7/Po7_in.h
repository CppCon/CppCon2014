//
//  Po7_in.h
//  PlusPlus
//
//  Created by Lisa Lippincott on 8/31/14.
//  Released into the public domain by Lisa Lippincott, 2014.
//

#ifndef PO7_IN_H
#define PO7_IN_H

#include "Po7_Basics.h"
#include "Po7_is_sockaddr.h"

#include <netinet/in.h>

namespace Po7
   {
    // in_port_t
        enum class in_port_t: ::in_port_t {};
        template <> struct Wrapper< in_port_t >: PlusPlus::EnumWrapper< in_port_t > {};
            // Note that this is in network byte order, per POSIX.
        
        using in_port_number = ::in_port_t;    // an integer type, meaning host byte order.
        
        in_port_t      MakeAnything( ThingToMake< in_port_t      >, in_port_number );       // htons
        in_port_number MakeAnything( ThingToMake< in_port_number >, in_port_t );            // ntohs

        in_port_t   MakeAnything( ThingToMake< in_port_t   >, const std::string& );
        std::string MakeAnything( ThingToMake< std::string >, in_port_t );
        
        std::ostream& operator<<( std::ostream& stream, in_port_t  p );
        std::istream& operator>>( std::istream& stream, in_port_t& p );


    // in_addr_t
        enum class in_addr_t: ::in_addr_t {};
        template <> struct Wrapper< in_addr_t >: PlusPlus::EnumWrapper< in_addr_t > {};
            // Note that this is in network byte order, per POSIX.

        using in_addr_number = ::in_addr_t;    // an integer type, meaning host byte order.

        in_addr_t      MakeAnything( ThingToMake< in_addr_t      >, in_addr_number );       // htonl
        in_addr_number MakeAnything( ThingToMake< in_addr_number >, in_addr_t );            // ntohl

        in_addr_t   MakeAnything( ThingToMake< in_addr_t   >, const std::string& );
        std::string MakeAnything( ThingToMake< std::string >, in_addr_t );
        
        std::ostream& operator<<( std::ostream& stream, in_addr_t  p );
        std::istream& operator>>( std::istream& stream, in_addr_t& p );


    // in_addr
        using ::in_addr;
        
        in_addr     MakeAnything( ThingToMake< in_addr     >, in_addr_t );
        in_addr     MakeAnything( ThingToMake< in_addr     >, const std::string& );
        std::string MakeAnything( ThingToMake< std::string >, const in_addr& );
    
    
    // sockaddr_in
        using ::sockaddr_in;
        template <> struct is_sockaddr< sockaddr_in >:      std::true_type {};
        template <> struct sockaddr_domain< sockaddr_in >:  std::integral_constant< socket_domain_t, af_inet > {};
        template <> struct sockaddr_type_trait< af_inet >   { using type = sockaddr_in; };
        
        sockaddr_in MakeAnything( ThingToMake< sockaddr_in >, in_addr_t,      in_port_t );
        sockaddr_in MakeAnything( ThingToMake< sockaddr_in >, const in_addr&, in_port_t );
    
        sockaddr_in MakeAnything( ThingToMake< sockaddr_in >, const std::string& );
        std::string MakeAnything( ThingToMake< std::string >, const sockaddr_in& );
    
    // in6_addr
        using ::in6_addr;
        
        in6_addr    MakeAnything( ThingToMake< in6_addr    >, const std::string& );
        std::string MakeAnything( ThingToMake< std::string >, const in6_addr& );
    
    
    // sockaddr_in6
        using ::sockaddr_in6;
        template <> struct is_sockaddr< sockaddr_in6 >:         std::true_type {};
        template <> struct sockaddr_domain< sockaddr_in6 >:     std::integral_constant< socket_domain_t, af_inet6 > {};
        template <> struct sockaddr_type_trait< af_inet6 >      { using type = sockaddr_in6; };

        sockaddr_in6 MakeAnything( ThingToMake< sockaddr_in6 >, const in6_addr&, in_port_t );
        // Missing here: an overload that takes flowinfo and scope_id. That's more esoteric than I feel like getting today.
        // And besides, I'm not sure anybody has figured out how to use them.
    
        sockaddr_in6 MakeAnything( ThingToMake< sockaddr_in6 >, const std::string&  );
        std::string  MakeAnything( ThingToMake< std::string  >, const sockaddr_in6& );
   }

#endif
