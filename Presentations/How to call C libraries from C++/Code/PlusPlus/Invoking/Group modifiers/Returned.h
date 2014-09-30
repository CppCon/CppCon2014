//
//  Returned.h
//  PlusPlus
//
//  Created by Lisa Lippincott on 8/16/14.
//  Released into the public domain by Lisa Lippincott, 2014.
//

#ifndef PLUSPLUS_RETURNED_H
#define PLUSPLUS_RETURNED_H

#include <tuple>

/*
    The items in this file are used to modify the basic parameter and result groups 
    to change whether a parameter is returned.  For example:
    
        In( p... )        + Returned()      in addition to any other results, returns p...
        InOut( p... )     + Returned()      in addition to any other results, returns p... (after modification)
        NotPassed( p... ) + Returned()      in addition to any other results, returns p...
        
        Out<T...>() + NotReturned()         passes an output parameter, but does not return it
        Result<T>() + NotReturned()         expects a result of type T, but does not return it
*/

namespace PlusPlus
   {
    template < bool thrown > struct ReturnSelector;
    
    template <>
    struct ReturnSelector< true >
       {
        template < class... P >
        std::tuple<P...> operator()( std::tuple<P...> toReturn ) const      { return toReturn; }
       };
    
    template <>
    struct ReturnSelector< false >
       {
        template < class... P >
        std::tuple<> operator()( std::tuple<P...> ) const                   { return std::tuple<>(); }
       };
    
    inline ReturnSelector<true>  Returned()        { return ReturnSelector<true>(); }
    inline ReturnSelector<false> NotReturned()     { return ReturnSelector<false>(); }
   }

#endif
