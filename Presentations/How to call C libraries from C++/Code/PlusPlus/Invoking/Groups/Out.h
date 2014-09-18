//
//  Out.h
//  PlusPlus
//
//  Created by Lisa Lippincott on 8/17/14.
//  Released into the public domain by Lisa Lippincott, 2014.
//

#ifndef PLUSPLUS_OUT_H
#define PLUSPLUS_OUT_H

#include "Checked.h"
#include "Thrown.h"
#include "Returned.h"
#include "integer_sequence.h"

#include <tuple>

/*
    Out<P...>() produces an output parameter group.  Parameters of type 
    P... are value-constructed and passed by lvalue reference.
    
    These parameters are returned by default, but this can be changed by adding +NotReturned().
    
    The parameters p... are not checked or thrown by default, but the group can be modified 
    (by +Checked(f) or +Thrown()) to be checked or thrown.

    OutFailureFlag<P...>()  is equivalent to Out<P...>() + Checked( AnyIsTrue() )  + NotReturned().
    OutSuccessFlag<P...>()  is equivalent to Out<P...>() + Checked( AnyIsFalse() ) + NotReturned().

    OutError<P...>()        is equivalent to Out<P...>() + NotReturned() + Thrown().
    OutError<P...>(f)       is equivalent to Out<P...>() + Checked(f) + NotReturned() + Thrown().
*/

namespace PlusPlus
   {
    template < class FailureChecker, bool thrown, bool returned, class... P >
    class OutParameterGroup
       {
        private:
            using ValueTuple          = std::tuple< P... >;
            using ReferenceTuple      = std::tuple< P&... >;
            using ConstReferenceTuple = std::tuple< const P&... >;

            using SelectForThrowing  = ThrowSelector<  thrown >;
            using SelectForReturning = ReturnSelector< returned >;
            
            FailureChecker checkFailure;
            ValueTuple parameters;
            
        public:
            explicit OutParameterGroup( FailureChecker f )
               : checkFailure( std::move(f) )
               {}

            ReferenceTuple PassedParts()                    { return parameters; }
            
            bool CheckForFailure() const                    { return stdish::apply( checkFailure, ConstReferenceTuple( parameters ) ); }
            
            auto ThrownParts()      -> decltype( SelectForThrowing()(  parameters ) )    { return SelectForThrowing()(  std::move( parameters ) ); }
            auto ReturnedParts()    -> decltype( SelectForReturning()( parameters ) )    { return SelectForReturning()( std::move( parameters ) ); }


            template < class C > using WithDifferentChecker  = OutParameterGroup< C,              thrown, returned, P... >;
            template < bool t >  using WithDifferentThrown   = OutParameterGroup< FailureChecker, t,      returned, P... >;
            template < bool r >  using WithDifferentReturned = OutParameterGroup< FailureChecker, thrown, r,        P... >;

            template < class C >
            friend WithDifferentChecker<C> operator+( OutParameterGroup, CheckFailureWith<C> checker )
               {
                return WithDifferentChecker<C>( std::move( checker ).Checker() );
               }
            
            template < bool t >
            friend WithDifferentThrown<t> operator+( OutParameterGroup original, ThrowSelector<t> )
               {
                return WithDifferentThrown<t>( std::move( original.checkFailure ) );
               }
            
            template < bool r >
            friend WithDifferentReturned<r> operator+( OutParameterGroup original, ReturnSelector<r> )
               {
                return WithDifferentReturned<r>( std::move( original.checkFailure ) );
               }
       };
    
            
    
    template < class... P >
    OutParameterGroup< NeverFails, false, true, P... >
    Out()
       {
        return OutParameterGroup< NeverFails, false, true, P... >( NeverFails() );
       }

    template < class... P >
    OutParameterGroup< AnyIsTrue, false, false, P... >
    OutFailureFlag()
       {
        return OutParameterGroup< AnyIsTrue, false, false, P... >( AnyIsTrue() );
       }

    template < class... P >
    OutParameterGroup< AnyIsFalse, false, false, P... >
    OutSuccessFlag()
       {
        return OutParameterGroup< AnyIsFalse, false, false, P... >( AnyIsFalse() );
       }

    template < class P >
    OutParameterGroup< AnyIsTrue, true, false, P >
    OutError()
       {
        return OutParameterGroup< AnyIsTrue, true, false, P >( AnyIsTrue() );
       }

    template < class P, class FailureChecker >
    OutParameterGroup< FailureChecker, true, false, P >
    OutError( FailureChecker failureChecker )
       {
        return OutParameterGroup< FailureChecker, true, false, P >( std::move( failureChecker ) );
       }
   }

#endif
