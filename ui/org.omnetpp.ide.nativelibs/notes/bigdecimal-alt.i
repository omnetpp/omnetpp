//
// Map common::BigDecimal to java.math.BigDecimal.
//
// *** THIS IS CURRENTLY UNUSED, WE WRAP OUR BigDecimal C++ CLASS INSTEAD ***
//
// Note: be sure that BigDecimal does not contain NaN/Infinity, because they
//  cannot be mapped to java.math.BigDecimal. (Nil is mapped to null)
//
// Author: Tamas Borbely, 2007
//

%{
#include "common/bigdecimal.h"
%}

%typemap(jni)     omnetpp::common::BigDecimal "jobject";
%typemap(jtype)   omnetpp::common::BigDecimal "java.math.BigDecimal";
%typemap(jstype)  omnetpp::common::BigDecimal "java.math.BigDecimal";
%typemap(javain)  omnetpp::common::BigDecimal "$javainput";
%typemap(javaout) omnetpp::common::BigDecimal {
   return $jnicall;
}

%typemap(in) omnetpp::common::BigDecimal {
   if ($input)
   {
      jclass cl = jenv->FindClass("java/math/BigDecimal");
      jmethodID methodID = jenv->GetMethodID(cl, "toPlainString", "()Ljava/lang/String;");
      jstring javaString = (jstring)jenv->CallObjectMethod($input, methodID);
      const char *chars = jenv->GetStringUTFChars(javaString, 0);
      $1 = BigDecimal::parse(chars);
      jenv->ReleaseStringUTFChars(javaString, chars);
   }
   else
   {
      $1 = BigDecimal::Nil;
   }
}

%typemap(out) omnetpp::common::BigDecimal {
   if ($1.isNil())
   {
      $result = NULL;
   }
   else
   {
      jclass cl = jenv->FindClass("java/math/BigDecimal");
      jmethodID methodId = jenv->GetMethodID(cl, "<init>", "(Ljava/lang/String;)V");
      $result = (jenv->NewObject(cl, methodId, jenv->NewStringUTF($1.str().c_str())));
   }
}

