//
//  Checked.h
//  PlusPlus
//
//  Created by Lisa Lippincott on 8/16/14.
//  Released into the public domain by Lisa Lippincott, 2014.
//

#ifndef PLUSPLUS_CHECKED_H
#define PLUSPLUS_CHECKED_H

#include <utility>

/*
    The items in this file are used to modify the basic parameter and result groups 
    to add checks for failure.  So, taking Out<T...>() as a base group,
    
        Out<T...>() + FailsWhenTrue()       fails when any member of the group, converted to bool, is true
        Out<T...>() + FailsWhenFalse()      fails when any member of the group, converted to bool, is false
        Out<T...>() + FailsWhen( f )        fails when f( outputs... ), converted to bool, is true
        Out<T...>() + SucceedsWhen( f )     fails when f( outputs... ), converted to bool, is false
*/

namespace PlusPlus
   {
    struct NeverFails
       {
        template < class... P >
        bool operator()( const P&... ) const                    { return false; }
       };

    struct AnyIsTrue
       {
        bool operator()() const                                 { return false; }
        
        template < class P0, class... P >
        bool operator()( const P0& p0, const P&... p ) const    { return static_cast<bool>(p0) || operator()( p... ); }
       };

    struct AnyIsFalse
       {
        bool operator()() const                                 { return false; }
        
        template < class P0, class... P >
        bool operator()( const P0& p0, const P&... p ) const    { return !static_cast<bool>(p0) || operator()( p... ); }
       };
    
    
    
    template < class SuccessFunctor >
    class DoesNotSucceed
       {
        private:
            SuccessFunctor success;
        
        public:
            DoesNotSucceed( SuccessFunctor s )                  : success( s ) {}

            template < class... P >
            bool operator()( const P&... p ) const              { return !success( p... ); }
       };
    
    template < class SuccessFunctor >
    DoesNotSucceed< SuccessFunctor > NotSuccessful( SuccessFunctor s )
       {
        return DoesNotSucceed< SuccessFunctor >( s );
       }
    
    
    
    template < class FailureFunctor >
    class CheckFailureWith
       {
        private:
            FailureFunctor checker;
        
        public:
            CheckFailureWith( FailureFunctor f )                : checker( f ) {}
            
            const FailureFunctor& Checker() const &             { return checker; }
            FailureFunctor&& Checker() &&                       { return std::move( checker ); }
       };
    
    
    
    template < class FailureFunctor >
    CheckFailureWith< FailureFunctor > FailsWhen( FailureFunctor f )
       {
        return CheckFailureWith< FailureFunctor >( std::move(f) );
       }
    
    template < class SuccessFunctor >
    CheckFailureWith< DoesNotSucceed<SuccessFunctor> > SucceedsWhen( SuccessFunctor s )
       {
        return FailsWhen( NotSuccessful( std::move(s) ) );
       }
    
    inline CheckFailureWith< AnyIsTrue > FailsWhenTrue()
       {
        return FailsWhen( AnyIsTrue() );
       }
    
    inline CheckFailureWith< AnyIsFalse > FailsWhenFalse()
       {
        return FailsWhen( AnyIsFalse() );
       }
   }

#endif
