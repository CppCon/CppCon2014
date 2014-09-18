#include <stdio.h>
#include <string>
#include <tuple>
#include <type_traits>
#include <assert.h>
#include <utility>

#ifndef MAKE
 #error "-DMAKE= make1, make2, or make3"
#endif
#ifndef PARAMETER
 #error "-DPARAMETER= 1,2,4,8,16,32,64,128,256, or 512"
#endif
#ifndef SPACESHIP
 #error "-DSPACESHIP= spaceship1 or spaceship2"
#endif

#if 1
#define INDEX_SEQUENCE(NN) std::index_sequence<NN>
#define MAKE_INDEX_SEQUENCE(N) std::make_index_sequence<N>
#else
#define INDEX_SEQUENCE(NN) std::__tuple_indices<NN>
#define MAKE_INDEX_SEQUENCE(N) typename std::__make_tuple_indices<N>::type
#endif

// The easy part.
//

namespace std {

    template <class _Tp, class _Up>
    constexpr int spaceship(const _Tp& __x, const _Up& __y)
    {
        return (__x < __y) ? -1 : (__y < __x) ? 1 : 0;
    }

    template<class _Cp, class _Tp, class _Ap,
                        class _Up, class _Bp>
    int spaceship(const basic_string<_Cp,_Tp,_Ap>& __x,
                  const basic_string<_Cp,_Up,_Bp>& __y)
    {
        int __r = __x.compare(0, __x.size(), __y.data(), __y.size());
        return (__r < 0) ? -1 : (__r > 0);
    }

template<size_t _Ip> struct __tuple_spaceship1
{
    template <class _Tp, class _Up>
    constexpr int operator()(const _Tp& __x, const _Up& __y) const
    {
        int __r = __tuple_spaceship1<_Ip-1>()(__x, __y);
        return (__r != 0) ? __r : spaceship(get<_Ip-1>(__x), get<_Ip-1>(__y));
    }
};

template<> struct __tuple_spaceship1<0>
{
    template <class _Tp, class _Up>
    constexpr int operator()(const _Tp&, const _Up&) const
    {
        return 0;
    }
};

template <class ..._Tp, class ..._Up>
constexpr int spaceship1(const tuple<_Tp...>& __x, const tuple<_Up...>& __y)
{
    static_assert(sizeof...(_Tp) == sizeof...(_Up), "");
    return __tuple_spaceship1<sizeof...(_Tp)>()(__x, __y);
}

template <class _Tp, class _Up, size_t ..._Ip>
constexpr int __tuple_spaceship2(const _Tp& __x, const _Up& __y, const INDEX_SEQUENCE(_Ip...)&)
{
    int __r = 0;
    std::initializer_list<int> x = {
        (__r != 0) ? 0 : (__r = spaceship(get<_Ip>(__x), get<_Ip>(__y))) ...
    };
    return __r;
}

template <class ..._Tp, class ..._Up>
constexpr int spaceship2(const tuple<_Tp...>& __x, const tuple<_Up...>& __y)
{
    static_assert(sizeof...(_Tp) == sizeof...(_Up), "");
    return __tuple_spaceship2(__x, __y, MAKE_INDEX_SEQUENCE(sizeof...(_Tp)){});
}

} // namespace std


namespace Foo {
  class Widget {
      int value;
    public:
      Widget(int v): value(v) {}
      friend int spaceship(const Widget& w1, const Widget& w2) { return std::spaceship(w1.value, w2.value); }
      void operator< (const Widget&) const { printf("BAAAD\n"); assert(false); }
      void operator== (const Widget&) const { printf("BAAAD\n"); assert(false); }
      void operator> (const Widget&) const { printf("BAAAD\n"); assert(false); }
  };
}


template<int N> auto make1() { return std::tuple_cat(make1<N/2>(), make1<N/2>()); }
template<> auto make1<1>() { return std::tuple<std::string>("hello"); }
template<> auto make1<0>() { return std::tuple<>(); }
#define make1(N) make1<N>()

template<size_t... N> auto make2(const INDEX_SEQUENCE(N...)&) { return std::tuple<decltype(N,std::string{})...>((N,"hello")...); }
template<int N> auto make2() { return make2(MAKE_INDEX_SEQUENCE(N){}); }
#define make2(N) make2<N>()

#define S1(X) X
#define S2(X) X,X
#define S4(X) X,X,X,X
#define S8(X) S4(X),S4(X)
#define S16(X) S8(X),S8(X)
#define S32(X) S16(X),S16(X)
#define S64(X) S32(X),S32(X)
#define S128(X) S64(X),S64(X)
#define S256(X) S128(X),S128(X)
#define S512(X) S256(X),S256(X)
#define TOKEN_PASTE(x,y) x##y
#define S(N,X) TOKEN_PASTE(S,N)(X)
#define make3(N) std::tuple<S(N,std::string)>{S(N,"hello")}

int main()
{
    auto a = MAKE(PARAMETER);

    return std::SPACESHIP(a,a);
}
