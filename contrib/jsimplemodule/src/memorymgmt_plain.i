%typemap(javabody) SWIGTYPE %{
  private long swigCPtr;

  protected $javaclassname(long cPtr, boolean dummy) {
    swigCPtr = cPtr;
  }

  public static long getCPtr($javaclassname obj) {
    return obj==null ? 0 : obj.swigCPtr;
  }

  public long getBaseCPtr() {
    return swigCPtr;
  }

  public boolean sameAs($javaclassname obj) {
    return getBaseCPtr() == obj.getBaseCPtr();
  }

  public boolean equals(Object obj) {
    return (obj instanceof $javaclassname) && sameAs(($javaclassname)obj);
  }

  @Override
  public int hashCode() {
    return (int)swigCPtr;
  }

  protected void setCPtr(long cPtr) {
    swigCPtr = cPtr;
  }
%}

%typemap(javabody_derived) SWIGTYPE %{
  private long swigCPtr;

  protected $javaclassname(long cPtr, boolean dummy) {
    super(SimkernelJNI.SWIG$javaclassnameUpcast(cPtr), false);
    swigCPtr = cPtr;
  }

  public static long getCPtr($javaclassname obj) {
    return obj==null ? 0 : obj.swigCPtr;
  }

  protected void setCPtr(long cPtr) {
    swigCPtr = cPtr;
    super.setCPtr(cPtr==0 ? 0 : SimkernelJNI.SWIG$javaclassnameUpcast(cPtr));
  }
%}

%typemap(javaconstruct) SWIGTYPE {
    this($imcall, false);
  }

%typemap(javafinalize) SWIGTYPE %{
%}

%typemap(javadestruct, methodname="delete") SWIGTYPE {
    if (swigCPtr != 0) {
      $jnicall;
      setCPtr(0);
    }
  }

%typemap(javadestruct_derived, methodname="delete") SWIGTYPE {
    if (swigCPtr != 0) {
      $jnicall;
      setCPtr(0);
    }
  }

// The following BASECLASS(), DERIVEDCLASS() macros are applied in simkernel.i:
%define BASECLASS(CLASS)
%enddef

%define DERIVEDCLASS(CLASS,BASECLASS)
%extend CLASS {
  static CLASS *castFrom(BASECLASS *obj) {return dynamic_cast<CLASS *>(obj);}
}
%enddef


