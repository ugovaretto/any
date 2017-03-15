#pragma once
//
// Created by Ugo Varetto on 15/03/2017.
//
//------------------------------------------------------------------------------
#include <type_traits>
#include <iostream>
#include <tuple>
#include <cstdlib> //malloc
///Utility function to serialize the content of a tuple
namespace { template < size_t idx, size_t size >
    struct PrintTuple {
        template < typename T >
        static std::ostream& Do(std::ostream& os, const T& t) {
            os << std::get< idx >(t);
            if(idx < (size - 1)) os << ' ';
            return PrintTuple< idx + 1, size >::Do(os, t);
        }
    };
    template < size_t I >
    struct PrintTuple< I, I> {
        template < typename T >
        static std::ostream& Do(std::ostream& os, const T&) {
            return os;
        }
    };
}
template < typename...ArgsT >
std::ostream& operator<<(std::ostream& os, const std::tuple< ArgsT... >& t) {
    return PrintTuple< size_t(0), sizeof...(ArgsT) >::Do(os, t);
}

///XXX add posix_memalign __aligned_malloc support or
///just wait for aligned_alloc from C++17
struct MallocAllocator {
    static void* Allocate(size_t n) { return malloc(n); }
    static void Deallocate(void* p, size_t n) { free(p); }
};
//------------------------------------------------------------------------------
namespace {
#ifdef ANY_HASOP2
#error "ANY_HASOP2 already defined"
#endif

#define ANY_HASOP2(op, name) \
template < typename A, typename T = decltype((std::declval< A >() op std::declval< A >())) > \
std::true_type Test ##name(A&&); \
template < typename A > \
std::false_type Test ##name(...); \
template< typename T > using Has ##name = decltype((Test ##name < T >(std::declval< T >())));

#ifdef ANY_HASOP1
#error "ANY_HASOP1 already defined"
#endif

#define ANY_HASOP1(op, name) \
template < typename A, typename T = decltype((op std::declval< A >())) > \
std::true_type Test ##name(A&&); \
template < typename A > \
std::false_type Test ##name(...); \
template< typename T > using Has ##name = decltype((Test ##name < T >(std::declval< T >())));
}
//use like this: HasPlus< int >::value_type
ANY_HASOP1(++, Inc)
ANY_HASOP1(--, Dec)
ANY_HASOP1(+, Plus)
ANY_HASOP1(-, Minus)
ANY_HASOP1(!, Not)
ANY_HASOP2(+, Sum)
ANY_HASOP2(-, Sub)
ANY_HASOP2(*, Mul)
ANY_HASOP2(/, Div)
ANY_HASOP2(%, Modulo)
ANY_HASOP2(>, GT)
ANY_HASOP2(<, LT)
ANY_HASOP2(==, EQ)
ANY_HASOP2(!=, NEQ)
ANY_HASOP2(&&, AND)
ANY_HASOP2(||, OR)
ANY_HASOP2(&, ANDB)
ANY_HASOP2(|, ORB)
ANY_HASOP2(^, XORB)
ANY_HASOP2(>>, RSHIFT)
ANY_HASOP2(<<, LSHIFT)

#define ANY_HAS(T, name) Has ##name < T >::value


