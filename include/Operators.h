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
#include <cstring> //memmove
#include <stdexcept>
#include <type_traits>
#include <iostream>
#include "utility.h"



//no implementation

template<typename T>
struct NoComparisonOperators {
    using ComparisonOperators = std::false_type;

    static bool Less(const T&, const T&) {
        throw std::logic_error("'<' not implemented");
        return false;
    }

    static bool Equal(const T&, const T&) {
        throw std::logic_error("'==' not implemented");
        return false;
    }

    static bool NotEqual(const T&, const T&) {
        throw std::logic_error("'!=' not implemented");
        return false;
    }

    static bool Greater(const T&, const T&) {
        throw std::logic_error("'>' not implemented");
        return false;
    }
};

template<typename T>
struct NoSerializer {
    using Serializer = std::false_type;

    static std::ostream& Serialize(std::ostream& os, const T&) {
        throw std::logic_error("'ostream <<' not implemented");
        return os;
    }

    static char* Serialize(const T&, char* begin, const char*) {
        throw std::logic_error("'char* serialization' not implemented");
        return begin;
    }
};

template<typename T>
struct NoArithmeticOperators {
    using ArithmeticOperators = std::false_type;

    static T Plus(const T&, const T&) {
        throw std::logic_error("'+' not implemented");
        return T();
    }

    static T Minus(const T&, const T&) {
        throw std::logic_error("'-' not implemented");
        return T();
    }

    static T Times(const T&, const T&) {
        throw std::logic_error("'*' not implemented");
        return T();
    }

    static T Div(const T&, const T&) {
        throw std::logic_error("'/' not implemented");
        return T();
    }

    static T Modulo(const T&, const T&) {
        throw std::logic_error("'%' not implemented");
        return T();
    }
};

template<typename T>
struct NoLogicalOperators {
    using LogicalOperators = std::false_type;

    static bool Not(const T&) {
        throw std::logic_error("'!' not implemented");
        return false;
    }

    static bool And(const T&, const T&) {
        throw std::logic_error("'&&' not implemented");
        return false;
    }

    static bool Or(const T&, const T&) {
        throw std::logic_error("'||' not implemented");
        return false;
    }
};

template<typename T>
struct NoCallOperator {
    using CallOperator = std::false_type;
};

template<typename T>
struct NoBitwiseOperators {
    using BitwiseOperators = std::false_type;

    static bool And(const T&, const T&) {
        throw std::logic_error("'&' not implemented");
        return false;
    }

    static bool Or(const T&, const T&) {
        throw std::logic_error("'|' not implemented");
        return false;
    }

    static bool Xor(const T&, const T&) {
        throw std::logic_error("'^' not implemented");
        return false;
    }
};

template<typename T>
struct NoHash {
    using HashFun = std::false_type;

    static size_t Hash(const T&) {
        throw std::logic_error("Hash function not implemented");
        return 0;
    }
};

//default implementation

template<typename T>
struct DefaultComparisonOperators {
    using ComparisonOperators = std::true_type;

    static bool Less(const T& v1, const T& v2) {
        if(ANY_HAS(T, LT)) return v1 < v2;
        return NoComparisonOperators< T >::Less(v1, v2);
    }

    static bool Equal(const T& v1, const T& v2) {
        if(ANY_HAS(T, EQ)) return v1 == v2;
        return NoComparisonOperators< T >::Equal(v1, v2);
    }

    static bool NotEqual(const T& v1, const T& v2) {
        if(ANY_HAS(T, NEQ)) return v1 != v2;
        return NoComparisonOperators< T >::NotEqual(v1, v2);
    }

    static bool Greater(const T& v1, const T& v2) {
        if(ANY_HAS(T, GT)) return v1 > v2;
        return NoComparisonOperators< T >::Greater(v1, v2);
    }
};

template<typename T>
struct DefaultSerializer {
    using Serializer = std::true_type;

    static std::ostream& Serialize(std::ostream& os, const T& v) {
        os << v;
        return os;
    }

    static char* Serialize(const T& v, char* begin, const char* end) {
        if(begin + sizeof(v) >= end) return begin;
        memmove(begin, &v, sizeof(v));
        return begin + sizeof(v);
    }
};

template<typename T>
struct DefaultArithmeticOperators {
    using ArithmeticOperators = std::true_type;

    static T Plus(const T& v1, const T& v2) {
        if(ANY_HAS(T, Sum)) return v1 + v2;
        return NoArithmeticOperators< T >::Plus(v1, v2);
    }

    static T Minus(const T& v1, const T& v2) {
        if(ANY_HAS(T, Sum)) return v1 - v2;
        return NoArithmeticOperators< T >::Minus(v1, v2);
    }

    static T Times(const T& v1, const T& v2) {
        if(ANY_HAS(T, Sum)) return v1 * v2;
        return NoArithmeticOperators< T >::Times(v1, v2);
    }

    static T Div(const T& v1, const T& v2) {
        if(ANY_HAS(T, Sum)) return v1 / v2;
        return NoArithmeticOperators< T >::Div(v1, v2);
    }

    static T Modulo(const T& v1, const T& v2) {
        if(ANY_HAS(T, Modulo)) return v1 % v2;
        return NoArithmeticOperators< T >::Modulo(v1, v2);
    }
};


template<typename T>
struct DefaultHash {
    using HashFun = std::true_type;

    static size_t Hash(const T& v) {
        static std::hash<T> h;
        return h(v);
    }
};
