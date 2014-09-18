//
//  In.h
//  PlusPlus
//
//  Created by Lisa Lippincott on 8/16/14.
//  Released into the public domain by Lisa Lippincott, 2014.
//

#ifndef PLUSPLUS_IN_H
#define PLUSPLUS_IN_H

#include "Checked.h"
#include "Thrown.h"
#include "Returned.h"
#include "integer_sequence.h"

#include <tuple>

/*
    In( p... ) produces an input parameter group.
    
    In( p... ) passes p... as either const P& or P&&, using P&& where p is a non-const rvalue.
    The parameters p... are not checked, thrown, or returned by default.
    
    If modified (by +Checked(f), +Thrown(), or +Returned()) to access parameters after the function call, 
    all parameters are passed as const P&.  P&& is then used for throwing and returning.
    
    When putting owned resources (i.e., unique_ptrs) in In groups, remember to move or forward them,
    because const references can't be released.
*/

namespace PlusPlus
   {
    template < class FailureChecker, bool thrown, bool returned, class... P >
    class InParameterGroup
       {
        private:
            using ReferenceTuple      = std::tuple< P&&... >;
            using ConstReferenceTuple = std::tuple< const typename std::remove_reference<P>::type&... >;

            using SelectForThrowing  = ThrowSelector<  thrown >;
            using SelectForReturning = ReturnSelector< returned >;
            
            FailureChecker checkFailure;
            ReferenceTuple parameters;
            
        public:
            InParameterGroup( FailureChecker f, ReferenceTuple p )
               : checkFailure( std::move(f) ), 
                 parameters( std::move( p ) )
               {}
            
            ConstReferenceTuple PassedParts() const         { return parameters; }
            
            bool CheckForFailure() const                    { return stdish::apply( checkFailure, ConstReferenceTuple( parameters ) ); }
            
            auto ThrownParts()      -> decltype( SelectForThrowing()(  parameters ) )    { return SelectForThrowing()(  std::move( parameters ) ); }
            auto ReturnedParts()    -> decltype( SelectForReturning()( parameters ) )    { return SelectForReturning()( std::move( parameters ) ); }


            template < class C > using WithDifferentChecker  = InParameterGroup< C,              thrown, returned, P... >;
            template < bool t >  using WithDifferentThrown   = InParameterGroup< FailureChecker, t,      returned, P... >;
            template < bool r >  using WithDifferentReturned = InParameterGroup< FailureChecker, thrown, r,        P... >;

            template < class C >
            friend WithDifferentChecker<C> operator+( InParameterGroup original, CheckFailureWith<C> checker )
               {
                return WithDifferentChecker<C>( std::move( checker ).Checker(), std::move( original.parameters ) );
               }
            
            template < bool t >
            friend WithDifferentThrown<t> operator+( InParameterGroup original, ThrowSelector<t> )
               {
                return WithDifferentThrown<t>( std::move( original.checkFailure ), std::move( original.parameters ) );
               }
            
            template < bool r >
            friend WithDifferentReturned<r> operator+( InParameterGroup original, ReturnSelector<r> )
               {
                return WithDifferentReturned<r>( std::move( original.checkFailure ), std::move( original.parameters ) );
               }
       };

    template < class... P >
    class InParameterGroup< NeverFails, false, false, P... >            // In the usual case, when not checked, thrown, or returned, forward rvalue references in PassedParts().
       {
        private:
            using ReferenceTuple = std::tuple< P&&... >;
            
            template < class R > using MakeLValueConst = typename std::conditional< std::is_lvalue_reference<R>::value,
                                                                                    const typename std::remove_reference<R>::type&,
                                                                                    R >::type;
            
            using TupleToPass = std::tuple< MakeLValueConst<P&&>... >;
            
            ReferenceTuple parameters;
            
        public:
            InParameterGroup( NeverFails, ReferenceTuple p )
               : parameters( std::move( p ) )
               {}
            
            TupleToPass PassedParts() const                 { return std::move( parameters ); }
            
            bool CheckForFailure() const                    { return false; }
            
            std::tuple<> ThrownParts()                      { return std::tuple<>(); }
            std::tuple<> ReturnedParts()                    { return std::tuple<>(); }


            template < class C > using WithDifferentChecker  = InParameterGroup< C,          false, false, P... >;
            template < bool t >  using WithDifferentThrown   = InParameterGroup< NeverFails, t,     false, P... >;
            template < bool r >  using WithDifferentReturned = InParameterGroup< NeverFails, false, r,     P... >;

            template < class C >
            friend WithDifferentChecker<C> operator+( InParameterGroup original, CheckFailureWith<C> checker )
               {
                return WithDifferentChecker<C>( std::move( checker ).Checker(), std::move( original.parameters ) );
               }
            
            template < bool t >
            friend WithDifferentThrown<t> operator+( InParameterGroup original, ThrowSelector<t> )
               {
                return WithDifferentThrown<t>( NeverFails(), std::move( original.parameters ) );
               }
            
            template < bool r >
            friend WithDifferentReturned<r> operator+( InParameterGroup original, ReturnSelector<r> )
               {
                return WithDifferentReturned<r>( NeverFails(), std::move( original.parameters ) );
               }
       };

    
    
    template < class... P >
    InParameterGroup< NeverFails, false, false, P... >
    In( P&&... p )
       {
        return InParameterGroup< NeverFails, false, false, P... >( NeverFails(), std::forward_as_tuple( std::forward<P>(p)... ) );
       }
   }

#endif
