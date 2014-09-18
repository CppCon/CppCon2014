//
//  ForwardAsPointer.h
//  PlusPlus
//
//  Created by Lisa Lippincott on 8/28/14.
//  Released into the public domain by Lisa Lippincott, 2014.
//

#ifndef PLUSPLUS_FORWARDASPOINTER_H
#define PLUSPLUS_FORWARDASPOINTER_H

#include <utility>

// ForwardAsPointer<T> is forwarder functor that passes references to T as pointers.
// Most objects are passed pointer to themselves; arrays are passed as pointers
// to their first element.

namespace PlusPlus
   {
    template < class T >                T *       MakePointer( T&  t      )   { return &t; }
    template < class T >                T const * MakePointer( T&& t      )   { return &t; }
    template < class E >                E *       MakePointer( E (&a)[]   )   { return  a; }
    template < class E >                E const * MakePointer( E (&&a)[]  )   { return  a; }
    template < class E, std::size_t n > E *       MakePointer( E (&a)[n]  )   { return  a; }
    template < class E, std::size_t n > E const * MakePointer( E (&&a)[n] )   { return  a; }
    
    template < class T >
    struct ForwardAsPointer
       {
        auto operator()( T&& t ) const
        -> decltype( MakePointer( std::forward<T>(t) ) )
           {
            return   MakePointer( std::forward<T>(t) );
           }
       };
   }

#endif
