%module test;

namespace ns {
%ignore *::foo; // <--------- DOES NOT WORK! SWIG BUG: SEEMS LIKE %ignore WITHIN A NAMESPACE HAS NO EFFECT!
}

%ignore *::bar; // works, affects classes in and outside namespaces

class X {
public:
    void foo(int);
    void bar(int);
};

namespace ns {
class Y {
public:
    void foo(double);
    void bar(double);
};
}

