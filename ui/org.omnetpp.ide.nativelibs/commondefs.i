%{
#include "bigdecimal.h"
#include "jprogressmonitor.h"
%}

/*--------------------------------------------------------------------------
 * int64 <--> long mapping
 *--------------------------------------------------------------------------*/
%include "inttypes.h"

%typemap(jni)    int64 "jlong"
%typemap(jtype)  int64 "long"
%typemap(jstype) int64 "long"
%typemap(javain) int64 "$javainput"
%typemap(javaout) int64 {
   return $jnicall;
}

/*--------------------------------------------------------------------------
 * IProgressMonitor
 *--------------------------------------------------------------------------*/
%typemap(jni)    IProgressMonitor * "jobject"
%typemap(jtype)  IProgressMonitor * "org.eclipse.core.runtime.IProgressMonitor"
%typemap(jstype) IProgressMonitor * "org.eclipse.core.runtime.IProgressMonitor"
%typemap(javain) IProgressMonitor * "$javainput"
%typemap(in) IProgressMonitor * (JniProgressMonitor jProgressMonitor) {
   if ($input)
   {
      jProgressMonitor = JniProgressMonitor($input, jenv);
      $1 = &jProgressMonitor;
   }
   else
   {
      $1 = NULL;
   }
}

/*--------------------------------------------------------------------------
 * use JAVA_MATH_BIGDECIMAL() if you want to map BigDecimals
 *                            to java.math.BigDecimal
 * WARN: be sure that BigDecimal does not contain NaN/Infinity, because they
 *       cannot be mapped to java.math.BigDecimal. (Nil is mapped to null)
 *--------------------------------------------------------------------------*/
%define JAVA_MATH_BIGDECIMAL()
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
%enddef // JAVA_MATH_BIGDECIMAL

/*--------------------------------------------------------------------------
 * use COMMON_ENGINE_BIGDECIMAL() if you want to map BigDecimals
 *                                to org.omnetpp.common.engine.BigDecimal
 * The C++ objects are wrapped by the java object, but BigDecimal::Nil
 * mapped to null.
 *--------------------------------------------------------------------------*/
%define COMMON_ENGINE_BIGDECIMAL()
class BigDecimal;
%typemap(jstype) BigDecimal "org.omnetpp.common.engine.BigDecimal";
%typemap(javain) BigDecimal "org.omnetpp.common.engine.BigDecimal.getCPtr($javainput)"
%typemap(javaout) BigDecimal {
    long cPtr = $jnicall;
    return (cPtr == 0) ? null : new org.omnetpp.common.engine.BigDecimal(cPtr, true);
}
%typemap(in) BigDecimal ($&1_type argp) %{
   argp = *($&1_ltype*)(void *)&$input; 
   if (!argp) {
      $1 = BigDecimal::Nil;
   }
   else {
      $1 = *argp;
   }
%}

%typemap(out) BigDecimal %{
   if ($1.isNil())
   {
      $result = 0;
   }
   else
   {
      {*($&1_ltype*)(void *)&$result = new $1_ltype(($1_ltype &)$1); }
   }
%}

%typemap(jstype) BigDecimal* "org.omnetpp.common.engine.BigDecimal";
%typemap(javain) BigDecimal* "org.omnetpp.common.engine.BigDecimal.getCPtr($javainput)"
%typemap(javaout) BigDecimal* {
    long cPtr = $jnicall;
    return (cPtr == 0) ? null : new org.omnetpp.common.engine.BigDecimal(cPtr, $owner);
}
%typemap(in) BigDecimal* %{
   if (!$input) {
      $1 = &BigDecimal::Nil;
   }
   else {
      $1 = *($1_ltype*)(void*)&$input;
   }
%}
%typemap(out) BigDecimal* %{
   if (!$1 || $1->isNil())
   {
      $result = 0;
   }
   else
   {
      *($1_ltype*)(void *)&$result = $1;
   }
%}

%typemap(jstype) BigDecimal& "org.omnetpp.common.engine.BigDecimal";
%typemap(javain) BigDecimal& "org.omnetpp.common.engine.BigDecimal.getCPtr($javainput)"
%typemap(javaout) BigDecimal& {
    long cPtr = $jnicall;
    return (cPtr == 0) ? null : new org.omnetpp.common.engine.BigDecimal(cPtr, false);
}
%typemap(in) BigDecimal& %{
   if (!$input) {
      $1 = ($1_ltype)(void *)&BigDecimal::Nil;
   }
   else {
      $1 = *($1_ltype*)(void *)&$input;
   }
%}
%typemap(out) BigDecimal& %{
   if ($1->isNil())
   {
      $result = 0;
   }
   else
   {
      {*($1_ltype*)(void *)&$result = $1; }
   }
%}
%enddef // COMMON_ENGINE_BIGDECIMAL

/*
 *
 */
%define ADD_CPTR_EQUALS_AND_HASHCODE(CLASS)
%typemap(javacode) CLASS %{
  public boolean equals(Object obj) {
    if (this == obj)
      return true;
    if (obj == null || this.getClass() != obj.getClass())
      return false;
    return getCPtr(this) == getCPtr((CLASS)obj);
  }

  public int hashCode() {
    return (int)getCPtr(this);
  }
%}
%enddef 
