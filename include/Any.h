#pragma once
////////////////////////////////////////////////////////////////////////////////
//Parsley - parsing framework
//Copyright (c) 2010-2015, Ugo Varetto
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
#include <tuple>

//------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/// @brief Minimal implementation of a class that can hold instances of any
/// type.
/// @ingroup utility
class Any {
public:
    /// Type used by Any::Type() method to signal an empty @c Any instance.
    struct EMPTY_ {};
    /// Default constructor, sets the internal pointer to @c NULL
    Any() : pval_( 0 ) {}
    /// Constructor accepting a parameter copied into internal type instance.
    template < class ValT >
    Any( const ValT& v ) : pval_( new ValHandler< ValT >( v ) ) {}
    /// Copy constructor.
    Any( const Any& a ) : pval_( a.pval_ ? a.pval_->Clone() : 0 ) {}
    /// Destructor: deletes the contained data type.
    ~Any() { delete pval_; }
public:
    /// Returns @c true if instance empty.
    bool Empty() const { return pval_ == 0; }
    /// Returns type of contained data or Any::EMPTY_ if instance empty.
    const std::type_info& Type() const {
        //note gcc requires typeid(C) with C != void; compiles on vc++ 2008
        return !Empty() ? pval_->GetType() : typeid( EMPTY_ ); //
    }
    /// Swap two Any instances by swapping the internal pointers.
    Any& Swap( Any& a ) { std::swap( pval_, a.pval_ ); return *this; }
    /// Assignment
    Any& operator=( const Any& a ) { Any( a ).Swap( *this ); return *this; }
    /// Assignment from non - @c Any value.
    template < class ValT >
    Any& operator=( const ValT& v ) { Any( v ).Swap( *this ); return *this; }
    /// Equality: check by converting value to contained value type then
    /// invoking equality operator on converted type.
    template < class ValT >
    bool operator==( const ValT& v ) const {
        CheckAndThrow< ValT >();
        return (static_cast< ValHandler<ValT>* >( pval_ )->val_ ) == v;
    }
    /// Lower than
    bool operator <(const Any& other) const {
      return pval_->LowerThan(other.pval_);
    }
    /// Equal to
    bool operator ==(const Any& other) const {
      return pval_->EqualTo(other.pval_);
    }
    /// Not equal to
    bool operator !=(const Any& other) const {
      return pval_->NotEqualTo(other.pval_);
    }
    /// Greater than
    bool operator >(const Any& other) const {
      return pval_->GreaterThan(other.pval_);
    }
    /// Size
    size_t Size() const {
      return pval_->Sizeof();
    }
public:
    ///Check, cast and return const reference.
    template < typename T > const T& Get() const {
        CheckAndThrow< T >();
        return static_cast< ValHandler< T >* >( pval_ )->val_;
    }
    ///Convert to const reference.
    template < class ValT > operator const ValT&() const {
        CheckAndThrow< ValT >();
        return (static_cast< ValHandler<ValT>* >( pval_ )->val_ );
    }
    ///Convert to reference.
    template < class ValT > operator ValT&() const {
        CheckAndThrow< ValT >();
        return ( static_cast< ValHandler<ValT>* >( pval_ )->val_ );
    }
    ///Convert to pointer.
    template < class ValT > operator ValT*() const {
        CheckAndThrow< ValT >();
        return &( static_cast< ValHandler<ValT>* >( pval_ )->val_ );
    }
    ///Convert to const pointer.
    template < class ValT > operator const ValT*() const {
        CheckAndThrow< ValT >();
        return &( static_cast< ValHandler<ValT>* >( pval_ )->val_ );
    }
private:
    /// Check if contained data is convertible to specific type.
    template < class ValT > void CheckAndThrow() const {
#ifdef ANY_CHECK_TYPE
        if(typeid( ValT ) != Type())
            throw std::logic_error(
                    (std::string( " Attempt to convert from ")
                    + Type().name()
                    + std::string( " to " )
                    + typeid( ValT ).name() ).c_str() );
#endif
    }
    /// @interface HandlerBase Wrapper for data storage.
    struct HandlerBase {// hint: use small object allocator {
        virtual const std::type_info& GetType() const = 0;
        virtual HandlerBase* Clone() const = 0;
        virtual ~HandlerBase() {}
        virtual std::ostream& Serialize(std::ostream& os) const = 0;
        virtual char* Serialize(char* begin, const char* end) const = 0;
        virtual bool LowerThan(const HandlerBase* other) const = 0;
        virtual bool EqualTo(const HandlerBase* other) const = 0;
        virtual bool NotEqualTo(const HandlerBase* other) const = 0;
        virtual bool GreaterThan(const HandlerBase* other) const = 0;
        virtual bool Sizeof() const = 0;
    };

    /// HandlerBase actual data container class.
    template < class T > struct ValHandler :  HandlerBase {
        typedef T Type;
        T val_;
        ValHandler( const T& v ) : val_( v ) {}
        const std::type_info& GetType() const { return typeid( T ); }
        ValHandler* Clone() const { return new ValHandler( val_ ); }
        std::ostream& Serialize( std::ostream& os ) const {
            os << val_;
            return os;
        }
        bool LowerThan(const HandlerBase* other) const {
          const ValHandler< Type >* v
            = static_cast< const ValHandler< Type >* >(other);
          return val_ < v->val_;
        }
        bool EqualTo(const HandlerBase* other) const {
          const ValHandler< Type >*v
            = static_cast< const ValHandler< Type >* >(other);
          return val_ == v->val_;
        }
        bool NotEqualTo(const HandlerBase* other) const {
          const ValHandler< Type >*v
            = static_cast< const ValHandler< Type >* >(other);
          return val_ != v->val_;
        }
        bool GreaterThan(const HandlerBase* other) const {
          const ValHandler< Type >*v
            = static_cast< const ValHandler< Type >* >(other);
          return val_ > v->val_;
        }
        char* Serialize(char* begin, const char* end) {
          return SerializeToCharBuffer(val_, begin, end);
        }
        size_t Sizeof() const {
          return sizeof(val_);
        }
    };

    ///Pointer to contained data: deleted when Any instance deleted.
    HandlerBase* pval_;

    ///Overloaded operator to serialize data to output streams.
    friend inline std::ostream& operator<<( std::ostream& os, const Any& any ) {
        if( any.Empty() ) return os;
        return any.pval_->Serialize( os );
    }

};

//------------------------------------------------------------------------------
///Utility function to print the content of an std::vector of Any objects.
std::ostream& operator<<( std::ostream& os, const std::vector< Any >& av ) {
    std::copy( av.begin(), av.end(), std::ostream_iterator< Any >( os, ", " ) );
    return os;
}
//------------------------------------------------------------------------------
