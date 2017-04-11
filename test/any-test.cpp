//Author: Ugo Varetto

#define ANY_OSTREAM
#include <vector>
#include <tuple>
#include <cassert>
#include <map>
#include <unordered_map>
//AnyPolicies inclusion is required before including Any.h since virtual methods do call methods derived
//from AnyPolicies< T >::* base classes.
//Custom policies can be implemented by replacing the default implementations. Operators.h contains default
//implementations of all the available policies which can be reused as needed.
#include <AnyPolicies.h>
#define ANY_CHARPTR_TO_STRING
#define ANY_CHECK_TYPE
#include <Any.h>
#include <array>
#include <utility>

using namespace std;

struct Handler {
    virtual void Get(char* data) const = 0;
    virtual void Set(const char* data) = 0;
    virtual size_t Sizeof() const = 0;
    virtual ~Handler() {}
};

template < typename T >
struct Val : Handler {
    T data_;
    Val(const T& v) : data_(v) {}
    void Get(char* data) const {
        memmove(data, &data_, sizeof(data_));
    }
    void Set(const char* data) {
        memmove(&data_, data, sizeof(data_));
    }
    size_t Sizeof() const {
        return sizeof(data_);
    }
};

//in any constructor: if static allocation choose allocator which never invokes delete
//in clone function: pass reference to destination address; allocate memory first then invoke Clone(address of allocated memory)
//use std::array
struct Container {
    std::array< Handler*, 2 > data; //2 * sizeof(void*)
    template < typename T >
    Container(const T& v, bool s) {
        if(sizeof(declval<Val<T>>()) <= sizeof(declval<std::array< Handler*, 2 >>())) {
            cout << "Static" << endl;
        } else {
            cout << "Dynamic" << endl;
        }
        if(s) {
            new (&data[0]) Val<T>(v);
            //data[0] will never be zero since Handler always includes a vtable
            //pointer
            cout << "sizeof Val: " << sizeof(Val<T>(v)) << endl;
            cout << "array size: " << sizeof(std::array< Handler*, 2 >) << endl;
        } else {
            data[0] = 0;
            data[1] = new Val<T>(v);;
        }
    }
    Handler* Ptr() const {
        if(data[0] == 0) {
            return data[1];
        }
        return (Handler*) &data[0];
    }
};

struct C2 {
    void Method() { cout << "C2" << endl; }
};



int main( int, char** ) {

    Any ai1 = 2;
    Any ai2 = 1.0f;
    ai1 = ai2;

    vector< Any > vany( 4 );
    vany[ 0 ] = ai1;
    vany[ 1 ] = ai2;
    Any anyv = vany;

    const vector< Any >& anyvcref = anyv;
    vector< Any >& anyvref = anyv;
    try {
        vector< double >& anyvref2 = anyv;
        assert(false);
    } catch( const std::exception& e ) {
        assert(true);
    }
    assert(anyvcref[ 0 ] == ai1);
    anyvref[ 0 ] = std::string( "ciao" );
    assert(anyvref[ 0 ] == "ciao");

    Any anytuple = make_tuple( 1, 2.3 );
    typedef const tuple< int, double >& TupleConstRef;
    assert(get< 1 >(static_cast< TupleConstRef >( anytuple )) == 2.3);

    Any i1 = 8;
    Any i2 = 9;
    assert(i2 > i1);
    assert(i2 != i1);
    assert(i1 < i2);
    i1 = 9;
    assert(i1 == i2);
    i1 = i2;
    assert(i1 == i2);

    struct Base {
        virtual ~Base() {}
    };
    struct Derived : Base {

        ~Derived() {}
    };
    Base* pbase;
    Derived  derived;
    pbase = &derived;
    Any pbaseany(pbase);
    pbase = pbaseany;

    map< Any, Any > si = map< Any, Any >{{string("one"), 1},
                                         {string("two"), 2}};
    assert(si[string("one")] == 1);

    //with ordered map the assigned keys must always be of the same type after declaration,
    //if not an exception is thrown when ANY_CHECK_TYPE is #defined
    map< Any, Any > si2 = map< Any, Any >{{"one", 1},
                                          {"two", 2}};
    assert(si2[string("one")] == 1);
    assert(si2["one"] == 1);

    //with unordered maps the maps can hold any type of key at any given point in time as long as a valid
    //std::hash function is available
    unordered_map< Any, Any > um = {{"one", 1}, {"TWO", "two"}};
    assert(um["TWO"] == "two");
    um[1.0] = "ONE POINT ZERO";
    assert(um[1.0] == "ONE POINT ZERO");

    struct MyClass {
      int v = 3;
    };
    Any m = MyClass();

    Any m1(move(m));
    assert(m.Empty());

    cout << "PASSED" << endl;

    Container c(10, true);
    Container c2(100, false);

    int ci1;
    c.Ptr()->Get((char*) &ci1);
    cout << "1: " << ci1 << endl;

    int ci2;
    c2.Ptr()->Get((char*) &ci2);
    cout << "2: " << ci2 << endl;


    return 0;
}
