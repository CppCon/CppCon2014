//
//  PointerToValue.h
//  PlusPlus
//
//  Created by Lisa Lippincott on 7/11/14.
//  Released into the public domain by Lisa Lippincott, 2014.
//

#ifndef PLUSPLUS_POINTERLIKE_H
#define PLUSPLUS_POINTERLIKE_H

#include <cstddef>
#include <utility>

/*
    A PointerToValue is like a pointer, but points to a value it contains, rather than to an object.
    Two PointersToValues, a and b, are equal if *a == *b. PointersToValues are unlike pointers 
    in that it does not follow that &*a = &*b.  The PointersToValues don't allow non-const
    access to *a and *b, so the fact that they are different objects has little impact otherwise.

    Unlike ordinary pointer types, one PointerToValue type may be converted to another PointerToValue 
    type if the value type can be converterd.
    
    An PointerToValue is null if its contents are equal to a value-constructed instance of the value type.

    The primary use of an PointerToValue is as the pointer type in the deleter of a unique_ptr.
    This allows a resource that isn't represented by a pointer (e.g., a POSIX socket, represented 
    by an int) to be managed without adding another heap block to manage.
    
    Note that unique_ptrs that use PointersToValues should always point to const types:
    std::unique_ptr< const Foo, Deleter >.
    
    To satisfy the requirements of NullablePointer, and thus be usable in a deleter, the 
    ContentType must be EqualityComparable, DefaultConstructible, CopyConstructible, 
    CopyAssignable, Destructible, and Swappable, all without throwing exceptions.
*/

namespace PlusPlus
   {
    template < class TheValueType >
    class PointerToValue
       {
        public:
            using ValueType = TheValueType;
           
        private:
            ValueType value;
        
        public:
            PointerToValue()                                                            : value() {}
            PointerToValue( std::nullptr_t )                                            : value() {}
           
            explicit PointerToValue( ValueType c )                                      : value( std::move( c ) ) {}
        
            template < class Other >
            PointerToValue( const PointerToValue<Other>& p )                            : value( *p ) {}
            
            explicit operator bool() const                                              { return !( value == ValueType() ); }
           
            const ValueType& operator*() const                                          { return value; }
            const ValueType *operator->() const                                         { return value; }
           
            friend bool operator==( const PointerToValue& a, const PointerToValue& b )  { return *a == *b; }
            friend bool operator!=( const PointerToValue& a, const PointerToValue& b )  { return !( a == b ); }
       };
   }

#endif
