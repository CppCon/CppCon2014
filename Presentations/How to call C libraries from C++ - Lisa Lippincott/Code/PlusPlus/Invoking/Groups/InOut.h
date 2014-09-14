//
//  InOut.h
//  PlusPlus
//
//  Created by Lisa Lippincott on 8/17/14.
//  Released into the public domain by Lisa Lippincott, 2014.
//

#ifndef PLUSPLUS_INOUT_H
#define PLUSPLUS_INOUT_H

#include "Checked.h"
#include "Thrown.h"
#include "Returned.h"
#include "integer_sequence.h"

#include <tuple>

/*
    InOut( p... ) produces an input/output parameter group.
    
    InOut( p... ) passes p... as non-const lvalue references.
    The parameters p... are not checked, thrown, or returned by default,
    but this can be modified by adding +Checked(f), +Thrown(), or +Returned().
*/

namespace PlusPlus
   {
    template < class FailureChecker, bool thrown, bool returned, class... P >
    class InOutParameterGroup
       {
        private:
            using ReferenceTuple      = std::tuple< typename std::decay<P>::type&... >;
            using ConstReferenceTuple = std::tuple< const typename std::decay<P>::type&... >;

            using SelectForThrowing  = ThrowSelector<  thrown >;
            using SelectForReturning = ReturnSelector< returned >;
            
            FailureChecker checkFailure;
            ReferenceTuple parameters;
            
        public:
            InOutParameterGroup( FailureChecker f, ReferenceTuple p )
               : checkFailure( std::move(f) ), 
                 parameters( p )
               {}
            
            ReferenceTuple PassedParts() const              { return parameters; }
            
            bool CheckForFailure() const                    { return stdish::apply( checkFailure, ConstReferenceTuple( parameters ) ); }
            
            auto ThrownParts()      -> decltype( SelectForThrowing()(  parameters ) )    { return SelectForThrowing()(  parameters ); }
            auto ReturnedParts()    -> decltype( SelectForReturning()( parameters ) )    { return SelectForReturning()( parameters ); }


            template < class C > using WithDifferentChecker  = InOutParameterGroup< C,              thrown, returned, P... >;
            template < bool t >  using WithDifferentThrown   = InOutParameterGroup< FailureChecker, t,      returned, P... >;
            template < bool r >  using WithDifferentReturned = InOutParameterGroup< FailureChecker, thrown, r,        P... >;

            template < class C >
            friend WithDifferentChecker<C> operator+( InOutParameterGroup original, CheckFailureWith<C> checker )
               {
                return WithDifferentChecker<C>( std::move( checker ).Checker(), std::move( original.parameters ) );
               }
            
            template < bool t >
            friend WithDifferentThrown<t> operator+( InOutParameterGroup original, ThrowSelector<t> )
               {
                return WithDifferentThrown<t>( std::move( original.checkFailure ), std::move( original.parameters ) );
               }
            
            template < bool r >
            friend WithDifferentReturned<r> operator+( InOutParameterGroup original, ReturnSelector<r> )
               {
                return WithDifferentReturned<r>( std::move( original.checkFailure ), std::move( original.parameters ) );
               }
       };

    
    
    inline void InOutRequiresNonConstReferences()
       {}
    
    template < class P0, class... P >
    void InOutRequiresNonConstReferences( P0&&, P&&... p )
       {
        static_assert( !std::is_const< typename std::remove_reference<P0&&>::type >::value, "Argument to InOut is constant" );
        InOutRequiresNonConstReferences( std::forward<P>( p )... );
       }
    
    
    template < class... P >
    InOutParameterGroup< NeverFails, false, false, P... >
    InOut( P&... p )
       {
        InOutRequiresNonConstReferences( p... );
        using ReferenceTuple = std::tuple< typename std::decay<P>::type&... >;
        return InOutParameterGroup< NeverFails, false, false, P... >( NeverFails(), ReferenceTuple( p... ) );
       }
   }

#endif
