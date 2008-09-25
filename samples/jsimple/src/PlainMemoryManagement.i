//
// Simple memory management:
//    no object identity, no polymorphic return types, explicit deletion.
//
// swigCMemOwn got removed, Java proxy objects never delete anything in
// finalize(). All C++ objects have to be explicitly deallocated:
// msg.delete(), queue.delete().
//
// Object identity testing can be done via one.sameAs(other).
//
// Casts can be done with the cast() method: msg = cMessage.cast(obj).
//

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

%typemap(javadestruct, methodname="delete", methodmodifiers="public") SWIGTYPE {
    if (swigCPtr != 0) {
      setCPtr(0);
      $jnicall;
    }
  }

%typemap(javadestruct_derived, methodname="delete", methodmodifiers="public") SWIGTYPE {
    if (swigCPtr != 0) {
      setCPtr(0);
      $jnicall;
    }
  }

// The following BASECLASS(), DERIVEDCLASS() macros are applied in Simkernel.i:
%define BASECLASS(CLASS)
%ignore CLASS::CLASS(const CLASS&);
%ignore CLASS::operator=(const CLASS&);
%enddef

%define DERIVEDCLASS(CLASS,BASECLASS)
%ignore CLASS::CLASS(const CLASS&);
%ignore CLASS::operator=(const CLASS&);
%extend CLASS {
  static CLASS *cast(BASECLASS *obj) {return dynamic_cast<CLASS *>(obj);}
}
%enddef


