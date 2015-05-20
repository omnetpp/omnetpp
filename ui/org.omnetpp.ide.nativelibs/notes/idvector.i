%module ScaveEngine

// wrap int64_t into java long (from ./int64.i)
%typemap(in) int64_t = long;
%typemap(out) int64_t %{ $result = $1; %}
%typemap(javaout) int64_t = long;
%typemap(jni) int64_t  "jlong"
%typemap(jtype) int64_t  "long"
%typemap(jstype) int64_t  "long"
%typemap(javain) int64_t  "$javainput"
%typemap(freearg) int64_t  ""
%typemap(typecheck) int64_t  = long;

// wrap const int64_t& into java long (from ./int64.i)
%typemap(in) const int64_t & = const long &;
%typemap(out) const int64_t & %{ $result = *$1; %}
%typemap(javaout) const int64_t & = long;
%typemap(jni) const int64_t &  "jlong"
%typemap(jtype) const int64_t &  "long"
%typemap(jstype) const int64_t &  "long"
%typemap(javain) const int64_t &  "$javainput"
%typemap(freearg) const int64_t &  ""
%typemap(typecheck) const int64_t &  = long;


%{
namespace omnetpp { namespace scave {
typedef int64_t ID;
} }
%}

namespace omnetpp { namespace scave {
typedef int64_t ID;

class IDList {
public:
    ID get();
    int64_t get64();
    void set(ID);
   const int64_t& getRef(int);
   void setRef(int, const int64_t&);
};

} }


%include "std_common.i"
%include "std_vector.i"

namespace std {
   %template(IDVector) vector<omnetpp::scave::ID>;
   %template(IDVectorVector) vector<vector<omnetpp::scave::ID> >;
};


