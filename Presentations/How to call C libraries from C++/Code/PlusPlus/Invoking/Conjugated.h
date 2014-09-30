//
//  Conjugated.h
//  PlusPlus
//
//  Created by Lisa Lippincott on 7/29/14.
//  Released into the public domain by Lisa Lippincott, 2014.
//

#ifndef PLUSPLUS_CONJUGATED_H
#define PLUSPLUS_CONJUGATED_H

#include <utility>
#include <type_traits>
#include <tuple>

/*
    Conjugate< R, W >( f ) produces a functor that calls f, prefixed by applying the inverse of W to each input,
    and applying W to each output.  The specialization of W for R is applied to the function.
    
    W must be an invertible functor family (see Inverted.h).  In PlusPlus::Invoke, functors are conjugated 
    by both wrappers and seizers, but the names used internally in this file follow the nomenclature of wrapping.
    
    Any parameters passed as lvalue references are treated as both inputs and outputs.  When unwrapping such a 
    parameter produces a nonconst lvalue reference, no wrapping is performed.
    
    That is, for parameters p of types P, Conjugate<R,W>( f )( p... ) does these things:
    
        Evaluates W< decay<P>::type >().Inverse( p )... to produce unwrapped parameters.
        Calls f with those parameters.
        Where necessary, wraps output parameters with  W< decay<P>::type >()( unwrapped ).
        If R is not void, wraps the function result with W< decay<R>::type >()( result ), and returns that value.
        
    Conjugated may be combined with temporary_reference_wrapper to avoid copying f.
*/

namespace PlusPlus
   {
    template < class T > using is_lvalue_reference_to_nonconst
        = std::integral_constant< bool, std::is_lvalue_reference< T >::value && !std::is_const< typename std::remove_reference< T >::type >::value >;

    template < class... T > void DoNothingWith( T&&... ) {}


    
    template < class Wrapper,
               class W,
               bool rewrap = is_lvalue_reference_to_nonconst< W&& >::value
                             && !is_lvalue_reference_to_nonconst< decltype( Wrapper().Inverse( std::declval<W&&>() ) ) >::value >
    class TemporarilyUnwrapped;
    
    template < class Wrapper, class W >
    class TemporarilyUnwrapped< Wrapper, W, false >
       {
        private:
            using ResultOfUnwrap  = decltype( Wrapper().Inverse( std::declval<W&&>() ) );
            using UnwrappedValue  = typename std::decay< ResultOfUnwrap >::type;

            using UnwrappedMember = ResultOfUnwrap;
            
            UnwrappedMember unwrapped;
           
        public:
            explicit TemporarilyUnwrapped( W&& w )          : unwrapped( Wrapper().Inverse( std::forward<W>(w) ) ) {}
           
            UnwrappedMember&& Get()                         { return std::forward<UnwrappedMember>( unwrapped ); }
            void RewrapIfNecessary()                        {}
       };

    template < class Wrapper, class W >
    class TemporarilyUnwrapped< Wrapper, W, true >
       {
        private:
            using ResultOfUnwrap  = decltype( Wrapper().Inverse( std::declval<W&&>() ) );
            using UnwrappedValue  = typename std::decay< ResultOfUnwrap >::type;

            using UnwrappedMember   = UnwrappedValue;
            
            W&& wrapped;
            UnwrappedMember unwrapped;
           
        public:
            explicit TemporarilyUnwrapped( W&& w )          : wrapped( std::forward<W>(w) ), unwrapped( Wrapper().Inverse( std::forward<W>(w) ) ) {}
           
            UnwrappedMember& Get()                          { return unwrapped; }
            void RewrapIfNecessary()                        { wrapped = Wrapper()( std::move( unwrapped ) ); }
       };

    template < template <class W> class WrapperFamily, class W > using TemporarilyUnwrappedType = TemporarilyUnwrapped< WrapperFamily< typename std::decay<W>::type >, W&& >;

    template < template <class W> class WrapperFamily, class W >
    auto TemporarilyUnwrap( W&& w ) -> TemporarilyUnwrapped< WrapperFamily< typename std::decay<W>::type >, W&& >
       {
        return TemporarilyUnwrapped< WrapperFamily< typename std::decay<W>::type >, W&& >( std::forward<W>(w) );
       }




    template < class Result, class ResultWrapper >
    struct TemporarilyUnwrappedInvoker
       {        
        template < class Callable, class... TemporarilyUnwrappedTypes >
        Result operator()( Callable&& function, TemporarilyUnwrappedTypes&&... unwrapped ) const
           {
            using UnwrappedResult = decltype( std::forward< Callable >( function )( unwrapped.Get()... ) );
            using ResultOfWrap    = decltype( ResultWrapper()( std::declval< UnwrappedResult >() ) );
            
            static const bool resultIsReference          = std::is_reference< Result >::value;
            static const bool unwrappedResultIsReference = std::is_reference< UnwrappedResult >::value;
            static const bool wrappingMakesReference     = std::is_reference< ResultOfWrap >::value;

            static_assert( !resultIsReference || ( unwrappedResultIsReference && wrappingMakesReference ),
                           "Reference result would refer to an expired object." );
            
            UnwrappedResult unwrappedResult = std::forward< Callable >( function )( unwrapped.Get()... );
            
            DoNothingWith( ( unwrapped.RewrapIfNecessary(), nullptr )... );
            return ResultWrapper()( std::forward<UnwrappedResult>( unwrappedResult ) );
           }
       };
    
    template < class ResultWrapper >
    struct TemporarilyUnwrappedInvoker< void, ResultWrapper >
       {
        template < class Callable, class... TemporarilyUnwrappedTypes >
        void operator()( Callable&& function, TemporarilyUnwrappedTypes&&... unwrapped ) const
           {
            std::forward< Callable >( function )( unwrapped.Get()... );
            DoNothingWith( ( unwrapped.RewrapIfNecessary(), nullptr )... );
           }
       };
    
    
    
    template < class Result,
               template <class W> class WrapperFamily,
               class Callable,
               class... W >
    Result InvokeConjugated( Callable&& function, W&&... w )
       {
        using Invoker = TemporarilyUnwrappedInvoker< Result, WrapperFamily< typename std::decay<Result>::type > >;
        
        return Invoker()( std::forward<Callable>( function ), TemporarilyUnwrappedType<WrapperFamily, W>( std::forward<W>(w) )... );
       }
    
    

    template < class Result,
               template <class W> class WrapperFamily,
               class Callable >
    class Conjugated
       {
        private:
            Callable function;
            
        public:
            Conjugated( Callable f )                                            : function( std::move(f) ) {}
            
            template < class... W > Result operator()( W&&... w ) const &       { return InvokeConjugated< Result, WrapperFamily >( function, std::forward<W>(w)... ); }
            template < class... W > Result operator()( W&&... w ) &             { return InvokeConjugated< Result, WrapperFamily >( function, std::forward<W>(w)... ); }
            template < class... W > Result operator()( W&&... w ) &&            { return InvokeConjugated< Result, WrapperFamily >( std::forward<Callable>( function ), std::forward<W>(w)... ); }
       };

    template < class Result,
               template <class W> class WrapperFamily,
               class Callable >
    Conjugated< Result, WrapperFamily, typename std::decay<Callable>::type >
    Conjugate( Callable&& function )
       {
        return Conjugated< Result, WrapperFamily, typename std::decay<Callable>::type >( std::forward<Callable>( function ) );
       }
   }

#endif
