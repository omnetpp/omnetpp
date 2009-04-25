
%{
static void opp_JavaThrowException(JNIEnv *jenv, cException& e) {
    // wrap a copy of the C++ exception object into a Java exception, and throw it
    jenv->ExceptionClear();
    jclass clazz = jenv->FindClass("org/omnetpp/experimental/simkernel/WrappedException");
    jmethodID ctor = jenv->GetMethodID(clazz, "<init>", "(J)V");
    jobject excep = jenv->NewObject(clazz, ctor, (jlong)(void*)e.dup());
    jenv->Throw((jthrowable&)excep);
}
%}

%exception {
    try {
        $action
    }
    catch (cException& e) {
        opp_JavaThrowException(jenv, e);
        return $null;
    }
    catch (std::exception& e) {
        SWIG_JavaThrowException(jenv, SWIG_JavaRuntimeException, const_cast<char*>(e.what()));
        return $null;
    }
}

%typemap(javacode) cException %{
  // public constructor
  public cException(long cPtr) {
    this(cPtr, true);
  }
%}

