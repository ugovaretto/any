#pragma once
////////////////////////////////////////////////////////////////////////////////
//Any
//Copyright (c) 2017 Ugo Varetto
//All rights reserved.
//
//Redistribution and use in source and binary forms, with or without
//modification, are permitted provided that the following conditions are met:
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of the copyright holder nor the
//      names of its contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
//THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//DISCLAIMED. IN NO EVENT SHALL UGO VARETTO BE LIABLE FOR ANY
//DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
////////////////////////////////////////////////////////////////////////////////
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

struct EmptyAllocator {
    static void* Allocate(size_t n) { return nullptr; }
    static void Deallocate(void* p, size_t n) {}
};

///XXX add posix_memalign __aligned_malloc support or
///just wait for aligned_alloc from C++17
///Derived from EmptyAllocator to make conversion to ValHandler<T, EmptyAllocator> correct
struct MallocAllocator : EmptyAllocator {
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
ANY_HASOP2(<=, LE)
ANY_HASOP2(>=, GE)
ANY_HASOP2(&&, AND)
ANY_HASOP2(||, OR)
ANY_HASOP2(&, ANDB)
ANY_HASOP2(|, ORB)
ANY_HASOP2(^, XORB)
ANY_HASOP2(>>, RSHIFT)
ANY_HASOP2(<<, LSHIFT)

#define ANY_HAS(T, name) Has ##name < T >::value


