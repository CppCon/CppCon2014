//
//  Po7_is_sockaddr.h
//  PlusPlus
//
//  Created by Lisa Lippincott on 8/31/14.
//  Released into the public domain by Lisa Lippincott, 2014.
//

#ifndef PO7_IS_SOCKADDR_H
#define PO7_IS_SOCKADDR_H

#include <sys/socket.h>

#include <type_traits>

namespace Po7
   {
    // sockaddr and sockaddr_storage are structure types, but there's wacky casting to clean up
        using ::sockaddr;
        using ::sockaddr_storage;

    // is_sockaddr is a boolean trait that marks domain-specialized sockaddr classes    
        template < class T > struct is_sockaddr: std::false_type {};
    
    // is_sockaddr_castable determines which sockaddr casts are reasonable.
    // Any sockaddr type casts to plain sockaddr, and sockaddr_storage casts to any sockaddr type.
    
        template < class From, class To > struct is_sockaddr_castable: std::false_type {};
        
        template < class From > struct is_sockaddr_castable< const From&,             const sockaddr& >: is_sockaddr< From > {};
        template < class From > struct is_sockaddr_castable<       From&,                   sockaddr& >: is_sockaddr< From > {};
        template < class To   > struct is_sockaddr_castable< const sockaddr_storage&, const To&       >: is_sockaddr< To   > {};
        template < class To   > struct is_sockaddr_castable<       sockaddr_storage&,       To&       >: is_sockaddr< To   > {};
        template <>             struct is_sockaddr_castable< const sockaddr_storage&, const sockaddr& >: std::true_type      {};
        template <>             struct is_sockaddr_castable<       sockaddr_storage&,       sockaddr& >: std::true_type      {};
    
    // And sockaddr_cast reinterpret casts between sockaddr, sockaddr_storage and domain specific types
        template < class To, class From >
        auto sockaddr_cast( From&& from )
        -> typename std::enable_if< is_sockaddr_castable< From&&, To >::value, To >::type
           {
            return reinterpret_cast< const To& >( std::forward<From>( from ) );
           }



    // socket_domain_t is effectively a template parameter, determing the address type of a socket.
        enum class socket_domain_t: int {};
        template <> struct Wrapper< socket_domain_t >: PlusPlus::EnumWrapper< socket_domain_t > {};

        const socket_domain_t af_inet   = socket_domain_t( AF_INET );
        const socket_domain_t af_unix   = socket_domain_t( AF_UNIX );
        const socket_domain_t af_unspec = socket_domain_t( AF_UNSPEC );
        const socket_domain_t af_inet6  = socket_domain_t( AF_INET6 );
    

    
    // sockaddr_type maps domains to sockaddr types.  It has the usual membber named type.
        template < socket_domain_t > struct sockaddr_type_trait;
        
        template < socket_domain_t domain > using sockaddr_type = typename sockaddr_type_trait< domain >::type;
        
    // And sockaddr_domain maps sockaddr types to domains.  It's an integral_constant of type socket_domain_t.
        template < class addr_type > struct sockaddr_domain;
   }

#endif
