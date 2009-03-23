%module Simkernel;

#define simtime_t SimTime

class SimTime;

//
// temporary: map SimTime to double
//
%typemap(jni)    SimTime "jdouble"
%typemap(jtype)  SimTime "double"
%typemap(jstype) SimTime "double"
%typemap(javain) SimTime "$javainput"
%typemap(javaout) SimTime {
    return $jnicall;
  }

%typemap(out) SimTime %{
  $result = $1.dbl();
%}

%typemap(in) SimTime (double x) %{
  $1 = $input;
%}

%ignore record(SimTime);
%ignore recordWithTimestamp(simtime_t,SimTime);
%ignore recordScalar(const char *,SimTime);
%ignore recordScalar(const char *,SimTime,const char *);
%ignore collect(SimTime);
%ignore collect2(SimTime,double);
%ignore collect2(double,SimTime);
%ignore collect2(SimTime,SimTime);
%ignore doPacking(cCommBuffer*,SimTime);
%ignore doUnpacking(cCommBuffer*,SimTime&);
%ignore exponential(SimTime);
%ignore exponential(SimTime,int);
%ignore normal(SimTime,SimTime);
%ignore normal(SimTime,SimTime,int);
%ignore truncnormal(SimTime,SimTime);
%ignore truncnormal(SimTime,SimTime,int);
%ignore uniform(SimTime,SimTime);
%ignore uniform(SimTime,SimTime,int);


