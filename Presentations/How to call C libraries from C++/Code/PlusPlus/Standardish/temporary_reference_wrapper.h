//
//  temporary_reference_wrapper.h
//  PlusPlus
//
//  Created by Lisa Lippincott on 8/30/14.
//  Released into the public domain by Lisa Lippincott, 2014.
//

#ifndef PLUSPLUS_STDISH_TEMPORARY_REFERENCE_WRAPPER_H
#define PLUSPLUS_STDISH_TEMPORARY_REFERENCE_WRAPPER_H

#include <utility>

/*
    temporary_reference_wrapper is a class modeled on std::reference_wrapper, 
    but it can hold either an lvalue or an rvalue reference.  (It holds an lvalue 
    reference when the template parameter is an lvalue reference.)
    
    temporary_reference_wrappers cannot be copied or assigned, but may be move constructed.
    
    The get member produces an lvalue reference unless the wrapper is an rvalue 
    and the contained reference is an rvalue reference.  This mimics the 
    "use it until you move it" behavior of non-reference objects.  Likewise, 
    the operator() forwards the reference when calles on an rvalue wrapper.
    
    PlusPlus uses temporary_reference_wrapper to ensure that invoked function 
    object rvalues will have their rvalue operator() called.
*/

namespace PlusPlus
   {
    namespace stdish
       {
        template < class T >
        class temporary_reference_wrapper
           {
            private:
                T&& ref;

            public:
                temporary_reference_wrapper( T&& r )                                            : ref( std::forward<T>(r) )         {}
            
                temporary_reference_wrapper( temporary_reference_wrapper&& s )                  : ref( std::forward<T>(s.ref) )     {}
            
                T&  get() const &                                                               { return ref; }
                T&& get()       &&                                                              { return std::forward<T>( ref ); }
                
                operator T&() const &                                                           { return ref; }
                operator T&&()      &&                                                          { return std::forward<T>( ref ); }
                
                template < class... P >
                auto operator()( P&&... p ) const
                -> decltype( ref( std::forward<P>(p)... ) )
                   { return  ref( std::forward<P>(p)... ); }

                template < class... P >
                auto operator()( P&&... p ) &&
                -> decltype( std::forward<T>(ref)( std::forward<P>(p)... ) )
                   { return  std::forward<T>(ref)( std::forward<P>(p)... ); }
           };
        
        
        
        template < class T >
        temporary_reference_wrapper<T> temporary_ref( T&& t )
           {
            return temporary_reference_wrapper<T>( std::forward<T>(t) );
           }
       }
   }

#endif
