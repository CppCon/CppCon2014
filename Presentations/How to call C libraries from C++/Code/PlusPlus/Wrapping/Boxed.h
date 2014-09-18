//
//  Boxed.h
//  PlusPlus
//
//  Created by Lisa Lippincott on 7/11/14.
//  Released into the public domain by Lisa Lippincott, 2014.
//

#ifndef PLUSPLUS_BOXED_H
#define PLUSPLUS_BOXED_H

#include <utility>
#include <iosfwd>

/*
    Boxed is a simple class wrapper for things that are like fundamental types, but need to not be fundamental types.
    Some reasons to use a Boxed type are:
        
        -- To provide type safety, where the underlying type is too general.
        -- To have the type more clearly express the intent, as with an exception.
        -- To have a different default value than the underlying type.
    
    The tag parameter to Boxed is a functor that, given no parameters, produced a default value for the Boxed object.
    The value type of the Boxed object is the result type of that function.  Other traits are optional:
    
        operator()()        -- the default value.  Also determines the value_type.
        
        hasEquality         -- an optional static bool constant; if true, the boxed type has operators == and !=
        hasComparison       -- an optional static bool constant; if true, the boxed type has operators >, <, >=, and <=
        hasBitwise          -- an optional static bool constant; if true, the boxed type has operators ~, |, &, ^, |=, &=, and ^=
        hasIncrement        -- an optional static bool constant; if true, the boxed type has operators ++ and --
        hasAddition         -- an optional static bool constant; if true, the boxed type has operators +, -, +=, and -=
        hasMultiplication   -- an optional static bool constant; if true, the boxed type has operators * (binary), /, %, *=, /= and %=, modeled on operations with units
        hasStreaming        -- an optional static bool constant; if true, the boxed type has operators << and >> for streams
        convertsToBool      -- an optional static bool constant; if true, the boxed type has an explicit conversion to bool.
    
    Most of the operators can be defined elsewhere as nonmember functions when the corresponding boolean constant is 
    missing or false; the declarations here have no effect in that case.  The exception is the conversion to bool;
    it can't be defined outside the class, and is always declared in the class, but the body of the conversion operator 
    can't be instantiated if convertsToBool is missing or false.
*/

