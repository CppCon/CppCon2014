//
//  EnumWrapper.h
//  PlusPlus
//
//  Created by Lisa Lippincott on 8/23/14.
//  Released into the public domain by Lisa Lippincott, 2014.
//

#ifndef PLUSPLUS_ENUMWRAPPER_H
#define PLUSPLUS_ENUMWRAPPER_H

#include <type_traits>

/*
    EnumWrapper< WrappedType, UnwrappedType > is used as a base of Wrapper specializations (see Wrapper.h).
    It wraps and unwraps using static_cast, as appropriate for enumerated types.
    
    The default for the second parameter is std::underlying_type< TheWrappedType >::type, but 
    a different type can be specified if the unwrapped type is also an enumerated type.
*/

namespace PlusPlus
   {
    template < class TheWrappedType,
               class TheUnwrappedType = typename std::underlying_type< TheWrappedType >::type >
    class EnumWrapper
       {
        private:
            using Wrapped   = TheWrappedType;
            using Unwrapped = TheUnwrappedType;

        public:
            Wrapped   operator()(   Unwrapped u ) const             { return static_cast< Wrapped   >( u ); }
            Unwrapped Inverse(      Wrapped   w ) const             { return static_cast< Unwrapped >( w ); }
       };
   }

#endif
