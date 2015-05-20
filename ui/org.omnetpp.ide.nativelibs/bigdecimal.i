//
// Map common::BigDecimal to org.omnetpp.common.engine.BigDecimal which wraps the C++ object.
// Note: BigDecimal::Nil is mapped to Java's null.
//
// Author: Tamas Borbely, 2007
//

%{
#include "common/bigdecimal.h"
%}

namespace omnetpp { namespace common {
  class BigDecimal;
}}

// when passed by value:
%typemap(jstype) omnetpp::common::BigDecimal "org.omnetpp.common.engine.BigDecimal";
%typemap(javain) omnetpp::common::BigDecimal "org.omnetpp.common.engine.BigDecimal.getCPtr($javainput)"

%typemap(javaout) omnetpp::common::BigDecimal {
    long cPtr = $jnicall;
    return (cPtr == 0) ? null : new org.omnetpp.common.engine.BigDecimal(cPtr, true);
}

%typemap(in) omnetpp::common::BigDecimal ($&1_type argp) %{
   argp = *($&1_ltype*)(void *)&$input;
   if (!argp) {
      $1 = BigDecimal::Nil;
   }
   else {
      $1 = *argp;
   }
%}

%typemap(out) omnetpp::common::BigDecimal %{
   if ($1.isNil())
   {
      $result = 0;
   }
   else
   {
      {*($&1_ltype*)(void *)&$result = new $1_ltype(($1_ltype &)$1); }
   }
%}

// when passed by pointer:
%typemap(jstype) omnetpp::common::BigDecimal* "org.omnetpp.common.engine.BigDecimal";
%typemap(javain) omnetpp::common::BigDecimal* "org.omnetpp.common.engine.BigDecimal.getCPtr($javainput)"

%typemap(javaout) omnetpp::common::BigDecimal* {
    long cPtr = $jnicall;
    return (cPtr == 0) ? null : new org.omnetpp.common.engine.BigDecimal(cPtr, $owner);
}

%typemap(in) omnetpp::common::BigDecimal* %{
   if (!$input) {
      $1 = &BigDecimal::Nil;
   }
   else {
      $1 = *($1_ltype*)(void*)&$input;
   }
%}

%typemap(out) omnetpp::common::BigDecimal* %{
   if (!$1 || $1->isNil())
   {
      $result = 0;
   }
   else
   {
      *($1_ltype*)(void *)&$result = $1;
   }
%}

// when passed by reference:
%typemap(jstype) omnetpp::common::BigDecimal& "org.omnetpp.common.engine.BigDecimal";
%typemap(javain) omnetpp::common::BigDecimal& "org.omnetpp.common.engine.BigDecimal.getCPtr($javainput)"

%typemap(javaout) omnetpp::common::BigDecimal& {
    long cPtr = $jnicall;
    return (cPtr == 0) ? null : new org.omnetpp.common.engine.BigDecimal(cPtr, false);
}

%typemap(in) omnetpp::common::BigDecimal& %{
   if (!$input) {
      $1 = ($1_ltype)(void *)&BigDecimal::Nil;
   }
   else {
      $1 = *($1_ltype*)(void *)&$input;
   }
%}

%typemap(out) omnetpp::common::BigDecimal& %{
   if ($1->isNil())
   {
      $result = 0;
   }
   else
   {
      {*($1_ltype*)(void *)&$result = $1; }
   }
%}

