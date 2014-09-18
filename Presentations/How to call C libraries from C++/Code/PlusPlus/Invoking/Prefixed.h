//
//  Prefixed.h
//  PlusPlus
//
//  Created by Lisa Lippincott on 8/27/14.
//  Released into the public domain by Lisa Lippincott, 2014.
//

#ifndef PLUEPLUS_PREFIXED_H
#define PLUEPLUS_PREFIXED_H

#include <utility>
#include <type_traits>

/*
    Prefix< F >( f ) produces a functor that calls f, prefixed by applying F to each input as a forwarder.
    That is, for parameters p of types P&&,
    
        Prefix<F>( f )( p... )
    
    calls f like:
    
        f( F<P>()( p )... )
    
    In PlusPlus::Invoke, Prefixed is combined with temporary_reference_wrapper to 
    avoid copying the invoked function.
*/

namespace PlusPlus
   {
    template < template <class P> class FunctorFamily,
               class Callable,
               class... P >
    auto InvokePrefixed( Callable&& function, P&&... p )
    -> decltype( std::forward<Callable>( function )( FunctorFamily<P>()( std::forward<P>( p ) )... ) )
       {
        return   std::forward<Callable>( function )( FunctorFamily<P>()( std::forward<P>( p ) )... );
       }
    
    

    template < template <class P> class FunctorFamily,
               class Callable >
    class Prefixed
       {
        private:
            Callable function;
            
        public:
            Prefixed( Callable f )                                        : function( std::forward<Callable>(f) ) {}
            
            template < class... P >
            auto operator()( P&&... p ) const &
            -> decltype( InvokePrefixed< FunctorFamily >( function, std::forward<P>(p)... ) )
               { return  InvokePrefixed< FunctorFamily >( function, std::forward<P>(p)... ); }

            template < class... P >
            auto operator()( P&&... p ) &
            -> decltype( InvokePrefixed< FunctorFamily >( function, std::forward<P>(p)... ) )
               { return  InvokePrefixed< FunctorFamily >( function, std::forward<P>(p)... ); }

            template < class... P >
            auto operator()( P&&... p ) &&
            -> decltype( InvokePrefixed< FunctorFamily >( std::forward<Callable>( function ), std::forward<P>(p)... ) )
               { return  InvokePrefixed< FunctorFamily >( std::forward<Callable>( function ), std::forward<P>(p)... ); }
       };



    template < template <class P> class FunctorFamily,
               class Callable >
    Prefixed< FunctorFamily, typename std::decay<Callable>::type >
    Prefix( Callable&& function )
       {
        return Prefixed< FunctorFamily, typename std::decay<Callable>::type >( std::forward<Callable>( function ) );
       }
   }

#endif
