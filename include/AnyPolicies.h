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
#include <type_traits>
#include <string>
#include <vector>
#include "Operators.h"

template < typename T >
struct AnyPolicies {
  using Comparison =
    typename std::conditional<
      std::is_arithmetic< T >::value,
      DefaultComparisonOperators< T >,
      NoComparisonOperators< T > >::type;
  using Serializer =
    typename std::conditional<
      std::is_arithmetic< T >::value,
      DefaultSerializer< T >,
      NoSerializer< T > >::type;
  using Arithmetic = NoArithmeticOperators< T >;
  using Logical = NoLogicalOperators< T >;
  using Call = NoCallOperator< T >;
  using Bitwise = NoBitwiseOperators< T >;
  using HashFun =
    typename std::conditional<
      std::is_arithmetic< T >::value,
      DefaultHash< T >,
      NoHash< T > >::type;
};

template<
    class CharT,
    class Traits,
    class Allocator
>
struct AnyPolicies< std::basic_string< CharT, Traits, Allocator > > {
  using Type = std::basic_string< CharT, Traits, Allocator >;
  using Comparison = DefaultComparisonOperators< Type >;
  using Serializer = NoSerializer< Type >;
  using Arithmetic = NoArithmeticOperators< Type >;
  using Logical = NoLogicalOperators< Type >;
  using Call = NoCallOperator< Type >;
  using Bitwise = NoBitwiseOperators< Type >;
  using HashFun = DefaultHash< Type >;

};

template < typename T, typename A >
struct AnyPolicies< std::vector< T, A > > {
  using Type = std::vector< T, A >;
  using Comparison = DefaultComparisonOperators< Type >;
  using Serializer = NoSerializer< Type >;
  using Arithmetic = NoArithmeticOperators< Type >;
  using Logical = NoLogicalOperators< Type >;
  using Call = NoCallOperator< Type >;
  using Bitwise = NoBitwiseOperators< Type >;
  using HashFun = NoHash< Type >;
};
