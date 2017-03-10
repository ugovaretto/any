//Author: Ugo Varetto

#define ANY_OSTREAM
#include <vector>
#include <tuple>
#include <cassert>
#include <map>
#define ANY_CHARPTR_TO_STRING
#include "../include/Any.h"

using namespace std;

struct Base {};
struct Derived : Base {};

int main( int, char** )
{
    Any ai1 = 2;
    Any ai2 = 1.0f;
    ai1 = ai2;
    assert(ai1 == 1.0f);

    vector< Any > vany( 4 );
    vany[ 0 ] = ai1;
    vany[ 1 ] = ai2;
    Any anyv = vany;

    const vector< Any >& anyvcref = anyv;
    vector< Any >& anyvref = anyv;
    try {
        vector< double >& anyvref2 = anyv;
    } catch( const std::exception& e ) {
        cerr << e.what() << std::endl;
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

    Base* pbase;
    Derived  derived;
    pbase = &derived;
    Any pbaseany(pbase);
    pbase = static_cast< Base* >( static_cast< Derived* >( pbaseany ) );

    map< Any, Any > si = map< Any, Any >{{string("one"), 1},
                                         {string("two"), 2}};
    assert(si[string("one")] == 1);

    map< Any, Any > si2 = map< Any, Any >{{"one", 1},
                                         {"two", 2}};
    assert(si2[string("one")] == 1);
    assert(si2["one"] == 1);

    si2[1.0] = "ONE POINT ZERO";
    assert(si2[1.0] == "ONE POINT ZERO");
    cout << "PASSED" << endl;
    return 0;
}
