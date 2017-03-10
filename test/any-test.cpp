//Author: Ugo Varetto

#define ANY_OSTREAM
#include <vector>
#include <tuple>
#include <cassert>
#include "../include/Any.h"


struct Base {};
struct Derived : Base {};

int main( int, char** )
{
    Any any_int = 2;
    Any any_int2 = 1.0f;
    any_int2 = any_int;
    std::cout << any_int2 << std::endl;

    std::vector< Any > vany( 4 );
    vany[ 0 ] = any_int;
    vany[ 1 ] = any_int2;
    Any anyv = vany;

    const std::vector< Any >& anyvcref = anyv;
    std::vector< Any >& anyvref = anyv;
    try {
        std::vector< double >& anyvref2 = anyv;
    } catch( const std::exception& e ) {
        std::cout << e.what() << std::endl;
    }
    std::cout << anyvcref[ 0 ] << std::endl;
    anyvref[ 0 ] = std::string( "ciao" );
    std::cout << anyvref[ 0 ] << std::endl;

    Any anytuple = std::make_tuple( 1, 2.3 );
    typedef const std::tuple< int, double >& TupleConstRef;
    std::cout << std::get< 1 >(static_cast< TupleConstRef >( anytuple ))
              << std::endl;

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
    Any pbaseany(  pbase );
    pbase = static_cast< Base* >( static_cast< Derived* >( pbaseany ) );
    return 0;
}
