//
//  Po7_inet.cpp
//  PlusPlus
//
//  Created by Lisa Lippincott on 9/1/14.
//  Released into the public domain by Lisa Lippincott, 2014.
//

#include "Po7_inet.h"
#include "Po7_Invoke.h"

const char *Po7::inet_ntop( socket_domain_t af,
                            const void *src,
                            char *dst,
                            socklen_t size )
   {
    return Invoke( Result< const char * >() + FailsWhenFalse(),
                   ::inet_ntop,
                   In( af, src, dst, size ),
                   ThrowErrorFromErrno() );
   }

void Po7::inet_pton( socket_domain_t af,
                     const char *src,
                     void *dst )
   {
    return Invoke( Result< int >() + NotReturned() + FailsWhen( []( int r ){ return r != 1; } ),
                   ::inet_pton,
                   In( af, src, dst ),
                   ThrowErrorFromErrno() );
   }
