//
//  charlike.h
//  PlusPlus
//
//  Created by Lisa Lippincott on 9/5/14.
//  Released into the public domain by Lisa Lippincott, 2014.
//

#ifndef PLUSPLUS_CHARLIKE_H
#define PLUSPLUS_CHARLIKE_H

#include <type_traits>

/*
    is_charlike and charlike_cast handle the safe(ish) reinterpret casts between character types.
*/

namespace PlusPlus
   {    
    namespace stdish
       {
        template < class T > struct is_charlike: std::false_type {};

        template < class T > struct is_charlike< const T >: is_charlike<T> {};
        
        template <> struct is_charlike<          char >: std::true_type {};
        template <> struct is_charlike< signed   char >: std::true_type {};
        template <> struct is_charlike< unsigned char >: std::true_type {};
        
        
        
        template < class T > using is_charlike_pointer = std::integral_constant< bool, std::is_pointer<T>::value 
                                                                                       && is_charlike< typename std::remove_pointer<T>::type >::value >;
        
        
        
        template < class OutPointer, class InChar >
        auto charlike_cast( InChar *in )
        -> typename std::enable_if< is_charlike< InChar >::value && is_charlike_pointer< OutPointer >::value, OutPointer >::type
           {
            return reinterpret_cast< OutPointer >( in );
           }
       }
   }

#endif
