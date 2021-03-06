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

/// @file Any.h Implementation of class to hold instances of any type.

#include <typeinfo>
#include <vector>
#include <iterator>
#include <algorithm>
#include <iostream>
#include "utility.h"
#include "Operators.h"
#include "AnyPolicies.h"
#ifdef ANY_CHARPTR_TO_STRING
#include <string>
#endif

#include "ValHandler.h"
/// @brief Hold instances of any type.
class Any {
private:
    /// @interface HandlerBase Wrapper for data storage.
    struct HandlerBase {// hint: use small object allocator {
        virtual const std::type_info& GetType() const = 0;
        virtual HandlerBase* Clone() const = 0;
        virtual ~HandlerBase() {}
        virtual void Set(const void*) = 0;
        virtual size_t Sizeof() const = 0;
        virtual size_t Hash() const = 0;
        virtual void Destroy() = 0;
        //serilization
        virtual std::ostream& Serialize(std::ostream& os) const = 0;
        virtual char* Serialize(char* begin, const char* end) const = 0;
        //logical operators
        virtual bool LowerThan(const HandlerBase* other) const = 0;
        virtual bool EqualTo(const HandlerBase* other) const = 0;
        //virtual bool EqualTo(const void* other) const = 0;
        virtual bool NotEqualTo(const HandlerBase* other) const = 0;
        virtual bool GreaterThan(const HandlerBase* other) const = 0;
    };
public:
    /// Type used by Any::Type() method to signal an empty @c Any instance.
    struct EMPTY_ {};
    /// Default constructor, sets the internal pointer to @c NULL
    Any() : pval_( 0 ) {}
#ifdef ANY_CHARPTR_TO_STRING
    Any(const char* s) : Any(std::string(s)) {}
#endif
    using DefaultAllocator = MallocAllocator;
    template < typename ValT >
    using ValStorage = ValHandler< ValT,
            HandlerBase,
            EmptyAllocator,
            typename AnyPolicies< ValT >::Comparison,
            typename AnyPolicies< ValT >::Serializer,
            typename AnyPolicies< ValT >::Arithmetic,
            typename AnyPolicies< ValT >::Logical,
            typename AnyPolicies< ValT >::Call,
            typename AnyPolicies< ValT >::Bitwise,
            typename AnyPolicies< ValT >::HashFun >;
    /// Constructor accepting a parameter copied into internal type instance.
    template < typename ValT, typename AllocatorT = DefaultAllocator >
    Any(const ValT& v, AllocatorT&& a = AllocatorT()) : pval_(nullptr) {
        using V = ValHandler< ValT,
                HandlerBase,
                AllocatorT,
                typename AnyPolicies< ValT >::Comparison,
                typename AnyPolicies< ValT >::Serializer,
                typename AnyPolicies< ValT >::Arithmetic,
                typename AnyPolicies< ValT >::Logical,
                typename AnyPolicies< ValT >::Call,
                typename AnyPolicies< ValT >::Bitwise,
                typename AnyPolicies< ValT >::HashFun >;
        pval_ = reinterpret_cast< HandlerBase* >(a.Allocate(sizeof(V(v, a))));
        new (pval_) V(v, a);
    }
    /// Copy constructor.
    Any(const Any& a) : pval_(a.pval_ ? a.pval_->Clone() : nullptr) {}
    /// Move constructor
    Any(Any&& other) : pval_(other.pval_) {other.pval_ = nullptr;}
    /// Destructor: deletes the contained data type.
    ~Any() { if(pval_) pval_->Destroy(); }
public:
    /// Returns @c true if instance empty.
    bool Empty() const { return pval_ == 0; }
    /// Returns type of contained data or Any::EMPTY_ if instance empty.
    const std::type_info& Type() const {
        //note gcc requires typeid(C) with C != void; compiles on vc++ 2008
        return !Empty() ? pval_->GetType() : typeid( EMPTY_ ); //
    }
    /// Swap two Any instances by swapping the internal pointers.
    Any& Swap(Any& a) { std::swap( pval_, a.pval_ ); return *this; }
    /// Assignment
    Any& operator=(const Any& a) { Any(a).Swap(*this); return *this; }
    /// Assignment from non - @c Any value.
    template < class ValT >
    Any& operator=(const ValT& v) {
        if(Type() == typeid(v)) {
            pval_->Set(&v);
        } else {
            Any(v).Swap(*this);
        }
        return *this;
    }
    /// Size
    size_t Size() const {
        return pval_->Sizeof();
    }
    /// Hash
    size_t Hash() const {
        return pval_->Hash();
    }
#ifdef ANY_CHARPTR_TO_STRING
    bool operator==(const char* other) const {
      return operator==(std::string(other));
    }
#endif
    /// Equality: check by converting value to contained value type then
    /// invoking equality operator on converted type.
    template < class ValT >
    bool operator==(const ValT& v) const {
        return (typeid(v) == Type()) && static_cast< ValStorage< ValT >* >( pval_ )->val_ == v;
    }
    // Data access
public:
    ///Check, cast and return const reference.
    template < typename T > const T& Get() const {
        CheckAndThrow< T >();
        return static_cast< ValStorage< T >* >( pval_ )->val_;
    }
    ///Convert to const reference.
    template < class ValT > operator const ValT&() const {
        CheckAndThrow< ValT >();
        return (static_cast< ValStorage< ValT >* >( pval_ )->val_ );
    }
    ///Convert to reference.
    template < class ValT > operator ValT&() const {
        CheckAndThrow< ValT >();
        return ( static_cast< ValStorage< ValT >* >( pval_ )->val_ );
    }
    // Logical operators
public:
    /// Lower than
    bool operator <(const Any& other) const {
      CheckAndThrow(other);
      return pval_->LowerThan(other.pval_);
    }
    /// Equal to
    bool operator ==(const Any& other) const {
      return Type() == other.Type() && pval_->EqualTo(other.pval_);
    }
    /// Not equal to
    bool operator !=(const Any& other) const {
      CheckAndThrow(other);
      return pval_->NotEqualTo(other.pval_);
    }
    /// Greater than
    bool operator >(const Any& other) const {
      CheckAndThrow(other);
      return pval_->GreaterThan(other.pval_);
    }

private:
    /// Check if contained data is convertible to specific type.
    template < class ValT > void CheckAndThrow() const {
#ifdef ANY_CHECK_TYPE
        if(typeid(ValT) != Type())
            throw std::logic_error(
                    (std::string( " Attempt to convert from ")
                    + Type().name()
                    + std::string( " to " )
                    + typeid(ValT).name() ).c_str());
#endif
    }
    void CheckAndThrow(const Any& other) const {
      #ifdef ANY_CHECK_TYPE
              if(Type() != other.Type())
                  throw std::logic_error(
                          (std::string( " Attempt to convert from ")
                          + Type().name()
                          + std::string( " to " )
                          + other.Type().name()).c_str());
      #endif
    }

    ///Pointer to contained data: deleted when Any instance deleted.
    HandlerBase* pval_;

    ///Overloaded operator to serialize data to output streams.
    friend inline std::ostream& operator<<( std::ostream& os, const Any& any ) {
        if( any.Empty() ) return os;
        return any.pval_->Serialize( os );
    }

};

namespace std {
  template <>
  struct hash< Any > {
    typedef Any argument_type;
      typedef std::size_t result_type;
      result_type operator()(argument_type const& s) const {
          return s.Hash();
      }
  };
}


//------------------------------------------------------------------------------
///Utility function to print the content of an std::vector of Any objects.
std::ostream& operator<<( std::ostream& os, const std::vector< Any >& av ) {
    std::copy( av.begin(), av.end(), std::ostream_iterator< Any >( os, ", " ) );
    return os;
}
//------------------------------------------------------------------------------
