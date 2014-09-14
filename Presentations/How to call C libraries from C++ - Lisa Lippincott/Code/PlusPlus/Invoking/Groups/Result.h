//
//  Result.h
//  PlusPlus
//
//  Created by Lisa Lippincott on 8/20/14.
//  Released into the public domain by Lisa Lippincott, 2014.
//

#ifndef PLUSPLUS_RESULT_H
#define PLUSPLUS_RESULT_H

#include "Checked.h"
#include "Thrown.h"
#include "Returned.h"

/*
    Result<R>() produces a result group that expects a result of type R.
    
    The result is returned by default, but this can be changed by adding +NotReturned().
    
    Theresult is not checked or thrown by default, but the group can be modified 
    (by +Checked(f) or +Thrown()) to be checked or thrown.

    NoResult() produces a result group that expects no result.  It can't be modified.

    FailureFlagResult<R>()  is equivalent to Result<R>() + Checked( AnyIsTrue() )  + NotReturned().
    SuccessFlagResult<R>()  is equivalent to Result<R>() + Checked( AnyIsFalse() ) + NotReturned().

    ErrorResult<R>()        is equivalent to Result<R>() + Checked( AnyIsTrue ) + NotReturned() + Thrown()
    ErrorResult<R>(f)       is equivalent to Result<R>() + Checked( f )         + NotReturned() + Thrown()
*/

namespace PlusPlus
   {
    template < class FailureChecker, bool thrown, bool returned, class R >
    class ResultGroup
       {
        private:
            using SelectForThrowing  = ThrowSelector<  thrown >;
            using SelectForReturning = ReturnSelector< returned >;
            
            FailureChecker checkFailure;
            
        public:
            explicit ResultGroup( FailureChecker f )
               : checkFailure( std::move(f) )
               {}
            
            const FailureChecker& Checker() const &         { return checkFailure; }
            FailureChecker&& Checker() &&                   { return std::move( checkFailure ); }

            using ResultType = R;
            
            bool CheckForFailure( const R& r ) const        { return checkFailure( r ); }
            
            auto ThrownParts( R& r )    -> decltype( SelectForThrowing()(  std::make_tuple( std::move(r) ) ) )    { return SelectForThrowing()(  std::make_tuple( std::move(r) ) ); }
            auto ReturnedParts( R& r )  -> decltype( SelectForReturning()( std::make_tuple( std::move(r) ) ) )    { return SelectForReturning()( std::make_tuple( std::move(r) ) ); }
       };



    template < class OldChecker, class FailureChecker, bool thrown, bool returned, class... P >
    ResultGroup< FailureChecker, thrown, returned, P... >
    operator+( ResultGroup< OldChecker, thrown, returned, P... >, CheckFailureWith<FailureChecker> checker )
       {
        return ResultGroup< FailureChecker, thrown, returned, P... >( std::move( checker ).Checker() );
       }
    
    template < class FailureChecker, bool wasThrown, bool thrown, bool returned, class... P >
    ResultGroup< FailureChecker, thrown, returned, P... >
    operator+( ResultGroup< FailureChecker, wasThrown, returned, P... > original, ThrowSelector<thrown> )
       {
        return ResultGroup< FailureChecker, thrown, returned, P... >( original.Checker() );
       }

    template < class FailureChecker, bool thrown, bool wasReturned, bool returned, class... P >
    ResultGroup< FailureChecker, thrown, returned, P... >
    operator+( ResultGroup< FailureChecker, thrown, wasReturned, P... > original, ReturnSelector<returned> )
       {
        return ResultGroup< FailureChecker, thrown, returned, P... >( original.Checker() );
       }



    template < class FailureChecker >
    class NoResultGroup
       {
        private:
            FailureChecker checkFailure;
            
        public:
            explicit NoResultGroup( FailureChecker f )
               : checkFailure( std::move(f) )
               {}
            
            const FailureChecker& Checker() const &         { return checkFailure; }
            FailureChecker&& Checker() &&                   { return std::move( checkFailure ); }

            using ResultType = void;
            
            bool CheckForFailure() const                    { return checkFailure(); }
            
            std::tuple<> ThrownParts()                      { return std::tuple<>(); }
            std::tuple<> ReturnedParts()                    { return std::tuple<>(); }
       };



    template < class OldChecker, class FailureChecker >
    NoResultGroup< FailureChecker >
    operator+( NoResultGroup< OldChecker >, CheckFailureWith<FailureChecker> checker )
       {
        return NoResultGroup< FailureChecker >( std::move( checker ).Checker() );
       }
    
    
    
    template < class R >
    ResultGroup< NeverFails, false, true, R >
    Result()
       {
        return ResultGroup< NeverFails, false, true, R >( NeverFails() );
       }
    
    inline NoResultGroup< NeverFails >
    NoResult()
       {
        return NoResultGroup< NeverFails >( NeverFails() );
       }

    template < class R >
    ResultGroup< AnyIsTrue, false, false, R >
    FailureFlagResult()
       {
        return ResultGroup< AnyIsTrue, false, false, R >( AnyIsTrue() );
       }

    template < class R >
    ResultGroup< AnyIsFalse, false, false, R >
    SuccessFlagResult()
       {
        return ResultGroup< AnyIsFalse, false, false, R >( AnyIsFalse() );
       }

    template < class R >
    ResultGroup< AnyIsTrue, true, false, R >
    ErrorResult()
       {
        return ResultGroup< AnyIsTrue, true, false, R >( AnyIsTrue() );
       }

    template < class R, class FailureChecker >
    ResultGroup< FailureChecker, true, false, R >
    ErrorResult( FailureChecker failureChecker )
       {
        return ResultGroup< FailureChecker, true, false, R >( std::move( failureChecker ) );
       }
   }

#endif

