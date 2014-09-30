//
//  Seizer.h
//  PlusPlus
//
//  Created by Lisa Lippincott on 7/16/14.
//  Released into the public domain by Lisa Lippincott, 2014.
//

#ifndef PLUSPLUS_SEIZER_H
#define PLUSPLUS_SEIZER_H

#include "UniquePtrSeizer.h"
#include "Inverted.h"

#include <memory>

/*
    A seizer family is an invertible functor family (see Inverted.h) that establishes a correspondence 
    between a ownership-bearing type (usually std::unique_ptr; I don't know why any other class would be used)
    and its underlying resource type.  It's expected that each library relying on PlusPlus will have a 
    single seizer family to handle seizing and releasing resources uniformly.

    The Inverse member of a Seizer -- the releasing function -- is generally overloaded to take either an
    rvalue or an lvalue reference, but not references to const unless the Seizer is trivial.

    Seizing and releasing resources must not throw exceptions. Also, copying, assigning, default constructing, 
    destroying, wrapping and unwrapping released resources must not throw exceptions.
    
    This file provides two starting points, intended to be used as base classes for a library's 
    seizer, like this:
    
    namespace UnderlyingLibrary_PlusPlus
       {
        template < class S > struct Seizer: PlusPlus::DefaultSeizer<S> {};
       }
    
    The Seizer template can then be spacialized to handle the particular resource needs of UnderlyingLibrary_PlusPlus;
    UniquePtrSeizer provides a seizer implementations for std::unique_ptr, which can also be used by inheritance:
    
    namespace UnderlyingLibrary_PlusPlus
       {
        template <> struct Seizer< std::unique_ptr<Foo> >: PlusPlus::UniquePtrSeizer< std::unique_ptr<Foo> > {};
       }
    
    
    The two starting points are:
        
        DefaultSeizer                       Trivial for all types.
        
        AllUniquePtrsAreSeized              This unwrapper assumes all std::unique_ptr types may be released by 
                                            UnderlyingLibrary_PlusPlus.  If the pointer type in the unique_ptr is 
                                            an PointerToValue type, releasing also strips the PointerToValue layer.
                                            Don't use this if the underlying library uses std::unique_ptr.
    
    I don't see much need for specializing Seizers.  I don't know why one would use PlusPlus when the underlying 
    library uses unique_ptr, and I don't know why one would need a unique ownership class other than unique_ptr
    (possibly with a disposer that uses PointerToValue).  So the usual case should just be AllUniquePtrsAreSeized.
*/

namespace PlusPlus
   {
    template < class W > using DefaultSeizer = InvertibleIdentityFunctor<W>;


    template < class SeizedType > struct AllUniquePtrsAreSeized:                         DefaultSeizer< SeizedType >             {};
    template < class T, class D > struct AllUniquePtrsAreSeized< std::unique_ptr<T,D> >: UniquePtrSeizer< std::unique_ptr<T,D> > {};



    template < template <class> class SeizerFamily, class S, class R >
    auto Seize( R&& r )
    -> decltype( SeizerFamily<S>()( std::forward<R>(r) ) )
       { return  SeizerFamily<S>()( std::forward<R>(r) ); }
       
    template < template <class> class SeizerFamily, class S >
    auto Release( S&& s )
    -> decltype( SeizerFamily< typename std::decay<S>::type >().Inverse( std::forward<S>(s) ) )
       { return  SeizerFamily< typename std::decay<S>::type >().Inverse( std::forward<S>(s) ); }



    template < template <class S> class SeizerFamily >
    using Releaser = Inverted< SeizerFamily >;
   }

#endif
