//
//  Forwarder.h
//  PlusPlus
//
//  Created by Lisa Lippincott on 8/24/14.
//  Released into the public domain by Lisa Lippincott, 2014.
//

#ifndef PLUSPLUS_FORWARDER_H
#define PLUSPLUS_FORWARDER_H

#include "ForwardAsPointer.h"
#include "ForwardAsNonconstPointer.h"

#include <type_traits>

/*
    A forwarder family is functor template that converts references into pointers as necessary to 
    match the calling conventions of an underlying library.  It's expected that each library using 
    PlusPlus will define one forwarder, and specialize it as necessary.
    
    Because forwarders are used after resources have been released from their ownership classes,
    forwarders must not throw exceptions.
    
    This file provides several starting points, intended to be used as base classes for a library's 
    forwarder, like this:
    
    namespace UnderlyingLibrary_PlusPlus
       {
        template < class W > struct Forwarder: PlusPlus::DefaultForwarder<W> {};
       }
    
    
    
    The starting points are:
    
        DefaultForwarder                                Forward all references as references.
        
        ForwardOutputsAsPointers                        Forward non-const lvalues as pointers.
    
        ForwardOutputsAndNonscalarsAsPointers           Forward non-const lvalues and all references to nonscalar types
                                                        as pointers.
        
        ForwardOutputsAndNonscalarsAsNonconstPointers   Forward non-const lvalues and all references to nonscalar types
                                                        as pointers, casting away constness. This, of course, is dangerous.
*/

namespace PlusPlus
   {
    template < class T >
    struct DefaultForwarder
       {
        T&& operator()( T&& t ) const                           { return std::forward<T>( t ); }
       };
    
        
    
    template < class T, bool isOutput = std::is_lvalue_reference<T&&>::value
                                        && !std::is_const< typename std::remove_reference<T&&>::type >::value >
    struct ForwardOutputsAsPointers;

    template < class T > struct ForwardOutputsAsPointers< T, false >: DefaultForwarder<T> {};
    template < class T > struct ForwardOutputsAsPointers< T, true  >: ForwardAsPointer<T> {};

    
    
    template < class T, bool isScalar = std::is_scalar< typename std::remove_reference<T&&>::type >::value >
    struct ForwardOutputsAndNonscalarsAsPointers;
    
    template < class T > struct ForwardOutputsAndNonscalarsAsPointers< T, false >: ForwardAsPointer<T>         {};
    template < class T > struct ForwardOutputsAndNonscalarsAsPointers< T, true  >: ForwardOutputsAsPointers<T> {};

    
    
    template < class T, bool isScalar = std::is_scalar< typename std::remove_reference<T&&>::type >::value >
    struct ForwardOutputsAndNonscalarsAsNonconstPointers;
    
    template < class T > struct ForwardOutputsAndNonscalarsAsNonconstPointers< T, false >: ForwardAsNonconstPointer<T> {};
    template < class T > struct ForwardOutputsAndNonscalarsAsNonconstPointers< T, true  >: ForwardOutputsAsPointers<T> {};
   }

#endif
