%module Int64;


//typedef long long int64_t;


%typemap(in) int64_t = long;
%typemap(out) int64_t %{ $result = $1; %}
%typemap(javaout) int64_t = long;
%typemap(jni) int64_t  "jlong"
%typemap(jtype) int64_t  "long"
%typemap(jstype) int64_t  "long"
%typemap(javain) int64_t  "$javainput"
%typemap(freearg) int64_t  ""
%typemap(typecheck) int64_t  = long;

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
#include <stdint.h>
#include <vector>
%}

//------------------------------------
// testing:

void inc_long(long&);  // <------ not supported (produces SWIGTYPE)
void inc64(int64_t&);  // <------ not supported (produces SWIGTYPE)
int64_t add64(int64_t, int64_t);
int64_t abs64(int64_t);
int64_t misc64(bool, int64_t, double, int64_t, char);
int64_t& abs64ref(int64_t&);
const long& long_constref(const long&);
const int64_t& int64_constref(const int64_t&);

class Int64Vec {
public:
   int64_t get(int);
   void set(int, int64_t);
   const int64_t& getRef(int);
   void setRef(int, const int64_t&);
};



%include "std_vector.i"

%template(Int64Vector) std::vector<int64_t>;
%template(Int64VectorVector) std::vector<std::vector<int64_t> >;

