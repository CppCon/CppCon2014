//
//  Wrapper.h
//  PlusPlus
//
//  Created by Lisa Lippincott on 7/11/14.
//  Released into the public domain by Lisa Lippincott, 2014.
//

#ifndef PLUSPLUS_WRAPPER_H
#define PLUSPLUS_WRAPPER_H

#include "Boxed.h"
#include "EnumWrapper.h"
#include "Inverted.h"

#include <utility>
#include <type_traits>

/*
    A wrapper family is an invertible functor family (see Inverted.h) that establishes a correspondence 
    between a strong type (often a class or enum class) and a weaker underlying type.
    It's expected that each library relying on PlusPlus will have a single wrapper family 
    to handle its wrapping uniformly.

    Unwrapping is expected to be idempotent: if an object is double-unwrapped using the same wrapper family, 
    the second unwrap must be trivial, and must not throw an exception.
    
    This file provides three starting points, intended to be used as base classes for a library's 
    wrapper, like this:
    
    namespace UnderlyingLibrary_PlusPlus
       {
        template < class W > struct Wrapper: PlusPlus::DefaultWrapper<W> {};
       }
    
    The Wrapper template can then be specialized to handle the particular wrapping needs of UnderlyingLibrary_PlusPlus;
    Boxed.h and EnumWrapper.h provide wrapper implementations for PlusPlus::Boxed and enum types, which
    can also be used by inheritance:
    
    namespace UnderlyingLibrary_PlusPlus
       {
        struct SomeBoxTag
           {
            using ContentType = int;
            static ContentType DefaultContent()     { return 0; }
           };
        
        using SomeBoxType = PlusPlus::Boxed< SomeBoxTag >;
        
        template <> struct Wrapper< SomeBoxType >: PlusPlus::BoxedWrapper< SomeBoxType > {};
        
        
        enum class SomeEnumType: int;
        
        template <> struct Wrapper< SomeEnumType  >: PlusPlus::EnumWrapper< SomeEnumType > {};
       }
    
    
    The three starting points are:
        
        DefaultWrapper                      Trivial for all types.
        
        AllBoxedTypesAreWrapped             This unwrapper assumes all PlusPlus::Boxed types may be unwrapped by 
                                            UnderlyingLibrary_PlusPlus.  Don't use this if the underlying library uses PlusPlus::Boxed.
                                            (If the underlying library does use PlusPlus, something must have gone wrong.)
        
        AllBoxedAndEnumTypesAreWrapped      This unwrapper assumes all enums and PlusPlus::Boxed types may be unwrapped by 
                                            SomeLibrary.  Don't use this if the underlying library uses enums or PlusPlus::Boxed.
*/

namespace PlusPlus
   {
    template < class W > using DefaultWrapper = InvertibleIdentityFunctor<W>;

    
    template < class W > struct AllBoxedTypesAreWrapped:             DefaultWrapper< W >      {};
    template < class T > struct AllBoxedTypesAreWrapped< Boxed<T> >: BoxedWrapper< Boxed<T> > {};
    
    
    template < class W, bool isEnum = std::is_enum<W>::value > struct AllBoxedAndEnumTypesAreWrapped;
    
    template < class W > struct AllBoxedAndEnumTypesAreWrapped< W, false >: AllBoxedTypesAreWrapped<W> {};
    template < class W > struct AllBoxedAndEnumTypesAreWrapped< W, true  >: EnumWrapper<W>             {};
    
    
    
    template < template <class> class WrapperFamily, class W, class U >
    auto Wrap( U&& u )
    -> decltype( WrapperFamily<W>()( std::forward<U>(u) ) )
       { return  WrapperFamily<W>()( std::forward<U>(u) ); }
       
    template < template <class> class WrapperFamily, class W >
    auto Unwrap( W&& w )
    -> decltype( WrapperFamily< typename std::decay<W>::type >().Inverse( std::forward<W>(w) ) )
       { return  WrapperFamily< typename std::decay<W>::type >().Inverse( std::forward<W>(w) ); }
    
    
    
    template < template <class W> class WrapperFamily >
    using Unwrapper = Inverted< WrapperFamily >;
   }

#endif
