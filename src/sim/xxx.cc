#include "stlwrap.h"

// XXX just trying...

void f() {
    std::string a = stdstring("hello");
    a = stdstring("hello");

    stdstring b = std::string("hello");
    b = std::string("hello");

    stdstring c(b);
    c = b;

    stdstring s;
    stdstringvector v;
    for (stdstringvector::iterator i = v.begin(); i != v.end(); ++i)
        s += (*i);
}

#include "cpolymorphic.h"

class A : public cObject, public stdstringvector {

};