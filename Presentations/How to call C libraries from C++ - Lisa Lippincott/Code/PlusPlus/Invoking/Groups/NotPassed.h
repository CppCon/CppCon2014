//
//  NotPassed.h
//  PlusPlus
//
//  Created by Lisa Lippincott on 8/22/14.
//  Released into the public domain by Lisa Lippincott, 2014.
//

#ifndef PLUSPLUS_NOTPASSED_H
#define PLUSPLUS_NOTPASSED_H

#include "Checked.h"
#include "Thrown.h"
#include "Returned.h"
#include "integer_sequence.h"

#include <tuple>

/*
    NotPassed( p... ) produces a parameter group that passes no parameters.
    The parameters p... are also not checked, thrown, or returned by default.
    
    NotPassed( p... ) can be modified by adding +Checked(f), +Thrown(), or +Returned(),
    so that it actually has some effect.
    
    ExceptionToThrow( p... ) is equivalent to NotPassed( p... ) + Thrown(),
    and is the primary use of this group.  It is usually used with one parameter,
    and causes the exception thrown on failure to be derived from p... .
*/

namespace PlusPlus
   {
    template < class FailureChecker, bool thrown, bool returned, class... P >
    class UnpassedParameterGroup
       {
        private:
            using ReferenceTuple      = std::tuple< P&&... >;
            using ConstReferenceTuple = std::tuple< const typename std::remove_reference<P>::type&... >;

            using SelectForThrowing  = ThrowSelector<  thrown >;
            using SelectForReturning = ReturnSelector< returned >;
            
            FailureChecker checkFailure;
            ReferenceTuple parameters;
            
        public:
            UnpassedParameterGroup( FailureChecker f, ReferenceTuple p )
               : checkFailure( std::move(f) ), 
                 parameters( std::move( p ) )
               {}
            
            std::tuple<> PassedParts() const                { return std::tuple<>(); }
            
            bool CheckForFailure() const                    { return stdish::apply( checkFailure, ConstReferenceTuple( parameters ) ); }
            
            auto ThrownParts()      -> decltype( SelectForThrowing()(  parameters ) )    { return SelectForThrowing()(  std::move( parameters ) ); }
            auto ReturnedParts()    -> decltype( SelectForReturning()( parameters ) )    { return SelectForReturning()( std::move( parameters ) ); }

            
            template < class C > using WithDifferentChecker  = UnpassedParameterGroup< C,              thrown, returned, P... >;
            template < bool t >  using WithDifferentThrown   = UnpassedParameterGroup< FailureChecker, t,      returned, P... >;
            template < bool r >  using WithDifferentReturned = UnpassedParameterGroup< FailureChecker, thrown, r,        P... >;

            template < class C >
            friend WithDifferentChecker<C> operator+( UnpassedParameterGroup original, CheckFailureWith<C> checker )
               {
                return WithDifferentChecker<C>( std::move( checker ).Checker(), std::move( original.parameters ) );
               }
            
            template < bool t >
            friend WithDifferentThrown<t> operator+( UnpassedParameterGroup original, ThrowSelector<t> )
               {
                return WithDifferentThrown<t>( std::move( original.checkFailure ), std::move( original.parameters ) );
               }
            
            template < bool r >
            friend WithDifferentReturned<r> operator+( UnpassedParameterGroup original, ReturnSelector<r> )
               {
                return WithDifferentReturned<r>( std::move( original.checkFailure ), std::move( original.parameters ) );
               }
       };
    
    
    
    template < class... P >
    UnpassedParameterGroup< NeverFails, false, false, P... >
    NotPassed( P&&... p )
       {
        return UnpassedParameterGroup< NeverFails, false, false, P... >( NeverFails(), std::forward_as_tuple( std::forward<P>(p)... ) );
       }
    
    template < class... P >
    UnpassedParameterGroup< NeverFails, true, false, P... >
    ExceptionToThrow( P&&... p )
       {
        return UnpassedParameterGroup< NeverFails, true, false, P... >( NeverFails(), std::forward_as_tuple( std::forward<P>(p)... ) );
       }
   }

#endif
