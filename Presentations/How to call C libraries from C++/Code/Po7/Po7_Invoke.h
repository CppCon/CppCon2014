//
//  Po7_Invoke.h
//  PlusPlus
//
//  Created by Lisa Lippincott on 8/31/14.
//  Released into the public domain by Lisa Lippincott, 2014.
//

#ifndef PO7_INVOKE_H
#define PO7_INVOKE_H

#include "Po7_Basics.h"
#include "Forwarder.h"
#include "Invoke.h"
#include "GroupMakers.h"

namespace Po7
   {
    template < class T > struct Forwarder: PlusPlus::ForwardOutputsAndNonscalarsAsPointers<T> {};
    
    
    
    template < class R, class F, class... P >
    auto Invoke( R&& r, F&& f, P&&... p )
    -> decltype( PlusPlus::Invoke< Wrapper, Seizer, Forwarder >( std::forward<R>(r),
                                                                 std::forward<F>(f),
                                                                 std::forward<P>(p)... ) )
       {
        return   PlusPlus::Invoke< Wrapper, Seizer, Forwarder >( std::forward<R>(r),
                                                                 std::forward<F>(f),
                                                                 std::forward<P>(p)... );
       }
    
    using namespace PlusPlus::GroupMakers;
    
    
    inline std::system_error MakeSystemErrorFromErrno()
       {
        return std::system_error( Wrap< std::error_code >( errno ) );
       }
    
    struct ThrowErrorFromErrno
       {
        std::tuple<> PassedParts() const                    { return std::tuple<>(); }
        bool CheckForFailure() const                        { return false; }
        std::tuple< std::system_error > ThrownParts() const { return std::make_tuple( MakeSystemErrorFromErrno() ); }
        std::tuple<> ReturnedParts() const                  { return std::tuple<>(); }
       };

    struct CheckAndThrowErrorFromErrno
       {
        std::tuple<> PassedParts() const                    { return std::tuple<>(); }
        bool CheckForFailure() const                        { return errno != 0; }
        std::tuple< std::system_error > ThrownParts() const { return std::make_tuple( MakeSystemErrorFromErrno() ); }
        std::tuple<> ReturnedParts() const                  { return std::tuple<>(); }
       };
   }

#endif
