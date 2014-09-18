//
//  InvokeWithGroups.h
//  PlusPlus
//
//  Created by Lisa Lippincott on 8/20/14.
//  Released into the public domain by Lisa Lippincott, 2014.
//

#ifndef PLUSPLUS_INVOKEWITHGROUPS_H
#define PLUSPLUS_INVOKEWITHGROUPS_H

#include "integer_sequence.h"

#include <utility>
#include <tuple>
#include <stdexcept>

/***
    InvokeWithGroups calls a function, rearranging its parameters, errors, and results 
    according to parameter and result groups:
        
        The passed parts of the parameter groups are concatenated into a parameter list for the invoked function.
        The function is invoked.
        If any group fails, the thrown parts of all groups are concatenated into a parameter list and passed to the failure functor.
        Otherwise, the returned parts of all groups are concatenated into a result tuple.
            If the result tuple has no members, the function result is void.
            If the result tuple has exactly one member, the function result is that member.
            If the result tuple has more than one member, the function result is the result tuple.
    
        When concatenating parts, parts from the result group come first, them parts from the parameter groups in order.
    
    A parameter group is a class that provides four operations:
    
        group.PassedParts()         -- returns a tuple of parameters (often references) to be passed in the invocation
        group.CheckForFailure()     -- returns bool; if true, InvokeWithGroups will exit with an exception.
        group.ThrownParts()         -- returns a tuple of base class values for the exception.  Leaves the group with an unspecified value.
        group.ReturnedParts()       -- returns a tuple of items to be returned.  Also leaves the group with an unspecified value.
    
    A result group is a similar class that handles the function result, where r has type GroupType::ResultType.
    If ResultType is void, no parameter is passed where r appears.
    
        typename GroupType::ResultType  -- a type used to hold the result of the function invocation
        group.CheckForFailure( r )      -- returns bool; if true, InvokeWithGroups will exit with an exception.
        group.ThrownParts( r )          -- returns a tuple of base class values for the exception.  Leaves r and the group with an unspecified value.
        group.ReturnedParts( r )        -- returns a tuple of items to be returned.  Also leaves r and the group with an unspecified value.
    
    Some basic parameter groups are provided in In.h, Out.h, and InOut.h.
    A basic result group is provided by Result.h.
***/

namespace PlusPlus
   {
    inline bool AnyParameterGroupFailed()
       {
        return false;
       }

    template < class FirstGroup, class... MoreGroups >
    bool AnyParameterGroupFailed( const FirstGroup& first, const MoreGroups&... more )
       {
        return first.CheckForFailure() || AnyParameterGroupFailed( more... );
       }


    inline                                    void                     Detuple( std::tuple<            > )       {}
    template < class A >                      A                        Detuple( std::tuple< A          > t )     { return std::move( std::get<0>(t) ); }
    template < class A, class B, class... C > std::tuple< A, B, C... > Detuple( std::tuple< A, B, C... > t )     { return t; }



    template < class... Bases >
    struct InvokeWithGroupsFailed: Bases...
       {
        explicit InvokeWithGroupsFailed( Bases... b )                   : Bases( std::move(b) )... {}
       };

    struct ThrowInvokeWithGroupsFailed
       {
        template < class... Bases >
        [[noreturn]] void operator()( Bases&&... b ) const
           {
            using Exception = InvokeWithGroupsFailed< typename std::decay<Bases>::type... >;
            throw Exception( std::forward<Bases>(b)... );
           }
       };



    template < class InnerResultType >
    struct InvokeWithGroupsInvoker
       {
        template < class ResultGroup, class Callable, class... ParameterGroups >
        auto operator()( ResultGroup resultGroup, Callable&& function, ParameterGroups... parameterGroups ) const
        -> decltype( Detuple( std::tuple_cat( resultGroup.ReturnedParts( std::declval<InnerResultType&>() ), parameterGroups.ReturnedParts()... ) ) )
           {
            InnerResultType result = stdish::apply( std::forward<Callable>(function), std::tuple_cat( parameterGroups.PassedParts()... ) );
            
            if ( resultGroup.CheckForFailure( result ) || AnyParameterGroupFailed( parameterGroups... ) )
                stdish::apply( ThrowInvokeWithGroupsFailed(), std::tuple_cat( resultGroup.ThrownParts( result ), parameterGroups.ThrownParts()... ) );
            
            return Detuple( std::tuple_cat( resultGroup.ReturnedParts( result ), parameterGroups.ReturnedParts()... ) );
           }
       };

    template <>
    struct InvokeWithGroupsInvoker<void>
       {
        template < class ResultGroup, class Callable, class... ParameterGroups >
        auto operator()( ResultGroup resultGroup, Callable&& function, ParameterGroups... parameterGroups ) const
        -> decltype( Detuple( std::tuple_cat( resultGroup.ReturnedParts(), parameterGroups.ReturnedParts()... ) ) )
           {
            stdish::apply( std::forward<Callable>(function), std::tuple_cat( parameterGroups.PassedParts()... ) );
            
            if ( resultGroup.CheckForFailure() || AnyParameterGroupFailed( parameterGroups... ) )
                stdish::apply( ThrowInvokeWithGroupsFailed(), std::tuple_cat( resultGroup.ThrownParts(), parameterGroups.ThrownParts()... ) );
            
            return Detuple( std::tuple_cat( resultGroup.ReturnedParts(), parameterGroups.ReturnedParts()... ) );
           }
       };


    template < class ResultGroup, class Callable, class... ParameterGroups >
    auto InvokeWithGroups( ResultGroup&& resultGroup, Callable&& function, ParameterGroups&&... parameterGroups )
    -> decltype( InvokeWithGroupsInvoker< typename ResultGroup::ResultType >()( std::forward< ResultGroup     >( resultGroup ),
                                                                                std::forward< Callable        >( function ),
                                                                                std::forward< ParameterGroups >( parameterGroups )... ) )
       {
        return   InvokeWithGroupsInvoker< typename ResultGroup::ResultType >()( std::forward< ResultGroup     >( resultGroup ),
                                                                                std::forward< Callable        >( function ),
                                                                                std::forward< ParameterGroups >( parameterGroups )... );
       }
   }

#endif
