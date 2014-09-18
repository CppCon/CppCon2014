//
//  UniquePtrSeizer.h
//  PlusPlus
//
//  Created by Lisa Lippincott on 8/23/14.
//  Released into the public domain by Lisa Lippincott, 2014.
//

#ifndef PLUSPLUS_UNIQUEPTRSEIZER_H
#define PLUSPLUS_UNIQUEPTRSEIZER_H

#include "PointerToValue.h"

#include <memory>

/*
    UniquePtrSeizer< std::unique_ptr< T, D > > is a functor intended to be used to make 
    specializations of Seizer.  It constructs a unique_ptr when seizing, and calls release 
    when releasing the resource.
    
    It has special behavior when D::pointer is a specialization of PlusPlus::PointerToValue.
    In this case it also strips the PointerToValue from the released resource.
*/

namespace PlusPlus
   {
    template < class UniquePtrType, class PointerType = typename UniquePtrType::pointer >
    class UniquePtrSeizer
       {
        using Seized   = UniquePtrType;
        using Released = typename Seized::pointer;

        Seized operator()( Released r ) const               { return Seized( std::move( r ) ); }

        Released Inverse( Seized&  s ) const                { return s.release(); }
        Released Inverse( Seized&& s ) const                { return s.release(); }
       };
    
    template < class UniquePtrType, class ReleasedType >
    struct UniquePtrSeizer< UniquePtrType, PointerToValue<ReleasedType> >
       {
        using Seized   = UniquePtrType;
        using Pointer  = typename Seized::pointer;
        using Released = typename std::decay< decltype( *std::declval<Pointer>() ) >::type;

        Seized operator()( Released r ) const               { return Seized( Pointer( std::move( r ) ) ); }

        Released Inverse( Seized&  s ) const                { return *s.release(); }
        Released Inverse( Seized&& s ) const                { return *s.release(); }
       };
   }

#endif
