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

%typemap(jni) BigDecimal "jobject";
%typemap(jtype) BigDecimal "java.math.BigDecimal";
%typemap(jstype) BigDecimal "java.math.BigDecimal";
%typemap(javain) BigDecimal "$javainput";
%typemap(javaout) BigDecimal {
   return $jnicall;
}

%typemap(in) BigDecimal {
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

%typemap(out) BigDecimal {
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

