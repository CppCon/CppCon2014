//
//  Invoke.h
//  PlusPlus
//
//  Created by Lisa Lippincott on 8/28/14.
//  Released into the public domain by Lisa Lippincott, 2014.
//

#ifndef PLUSPLUS_INVOKE_H
#define PLUSPLUS_INVOKE_H

#include "InvokeWithGroups.h"
#include "Prefixed.h"
#include "Conjugated.h"
#include "temporary_reference_wrapper.h"

#include <utility>

/*
    PlusPlus::Invoke calls a function, performing the steps necessary to adjust to its calling convention.
    Because there are so many steps to the adjustment, it's perhaps the only function in PlusPlus that is 
    really complicated.  These are the steps, grouped by the function that handles them:
    
    InvokeWithGroups (entering)
        Collect references to the items to be passed to the internal parameters.
    
    Conjugated with Wrapper (outer, entering)
        Unwrap the passed parameters.  (This unwrapping may exit with an exception.)
    
    Conjugated with Seizer (entering)
        Release any input resources.  (Starting here, only the invoked function may exit with an exception.)
    
    Conjugated with Wrapper (inner, entering)
        Unwrap any input resources.
    
    Prefixed with Forwarder
        Make pointers to parameters that the internal calling convention passes by pointer.
    
    The invoked function
        Does whatever it does.  May throw exceptions, provided it is responsible about the resources.
    
    Conjugated with Wrapper (inner, exiting)
        Wrap any output resources.
    
    Conjugated with Seizer (exiting)
        Seize any output resources.  (After this step exceptions may be thrown again.)

    Conjugated with Wrapper (outer, exiting)
        Wrap the outputs.  (This wrapping may exit with an exception.)
    
    InvokeWithGroups (exiting)
        Test the groups for failure.
        On failure, collect and throw the thrown parts of the groups.
        On success, return the returned parts of the groups.
    
    
    For a full explanation of parameter and result groups, see InvokeWithGroups.h.
    Some basic parameter groups are created by these functions from In.h, InOut.h, Out.h, and NotPassed.h:
    
        In( references )                          -- input parameters, passed by const or rvalue reference.
        InOut( nonconst lvalue references )       -- input/output parameters, passed by reference.
        Out<Types>()                              -- output parameters, value constructed, passed by reference, and returned.
        OutError<Types>()                         -- output errors: value constructed, passed by reference, checked (fails when true), and thrown.
        OutError<Types>( FailureTest )            -- output errors: value constructed, passed by reference, checked (fails when test returns true), and thrown.
        ExceptionToThrow( references )            -- not passed, just thrown on failure.

    And basic result groups are created by these functions from Result.h:
    
        NoResult()                                -- ignore the result of the invoked function
        Result<Type>()                            -- Seize/wrap the result of the invoked function as the given type, and return it.
        ErrorResult<Type>()                       -- Seize/wrap the result as Type, check for failure (fail when true), and throw it.
        ErrorResult<Type>( FailureTest )          -- Seize/wrap the result as Type, check for failure (fail when test returns true), and throw it.
*/

namespace PlusPlus
   {
    // When the result group asks for a reference, ask the conjugated functions for references.
    // The conjugated functions check to make sure no temporary they create is returned by reference.
    
    template < template <class> class FunctorFamily, class Desired >
    struct TypeThatWrapsTo_Specializer
       {
        using Wrapper          = FunctorFamily< typename std::decay< Desired >::type >;
        using ResultOfUnwrap   = decltype( Wrapper().Inverse( std::declval<Desired>() ) );
        using Unwrapped        = typename std::decay< ResultOfUnwrap >::type;
        
        using type = Unwrapped;
       };
    
    template < template <class> class FunctorFamily, class Desired >
    struct TypeThatWrapsTo_Specializer< FunctorFamily, Desired& >
       {
        using Wrapper          = FunctorFamily< typename std::decay< Desired& >::type >;
        using ResultOfUnwrap   = decltype( Wrapper().Inverse( std::declval<Desired&>() ) );
        using Unwrapped        = typename std::decay< ResultOfUnwrap >::type;
        
        using type = Unwrapped&;
       };
    
    template < template <class> class FunctorFamily, class Desired >
    struct TypeThatWrapsTo_Specializer< FunctorFamily, Desired&& >
       {
        using Wrapper          = FunctorFamily< typename std::decay< Desired&& >::type >;
        using ResultOfUnwrap   = decltype( Wrapper().Inverse( std::declval<Desired&&>() ) );
        using Unwrapped        = typename std::decay< ResultOfUnwrap >::type;
        
        using type = Unwrapped&&;
       };
    
    template < template <class> class FunctorFamily, class Desired >
    using TypeThatWrapsTo = typename TypeThatWrapsTo_Specializer< FunctorFamily, Desired >::type;
    
    
    template < template <class> class Wrapper,
               template <class> class Seizer,
               template <class> class Forwarder,
               class ResultGroup,
               class Callable,
               class... ParameterGroups >
    auto Invoke( ResultGroup&& resultGroup,
                 Callable&& function,
                 ParameterGroups&&... parameterGroups )
    -> decltype( InvokeWithGroups( std::forward< ResultGroup >( resultGroup ),
                                   Conjugate< typename ResultGroup::ResultType, Wrapper >(
                                       Conjugate< TypeThatWrapsTo< Wrapper, typename ResultGroup::ResultType >, Seizer >(
                                           Conjugate< TypeThatWrapsTo< Seizer, TypeThatWrapsTo< Wrapper, typename ResultGroup::ResultType > >, Wrapper >(
                                               Prefix< Forwarder >(
                                                   stdish::temporary_ref( std::forward<Callable>( function ) ) ) ) ) ),
                                   std::forward< ParameterGroups >( parameterGroups )... ) )
       {
        return   InvokeWithGroups( std::forward< ResultGroup >( resultGroup ),
                                   Conjugate< typename ResultGroup::ResultType, Wrapper >(
                                       Conjugate< TypeThatWrapsTo< Wrapper, typename ResultGroup::ResultType >, Seizer >(
                                           Conjugate< TypeThatWrapsTo< Seizer, TypeThatWrapsTo< Wrapper, typename ResultGroup::ResultType > >, Wrapper >(
                                               Prefix< Forwarder >(
                                                   stdish::temporary_ref( std::forward<Callable>( function ) ) ) ) ) ),
                                   std::forward< ParameterGroups >( parameterGroups )... );
       }
   }

#endif
