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

#include "AnyPolicies.h"

struct DummyAlloc {};

/// HandlerBase implementation - actual data container class.
template < typename T >
using AP = AnyPolicies< T >;
template < typename T,
        typename HandlerBase,
        typename AllocatorT = DummyAlloc,
        typename ComparisonOperators = typename AP< T >::Comparison,
        typename Serializer = typename AP< T >::Serializer,
        typename ArithmeticOperators = typename AP< T >::Arithmetic,
        typename LogicalOperators = typename AP< T >::Logical,
        typename CallOperator = typename AP< T >::Call,
        typename BitWiseOperators = typename AP< T >::Bitwise,
        typename HashFun = typename AP< T >::HashFun >
struct ValHandler :
        HandlerBase,
        AllocatorT,
        ComparisonOperators,
        ArithmeticOperators,
        LogicalOperators,
        Serializer,
        CallOperator,
        BitWiseOperators,
        HashFun {
    typedef T Type;
    T val_;
    ValHandler(const T& v, const AllocatorT& a) : val_( v ), AllocatorT(a) {}
    const std::type_info& GetType() const { return typeid( T ); }
    ValHandler* Clone() const {
        ValHandler* p = reinterpret_cast<ValHandler*>(AllocatorT::Allocate(sizeof(ValHandler)));
        new (p) ValHandler(val_, static_cast< const AllocatorT& >(*this));
        return p;
    }
    std::ostream& Serialize( std::ostream& os ) const {
        return Serializer::Serialize(os, val_);
    }
    bool LowerThan(const HandlerBase* other) const {
        const ValHandler< Type, HandlerBase >* v
                = static_cast< const ValHandler< Type, HandlerBase >* >(other);
        return ComparisonOperators::Less(val_, v->val_);
    }
    bool EqualTo(const HandlerBase* other) const {
        const ValHandler< Type, HandlerBase >*v
                = static_cast< const ValHandler< Type, HandlerBase >* >(other);
        return ComparisonOperators::Equal(val_, v->val_);
    }
    bool NotEqualTo(const HandlerBase* other) const {
        const ValHandler< Type, HandlerBase >*v
                = static_cast< const ValHandler< Type, HandlerBase >* >(other);
        return ComparisonOperators::NotEqual(val_, v->val_);
    }
    bool GreaterThan(const HandlerBase* other) const {
        const ValHandler< Type, HandlerBase >*v
                = static_cast< const ValHandler< Type, HandlerBase >* >(other);
        return ComparisonOperators::Greater(val_, v->val_);
    }
    char* Serialize(char* begin, const char* end) const {
        return Serializer::Serialize(val_, begin, end);
    }
    size_t Sizeof() const {
        return sizeof(val_);
    }
    size_t Hash() const {
        return HashFun::Hash(val_);
    }
    void Destroy() {
        val_.~T();
        AllocatorT::Deallocate(this, sizeof(*this)); //DeAllocate will also clear any state in base allocator
    }
};