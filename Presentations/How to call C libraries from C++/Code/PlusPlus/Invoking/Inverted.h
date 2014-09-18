//
//  Inverted.h
//  PlusPlus
//
//  Created by Lisa Lippincott on 8/28/14.
//  Released into the public domain by Lisa Lippincott, 2014.
//

#ifndef PLUSPLUS_INVERTED_H
#define PLUSPLUS_INVERTED_H

#include <utility>

/*
    An invertible functor family is a template taking one type argument
    where each specialization is a functor with an inverse operation, 
    establishing a translation between two types, W and U:
    
    template < class W >
    struct InvertibleFunctorFamily
       {
        <W or reference to W> operator()( <reference to U> ) const
        <U or reference to U> Inverse( <reference to W> ) const
       };

    The parameter W is always a decayed type; a single specialization handles all references to W.

    This abstraction is used for both type wrappers and resource holders.
    In the terminology of wrappers, W is the wrapped type, and U is the unwrapped type;
    in the terminology of resources, W is the seized resource type, and U us the released type.
    
    A central assumption in the design is that each wrapped type maps to exactly one 
    unwrapped type, but 
    
    The Inverse member is a two-sided inverse for operator():
        
        Family<W>()( Family<W>().Inverse( w ) )   has the same value as w
        Family<W>().Inverse( Family<W>()( u ) )   has the same value as u
    
    If either operation returns a reference, it is assumed that modification of the referred-to-object
    corresponds to modification of the parameter.  That is, (ignoring const) when operator() returns a reference,
    
        Family<W>()( u ) = w   is equivalent to   u = Family<W>().Inverse( w ),
        
    and when Inverse returns a reference (again ignoring const),

        Family<W>().Inverse( w ) = u   is equivalent to   w = Family<W>()( u ),
    
    
    When defining InvertibleFunctorFamilies, it is convenient that they are class templates: 
    they can be defined piecemeal, putting full or partial spacializations with the types 
    to which they apply.  The Family InvertibleIdentityFunctor is a good starting point: 
    it defines a do-nothing operation for every type.
    
    Sometimes it's handy to have a single class representing an entire family.
    The Inverted template provides that: its operator() will unwrap or release any parameter 
    while its Inverse<W> member wraps back to W.
*/

namespace PlusPlus
   {
    template < class TheWrappedType >
    class InvertibleIdentityFunctor
       {
        private:
            using Wrapped   = TheWrappedType;
            using Unwrapped = Wrapped;

        public:
            const Wrapped&  operator()( const Unwrapped&  u ) const             { return u; }
            Wrapped&        operator()(       Unwrapped&  u ) const             { return u; }
            Wrapped&&       operator()(       Unwrapped&& u ) const             { return std::move( u ); }

            const Unwrapped& Inverse( const Wrapped&   w ) const                { return w; }
            Unwrapped&       Inverse(       Wrapped&   w ) const                { return w; }
            Unwrapped&&      Inverse(       Wrapped&&  w ) const                { return std::move( w ); }
       };
    
    
    
    template < template <class W> class InvertibleFunctorFamily >
    class Inverted
       {
        private:
            template < class W > using Wrapper        = InvertibleFunctorFamily< typename std::decay<W>::type >;
            template < class W > using ResultOfUnwrap = decltype( Wrapper<W>()( std::declval<W&&>() ) );
            template < class W > using Unwrapped      = typename std::decay< ResultOfUnwrap<W> >::type;
        
        public:
            template < class W > ResultOfUnwrap<W> operator()( W&& w ) const                                                { return Wrapper<W>().Inverse( std::forward<W>(w) ); }
        
            template < class W > auto Inverse( const Unwrapped<W>&  u ) const -> decltype( Wrapper<W>()( u ) )              { return Wrapper<W>()( u ); }
            template < class W > auto Inverse(       Unwrapped<W>&  u ) const -> decltype( Wrapper<W>()( u ) )              { return Wrapper<W>()( u ); }
            template < class W > auto Inverse(       Unwrapped<W>&& u ) const -> decltype( Wrapper<W>()( std::move(u) ) )   { return Wrapper<W>()( std::move(u) ); }
       };
   }

#endif
