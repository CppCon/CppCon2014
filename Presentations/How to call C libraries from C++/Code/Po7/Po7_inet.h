//
//  Po7_inet.h
//  PlusPlus
//
//  Created by Lisa Lippincott on 9/1/14.
//  Released into the public domain by Lisa Lippincott, 2014.
//

#ifndef PO7_INET_H
#define PO7_INET_H

#include "Po7_in.h"
#include "Po7_socket.h"

#include <arpa/inet.h>

namespace Po7
   {
    // Can't wrap htonl, htons, ntohs, and ntohs because they may be macros.
    // But there are corresponding MakeAnything functions in Po7_in.h.
    
    // I'm also skipping inet_addr and inet_ntoa, in part because inet_ntoa isn't thread-safe.
    // Use inet_ntop and inet_pton instead.  Or use Make or stream operators from Po7_in.
    
    // The basic inet_ntop and inet_pton have weakly typed interfaces:
        const char *inet_ntop( socket_domain_t, const void *src, char *dst, socklen_t size );
        void inet_pton( socket_domain_t, const char *src, void *dst );

    // This traits class matches domains to better interface types:

        template < socket_domain_t > struct inet_addr_type;
        
        template <> struct inet_addr_type< af_inet  >   { using type = in_addr_t; };
        template <> struct inet_addr_type< af_inet6 >   { using type = in6_addr;  };

    // This is the minimum length of a buffer for inet_ntop:
        template < socket_domain_t > struct inet_addr_string_length;
        
        template <> struct inet_addr_string_length< af_inet  >   : std::integral_constant< socklen_t, INET_ADDRSTRLEN  > {};
        template <> struct inet_addr_string_length< af_inet6 >   : std::integral_constant< socklen_t, INET6_ADDRSTRLEN > {};
    
    // These templates are more strongly typed than the functions above:
    
        template < socket_domain_t domain, std::size_t n >
        const char *inet_ntop( const typename inet_addr_type< domain >::type& src,
                               char (&dst)[n] )
           {
            static_assert( n >= inet_addr_string_length<domain>::value, "Buffer too small" );
            return inet_ntop( domain, &src, dst, n );
           }

        template < socket_domain_t domain >
        auto inet_pton( const char *src ) -> typename inet_addr_type< domain >::type
           {
            typename inet_addr_type< domain >::type result;
            inet_pton( domain, src, &result );
            return result;
           }
   }

#endif
