//
//  Po7_Basics.h
//  PlusPlus
//
//  Created by Lisa Lippincott on 8/30/14.
//  Released into the public domain by Lisa Lippincott, 2014.
//

#ifndef PO7_BASICS_H
#define PO7_BASICS_H

#include "Wrapper.h"
#include "Seizer.h"

#include <system_error>

namespace Po7
   {
    // Wrap and Unwrap convert between Po7 wrapped types and the underlying type used by POSIX.
    // All PlusPlus::Boxed types used as parameters or retured by Po7 are treated as wrapped by Po7;
    // other types are marked as wrapped by specializations of Po7::Wrapper.
    //
    // See PlusPlus/Wrapper.h for details.
    
        template < class W > struct Wrapper: PlusPlus::AllBoxedTypesAreWrapped<W> {};

        template < class W, class U >
        auto Wrap( U&& u )
        -> decltype( PlusPlus::Wrap< Wrapper, W >( std::forward<U>(u) ) )
           { return  PlusPlus::Wrap< Wrapper, W >( std::forward<U>(u) ); }
           
        template < class W >
        auto Unwrap( W&& w )
        -> decltype( PlusPlus::Unwrap< Wrapper >( std::forward<W>(w) ) )
           { return  PlusPlus::Unwrap< Wrapper >( std::forward<W>(w) ); }

    // Seize and Release convert between Po7 owned resources and the underlying type used by POSIX.
    // All std::unique_ptr types used as parameters or retured by Po7 are treated as seized by Po7;
    // other types could be marked as seized by specializations of Po7::Seizer, but there likely aren't any.
    //
    // Release generally only works on rvalues.
    //
    // See PlusPlus/Seizer.h for details.

        template < class S > struct Seizer: PlusPlus::AllUniquePtrsAreSeized<S> {};

        template < class S, class R >
        auto Seize( R&& r )
        -> decltype( PlusPlus::Seize<Seizer,S>( std::forward<R>(r) ) )
           { return  PlusPlus::Seize<Seizer,S>( std::forward<R>(r) ); }
           
        template < class S >
        auto Release( S&& s )
        -> decltype( PlusPlus::Release<Seizer>( std::forward<S>(s) ) )
           { return  PlusPlus::Release<Seizer>( std::forward<S>(s) ); }

    // Make handles conversions and construction-like operations for types that aren't Po7 wrappers.
    // It's used to fill in POSIX structure types, like sockaddr_in, and also to convert to C++ standard
    // types, like std::string or std::chrono::duration.  Make is extended by overloading MakeAnything,
    // with ThingToMake<T> as the first parameter type. 

        template < class Result > struct ThingToMake {};
        
        template < class Result, class... P >
        Result Make( P&&... p )
           {
            return MakeAnything( ThingToMake<Result>(), std::forward<P>(p)... );
           }


    // Posix error codes are wrapped by std::error_code.
    
        template <>
        struct Wrapper< std::error_code >
           {
            std::error_code operator()( int code ) const        { return std::error_code( code, std::system_category() ); }
            
            int Inverse( const std::error_code& e ) const       { return e.value(); }
           };
   }

#endif
