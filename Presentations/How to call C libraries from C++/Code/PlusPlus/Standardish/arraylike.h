//
//  arraylike.h
//  PlusPlus
//
//  Created by Lisa Lippincott on 9/5/14.
//  Released into the public domain by Lisa Lippincott, 2014.
//

#ifndef PLUSPLUS_STDISH_ARRAYLIKE_H
#define PLUSPLUS_STDISH_ARRAYLIKE_H

#include <type_traits>
#include <string>
#include <array>
#include <vector>

/*
    is_arraylike            recognizes those types that can be treated as a contiguous array of elements.
    arraylike_element_type  provides uniform access to the element type of an arraylike type
    arraylike_data          provides uniform access to the starting pointer of an arraylike type
    arraylike_size          provides uniform access to the array size of an arraylike type
*/

namespace PlusPlus
   {
    namespace stdish
       {
        template < class T > struct is_arraylike: std::false_type {};
        
        template < class E, std::size_t n >    struct is_arraylike< E[n]                     >: std::true_type {};
        template < class E, std::size_t n >    struct is_arraylike< std::array<E,n>          >: std::true_type {};
        template < class E, class A >          struct is_arraylike< std::vector<E,A>         >: std::true_type {};
        template < class C, class T, class A > struct is_arraylike< std::basic_string<C,T,A> >: std::true_type {};



        template < class T > struct arraylike_element_type;
        
        template < class E, std::size_t n >    struct arraylike_element_type< E[n]                     > { using type = E; };
        template < class E, std::size_t n >    struct arraylike_element_type< std::array<E,n>          > { using type = E; };
        template < class E, class A >          struct arraylike_element_type< std::vector<E,A>         > { using type = E; };
        template < class C, class T, class A > struct arraylike_element_type< std::basic_string<C,T,A> > { using type = C; };
        
        template < class T > using arraylike_element = typename arraylike_element_type<T>::type;



        template < class E, std::size_t n >    E       *arraylike_data( E(&a)[n] )                              { return a; }
        template < class E, std::size_t n >    E       *arraylike_data(       std::array<E,n>& a )              { return a.data(); };
        template < class E, std::size_t n >    E const *arraylike_data( const std::array<E,n>& a )              { return a.data(); };
        template < class E, class A >          E       *arraylike_data(       std::vector<E,A>& a )             { return a.data(); };
        template < class E, class A >          E const *arraylike_data( const std::vector<E,A>& a )             { return a.data(); };
        template < class C, class T, class A > C       *arraylike_data(       std::basic_string<C,T,A>& a )     { return a.data(); };
        template < class C, class T, class A > C const *arraylike_data( const std::basic_string<C,T,A>& a )     { return a.data(); };



        template < class E, std::size_t n >    std::size_t arraylike_size( E(&)[n] )                            { return n; }
        template < class E, std::size_t n >    std::size_t arraylike_size( const std::array<E,n>& )             { return n; };
        template < class E, class A >          std::size_t arraylike_size( const std::vector<E,A>& a )          { return a.size(); };
        template < class C, class T, class A > std::size_t arraylike_size( const std::basic_string<C,T,A>& a )  { return a.size(); };
       }
   }

#endif
