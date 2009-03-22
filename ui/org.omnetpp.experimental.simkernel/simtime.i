%module Simkernel;

class SimTime;

%{
#include "bigdecimal.h"
%}

//
// map SimTime to our common.BigDecimal
//
%typemap(jstype) SimTime "org.omnetpp.common.engine.BigDecimal";
%typemap(javain) SimTime "org.omnetpp.common.engine.BigDecimal.getCPtr($javainput)"
%typemap(javaout) SimTime {
    long cPtr = $jnicall;
    return (cPtr == 0) ? null : new org.omnetpp.common.engine.BigDecimal(cPtr, true);
  }

%typemap(out) SimTime %{
  *(BigDecimal **)&$result = new BigDecimal($1.raw(), SimTime::getScaleExp());
%}

%typemap(in) SimTime (BigDecimal *argp) %{
  {
    BigDecimal *argp = *(BigDecimal**)&$input;
    if (!argp) {
      SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "Attempt to dereference null SimTime");
      return $null;
    }
    $1.setRaw(argp->getMantissaForScale(SimTime::getScaleExp()));
  }
%}

//
// same thing again, this time with SimTime&
//
%typemap(jstype) SimTime& "org.omnetpp.common.engine.BigDecimal";
%typemap(javain) SimTime& "org.omnetpp.common.engine.BigDecimal.getCPtr($javainput)"
%typemap(javaout) SimTime& {
    long cPtr = $jnicall;
    return (cPtr == 0) ? null : new org.omnetpp.common.engine.BigDecimal(cPtr, true);
  }

%typemap(out) SimTime& %{
  *(BigDecimal **)&$result = new BigDecimal($1->raw(), SimTime::getScaleExp());
%}

%typemap(in) SimTime& (BigDecimal *argp) %{
  {
    BigDecimal *argp = *(BigDecimal**)&$input;
    if (!argp) {
      SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "Attempt to dereference null SimTime");
      return $null;
    }
    $1->setRaw(argp->getMantissaForScale(SimTime::getScaleExp()));
  }
%}