namespace PlusPlus_Boxed
   {
    // This template gets its own namespace so that its instantiations are not associated with the PlusPlus namespace.
    // Instantiations are associated with the namespace of the tag type, but not the content type.
    
    template < class TheTag >
    class Boxed
       {
        public:
            using Tag       = TheTag;
            using ValueType = decltype( Tag()() );
           
        private:
            ValueType contents;
                    
        public:
            constexpr Boxed()                               : contents( Tag()() ) {}
        
            explicit constexpr Boxed( ValueType c )         : contents( std::move( c ) ) {}
        
            template < class OtherTag >
            constexpr Boxed( const Boxed< OtherTag >& b )   : contents( b.Get() )               { static_assert( std::is_convertible< OtherTag, Tag >::value, "Boxed types only convert when their tags do." ); }

            template < class OtherTag >
            constexpr Boxed( Boxed< OtherTag >&& b )        : contents( std::move(b).Get() )    { static_assert( std::is_convertible< OtherTag, Tag >::value, "Boxed types only convert when their tags do." ); }
           
            ValueType&  Get() &                             { return contents; }
            ValueType&& Get() &&                            { return std::move( contents ); }
            const ValueType& Get() const &                  { return contents; }
            
            explicit operator bool() const                  { static_assert( Tag::convertsToBool, "Not convertible to bool" ); return contents; }
       };
    
    template < class Tag > auto operator==( const Boxed<Tag>& a, const Boxed<Tag>& b ) -> typename std::enable_if< Tag::hasEquality,   bool        >::type   { return a.Get() == b.Get(); }
    template < class Tag > auto operator!=( const Boxed<Tag>& a, const Boxed<Tag>& b ) -> typename std::enable_if< Tag::hasEquality,   bool        >::type   { return a.Get() != b.Get(); }

    template < class Tag > auto operator< ( const Boxed<Tag>& a, const Boxed<Tag>& b ) -> typename std::enable_if< Tag::hasComparison, bool        >::type   { return a.Get() <  b.Get(); }
    template < class Tag > auto operator> ( const Boxed<Tag>& a, const Boxed<Tag>& b ) -> typename std::enable_if< Tag::hasComparison, bool        >::type   { return a.Get() >  b.Get(); }
    template < class Tag > auto operator<=( const Boxed<Tag>& a, const Boxed<Tag>& b ) -> typename std::enable_if< Tag::hasComparison, bool        >::type   { return a.Get() <= b.Get(); }
    template < class Tag > auto operator>=( const Boxed<Tag>& a, const Boxed<Tag>& b ) -> typename std::enable_if< Tag::hasComparison, bool        >::type   { return a.Get() >= b.Get(); }
    
    template < class Tag > auto operator~ ( const Boxed<Tag>& a )                      -> typename std::enable_if< Tag::hasBitwise,    Boxed<Tag>  >::type   { return Boxed<Tag>( ~a.Get() ); }
    template < class Tag > auto operator| ( const Boxed<Tag>& a, const Boxed<Tag>& b ) -> typename std::enable_if< Tag::hasBitwise,    Boxed<Tag>  >::type   { return Boxed<Tag>( a.Get() | b.Get() ); }
    template < class Tag > auto operator& ( const Boxed<Tag>& a, const Boxed<Tag>& b ) -> typename std::enable_if< Tag::hasBitwise,    Boxed<Tag>  >::type   { return Boxed<Tag>( a.Get() & b.Get() ); }
    template < class Tag > auto operator^ ( const Boxed<Tag>& a, const Boxed<Tag>& b ) -> typename std::enable_if< Tag::hasBitwise,    Boxed<Tag>  >::type   { return Boxed<Tag>( a.Get() ^ b.Get() ); }
    template < class Tag > auto operator|=(       Boxed<Tag>& a, const Boxed<Tag>& b ) -> typename std::enable_if< Tag::hasBitwise,    Boxed<Tag>& >::type   { a.Get() |= b.Get(); return a; }
    template < class Tag > auto operator&=(       Boxed<Tag>& a, const Boxed<Tag>& b ) -> typename std::enable_if< Tag::hasBitwise,    Boxed<Tag>& >::type   { a.Get() &= b.Get(); return a; }
    template < class Tag > auto operator^=(       Boxed<Tag>& a, const Boxed<Tag>& b ) -> typename std::enable_if< Tag::hasBitwise,    Boxed<Tag>& >::type   { a.Get() ^= b.Get(); return a; }
    
    template < class Tag > auto operator++(       Boxed<Tag>& a )                      -> typename std::enable_if< Tag::hasIncrement,  Boxed<Tag>& >::type   { ++a.Get(); return a; }
    template < class Tag > auto operator--(       Boxed<Tag>& a )                      -> typename std::enable_if< Tag::hasIncrement,  Boxed<Tag>& >::type   { ++a.Get(); return a; }
    template < class Tag > auto operator++(       Boxed<Tag>& a, int )                 -> typename std::enable_if< Tag::hasIncrement,  Boxed<Tag>  >::type   { return Boxed<Tag>( a.Get()++ ); }
    template < class Tag > auto operator--(       Boxed<Tag>& a, int )                 -> typename std::enable_if< Tag::hasIncrement,  Boxed<Tag>  >::type   { return Boxed<Tag>( a.Get()-- ); }
    
    template < class Tag > auto operator+ ( const Boxed<Tag>& a )                      -> typename std::enable_if< Tag::hasAddition,   Boxed<Tag>  >::type   { return Boxed<Tag>( +a.Get() ); }
    template < class Tag > auto operator- ( const Boxed<Tag>& a )                      -> typename std::enable_if< Tag::hasAddition,   Boxed<Tag>  >::type   { return Boxed<Tag>( -a.Get() ); }
    template < class Tag > auto operator+ ( const Boxed<Tag>& a, const Boxed<Tag>& b ) -> typename std::enable_if< Tag::hasAddition,   Boxed<Tag>  >::type   { return Boxed<Tag>( a.Get() + b.Get() ); }
    template < class Tag > auto operator- ( const Boxed<Tag>& a, const Boxed<Tag>& b ) -> typename std::enable_if< Tag::hasAddition,   Boxed<Tag>  >::type   { return Boxed<Tag>( a.Get() - b.Get() ); }
    template < class Tag > auto operator+=(       Boxed<Tag>& a, const Boxed<Tag>& b ) -> typename std::enable_if< Tag::hasAddition,   Boxed<Tag>& >::type   { a.Get() += b.Get(); return a; }
    template < class Tag > auto operator-=(       Boxed<Tag>& a, const Boxed<Tag>& b ) -> typename std::enable_if< Tag::hasAddition,   Boxed<Tag>& >::type   { a.Get() -= b.Get(); return a; }

    template < class Tag > auto operator* ( const typename Tag::ContentType& a, const Boxed<Tag>& b ) -> typename std::enable_if< Tag::hasMultiplication, Boxed<Tag> >::type                    { return Boxed<Tag>( a * b.Get() ); }
    template < class Tag > auto operator* ( const Boxed<Tag>& a, const typename Tag::ContentType& b ) -> typename std::enable_if< Tag::hasMultiplication, Boxed<Tag> >::type                    { return Boxed<Tag>( a.Get() * b ); }
    template < class Tag > auto operator/ ( const Boxed<Tag>& a, const typename Tag::ContentType& b ) -> typename std::enable_if< Tag::hasMultiplication, Boxed<Tag> >::type                    { return Boxed<Tag>( a.Get() / b ); }
    template < class Tag > auto operator/ ( const Boxed<Tag>& a, const Boxed<Tag>&                b ) -> typename std::enable_if< Tag::hasMultiplication, typename Tag::ContentType >::type     { return a.Get() / b.Get(); }
    template < class Tag > auto operator% ( const Boxed<Tag>& a, const typename Tag::ContentType& b ) -> typename std::enable_if< Tag::hasMultiplication, Boxed<Tag>  >::type                   { return Boxed<Tag>( a.Get() % b ); }
    template < class Tag > auto operator% ( const Boxed<Tag>& a, const Boxed<Tag>&                b ) -> typename std::enable_if< Tag::hasMultiplication, Boxed<Tag>  >::type                   { return Boxed<Tag>( a.Get() % b.Get() ); }
    template < class Tag > auto operator*=(       Boxed<Tag>& a, const typename Tag::ContentType& b ) -> typename std::enable_if< Tag::hasMultiplication, Boxed<Tag>& >::type                   { a.Get() *= b;       return a; }
    template < class Tag > auto operator/=(       Boxed<Tag>& a, const typename Tag::ContentType& b ) -> typename std::enable_if< Tag::hasMultiplication, Boxed<Tag>& >::type                   { a.Get() /= b;       return a; }
    template < class Tag > auto operator%=(       Boxed<Tag>& a, const typename Tag::ContentType& b ) -> typename std::enable_if< Tag::hasMultiplication, Boxed<Tag>& >::type                   { a.Get() %= b;       return a; }
    template < class Tag > auto operator%=(       Boxed<Tag>& a, const Boxed<Tag>&                b ) -> typename std::enable_if< Tag::hasMultiplication, Boxed<Tag>& >::type                   { a.Get() %= b.Get(); return a; }
    
    template < class Tag > auto operator<<( std::ostream& stream, const Boxed<Tag>& b ) -> typename std::enable_if< Tag::hasStreaming, std::ostream& >::type    { return stream << b.Get(); }
    template < class Tag > auto operator>>( std::istream& stream,       Boxed<Tag>& b ) -> typename std::enable_if< Tag::hasStreaming, std::istream& >::type    { return stream >> b.Get(); }
   };

namespace PlusPlus
   {
    template < class BoxTag >
    using Boxed = ::PlusPlus_Boxed::Boxed< BoxTag >;
    
    template < class TheWrappedType >
    class BoxedWrapper
       {
        private:
            using Wrapped   = TheWrappedType;
            using Unwrapped = typename Wrapped::ValueType;

        public:
            Wrapped operator()( Unwrapped u ) const                         { return Wrapped( std::move( u ) ); }

            Unwrapped&       Inverse(       Wrapped&   w ) const            { return w.Get(); }
            Unwrapped&&      Inverse(       Wrapped&&  w ) const            { return std::move( w ).Get(); }
            const Unwrapped& Inverse( const Wrapped&   w ) const            { return w.Get(); }
       };
   }

#endif
