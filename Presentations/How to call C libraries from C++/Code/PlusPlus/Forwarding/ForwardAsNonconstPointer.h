//
//  ForwardAsNonconstPointer.h
//  PlusPlus
//
//  Created by Lisa Lippincott on 8/28/14.
//  Released into the public domain by Lisa Lippincott, 2014.
//

#ifndef PLUSPLUS_FORWARDASNONCONSTPOINTER_H
#define PLUSPLUS_FORWARDASNONCONSTPOINTER_H

#include "ForwardAsPointer.h"

#include <type_traits>

// ForwardAsNonconstPointer<T> is a forwarder functor that passes objects of type T
// by non-const pointer, even if T is const.  This is, of course, dangerous.

namespace PlusPlus
   {
    template < class T, bool isConst = std::is_const<T>::value >
    struct PointerConstCaster;
    
    // This specialization is necessary because const can't be cast away from a function pointer.
    template < class T > struct PointerConstCaster< T, false >      { T *operator()(       T *p ) const     { return p; }                   };
    template < class T > struct PointerConstCaster< const T, true > { T *operator()( const T *p ) const     { return const_cast<T*>( p ); } };

    template < class T >
    auto CastConstAwayFromPointer( T *p )
    -> typename std::remove_const<T>::type *
       {
        return PointerConstCaster< T >()( p );
       }
    
    template < class T >
    struct ForwardAsNonconstPointer
       {
        auto operator()( T&& t ) const
        -> decltype( CastConstAwayFromPointer( MakePointer( std::forward<T>(t) ) ) )
           { return  CastConstAwayFromPointer( MakePointer( std::forward<T>(t) ) ); }
       };
   }

#endif
