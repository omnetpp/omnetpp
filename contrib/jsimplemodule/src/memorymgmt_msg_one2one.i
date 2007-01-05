//
// Notes about SWIG:
//   Every wrapper object contains SEVERAL swigCPtrs, one for each subclass!!!
//   They may even be different in value, because as C++ casts may numerically
//   change the pointer value, esp if multiple inheritance is in use.
//

//
// PLANNED changes to normal SWIG proxy object behaviour (NOT WORKING YET!):
//
//  1. Java object identity. By default, SWIG creates a new Java proxy
//     object every time a C++ method returns an object. Here we
//     return the same proxy object every time, from a hashtable that
//     maps a C++ pointer to the Java proxy object. This functionality
//     is provided by the ProxyObjectMap class.
//
//     *** PROBLEM ***:
//          When we get a C++ object to wrap, we look up cPtr in the hashtable.
//          How do we know it's the same object for which we have the wrapper
//          object in the table, or is it a new object allocated on the same
//          memory address after the original object got deleted???
//
//          Two possible solutions:
//            1. C++ destructors (ie cPolymorphic destructor) should remove the
//               cPtr from the Java hashtable. TOO COSTLY? Not if we add a
//               hasJavaProxy flag into the C++ class...
//               REQUIRES MODIFICATION TO OMNET++ (modifying cPolymorphic dtor).
//
//            2. Add unique IDs to C++ objects to identify them. I.e. into the
//               ctor: id = lastId++. Then, hashtable lookup would also check
//               that object ID matches.
//               REQUIRES MODIFICATION TO OMNET++ (adding ID to cPolymorphic.)
//
//  2. Polymorphic return types. When a method returns an object, we create
//     it into a Java proxy object that corresponds to its actual type,
//     not the declared return type of the method. We derive the Java class
//     as "org.omnetpp.simkernel.<cplusplus-classname>".
//
//     TOO COSTLY? (Class.forName() each time we return an object!) Java object
//     identity would help (less frequent need for wrapping).
//

%typemap(javabody) SWIGTYPE %{
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected $javaclassname(long cPtr, boolean cMemoryOwn) {
    this(cPtr, cMemoryOwn, (byte)0); // go to another ctor which doesn't call swigSetAsProxyObject() in base classes
    if (cMemoryOwn)
      swigSetAsProxyObject(this);
  }

  protected $javaclassname(long cPtr, boolean cMemoryOwn, byte dummy) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  public static long getCPtr($javaclassname obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  public void swigDisown() {
    swigCMemOwn = false;
  }

  public void swigSetMemOwn(boolean value) {
    swigCMemOwn = value;
  }

  public long getBaseCPtr() {
    return swigCPtr;
  }

  @Override
  public int hashCode() {
    return (int)swigCPtr;
  }
%}

%typemap(javabody_derived) SWIGTYPE %{
  private long swigCPtr;

  protected $javaclassname(long cPtr, boolean cMemoryOwn) {
    this(SimkernelJNI.SWIG$javaclassnameUpcast(cPtr), cMemoryOwn, (byte)0); // go to another ctor which doesn't call swigSetAsProxyObject() in base classes
    if (cMemoryOwn)
      swigSetAsProxyObject(this);
  }

  protected $javaclassname(long cPtr, boolean cMemoryOwn, byte dummy) {
    super(SimkernelJNI.SWIG$javaclassnameUpcast(cPtr), cMemoryOwn, (byte)0);
    swigCPtr = cPtr;
  }

  public static long getCPtr($javaclassname obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }
%}

%typemap(javaout) cMessage* {
    long cPtr = $jnicall;
    if (cPtr == 0)
      return null;
    // @notnull@ -- do not delete this mark
    return cMessage.wrap(cPtr, $owner);
  };

%typemap(javaout) SWIGTYPE, SWIGTYPE*, SWIGTYPE& {
    long cPtr = $jnicall;
    if (cPtr == 0)
      return null;
    // @notnull@ -- do not delete this mark
    return new $javaclassname(cPtr, $owner);
  };

%typemap(javadestruct_derived, methodname="delete", methodmodifiers="public synchronized") cMessage {
    if(swigCPtr != 0)
      swigFinalizeProxyObject();
    if(swigCPtr != 0 && swigCMemOwn) {
      swigCMemOwn = false;
      $jnicall;
    }
    swigCPtr = 0;
  }

%typemap(javafinalize) SWIGTYPE %{
  protected void finalize() {
    Object self = this;
    if (self instanceof cObject)
        System.out.println("FINALIZING "+toString()+"  cPtr="+Long.toHexString(((cObject)self).getBaseCPtr()));
    delete();
  }
%}

//
// CURRENT SOLUTION: no object identity, no polymorphic return types.
//
//  - isSameAs(object) method can be used to test for object identity;
//    BEWARE: it only checks cptrs, and thus can falsely report "true"
//    if one wrapper object refers to a C++ object which *previously*
//    existed on the same memory location!
//
//  - cMessage.castFrom(object) can be used to cast to subtypes.
//    castFrom() also does transfer of ownership, so the original
//    "object" pointer SHOULD NOT be referenced afterwards!
//
/*
%typemap(javabody) SWIGTYPE %{
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected $javaclassname(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  public static long getCPtr($javaclassname obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  public long getBaseCPtr() {
    return swigCPtr;
  }

  public boolean isSameAs($javaclassname obj) {
    return getBaseCPtr() == obj.getBaseCPtr();
  }

  public boolean equals(Object obj) {
    return (obj instanceof $javaclassname) && isSameAs(($javaclassname)obj);
  }

  public void swigDisown() {
    swigCMemOwn = false;
  }

  @Override
  public int hashCode() {
    return (int)swigCPtr;
  }
%}
*/

%extend cMessage {
  jobject swigProxyObject() {
      printf("returning JOBJECT=%p\n", self->contextPointer());fflush(stdout);
      return (jobject)(self->contextPointer());
  }
  void swigSetAsProxyObject(jobject obj) {
      if (self->contextPointer()!=0) printf("WARNING: swigSetAsProxyObject: already set!!!\n");
      obj = JSimpleModule::jenv->NewWeakGlobalRef(obj);
      self->setContextPointer((void *)obj);
      printf("remembering JOBJECT=%p\n", self->contextPointer());fflush(stdout);
  }
  void swigFinalizeProxyObject() {
      printf("finalizing JOBJECT=%p\n", self->contextPointer());fflush(stdout);
      if (self->contextPointer()==0) printf("WARNING: swigFinalizeProxyObject: ref is already null!!!\n");
      jobject obj = (jobject)(self->contextPointer());
      JSimpleModule::jenv->DeleteWeakGlobalRef(obj);
      self->setContextPointer(NULL);
  }
}

%typemap(javacode) cMessage %{
  public static cMessage wrap(long cPtr, boolean memOwn) {
    if (cPtr == 0)
      return null;
    // use existing proxy object if there is one
    $javaclassname tmp = new $javaclassname(cPtr, false); // false == don't call swigSetAsProxyObject()
    $javaclassname proxy = ($javaclassname) tmp.swigProxyObject();
    if (proxy!=null) {
      // return existing proxy object
      System.out.println("cMessage.wrap ---- "+cPtr+"==>"+proxy+"   [existing wrapper]");
      proxy.swigSetMemOwn(memOwn);
      return proxy;
    }
    else {
      // make tmp the proxy object
      System.out.println("cMessage.wrap ---- "+cPtr+"==>"+tmp+"   [new proxy object]");
      tmp.swigSetAsProxyObject(tmp);
      tmp.swigSetMemOwn(memOwn);
      return tmp;
    }
  }
%}

%define DERIVEDCLASS(CLASS,BASECLASS)
%extend CLASS {
  //static int swigBaseClassOffset() {CLASS *p = (CLASS *)0x10000; return (int)(static_cast<BASECLASS *>(p)) - 0x10000;}
  static CLASS *castFrom(BASECLASS *obj) {return dynamic_cast<CLASS *>(obj);}
}
%enddef

%define BASECLASS(CLASS)
%extend CLASS {
  //static int swigBaseClassOffset() {return 0;}
  jobject swigProxyObject()              {return 0;}
  void swigSetAsProxyObject(jobject obj) {}
  void swigFinalizeProxyObject()         {}
}
%enddef

BASECLASS(cPolymorphic);
BASECLASS(cDisplayString);
BASECLASS(cEnvir);
BASECLASS(cException);
BASECLASS(cExpression);
BASECLASS(cSubModIterator);
BASECLASS(cVisitor);
BASECLASS(cXMLElement);
BASECLASS(std::vector<cXMLElement*>);
//BASECLASS(std::map<std::string,std::string>);
%extend std::map<std::string,std::string> {
  //static int swigBaseClassOffset() {return 0;}
  jobject swigProxyObject()              {return 0;}
  void swigSetAsProxyObject(jobject obj) {}
  void swigFinalizeProxyObject()         {}
}
DERIVEDCLASS(cArray, cPolymorphic);
DERIVEDCLASS(cBasicChannel, cPolymorphic);
DERIVEDCLASS(cChannel, cPolymorphic);
DERIVEDCLASS(cChannelType, cPolymorphic);
DERIVEDCLASS(cCompoundModule, cPolymorphic);
DERIVEDCLASS(cDefaultList, cPolymorphic);
DERIVEDCLASS(cDoubleExpression, cExpression);
DERIVEDCLASS(cGate, cPolymorphic);
DERIVEDCLASS(cMessage, cPolymorphic);
DERIVEDCLASS(cModule, cPolymorphic);
DERIVEDCLASS(cModulePar, cPolymorphic);
DERIVEDCLASS(cModuleType, cPolymorphic);
DERIVEDCLASS(cNetworkType, cPolymorphic);
DERIVEDCLASS(cObject, cPolymorphic);
DERIVEDCLASS(cOutVector, cPolymorphic);
DERIVEDCLASS(cPar, cPolymorphic);
DERIVEDCLASS(cPolymorphic, cPolymorphic);
DERIVEDCLASS(cQueue, cPolymorphic);
DERIVEDCLASS(cRuntimeError, cException);
DERIVEDCLASS(cSimpleModule, cPolymorphic);
DERIVEDCLASS(cSimulation, cPolymorphic);
DERIVEDCLASS(cStatistic, cPolymorphic);
DERIVEDCLASS(cStdDev, cPolymorphic);
DERIVEDCLASS(cWeightedStdDev, cPolymorphic);


// memory management (see also patchownership.pl)
%newobject cMessage::decapsulate;
%newobject cArray::remove;
%newobject cMessage::removeObject;
%newobject cMessage::removeControlInfo;
%newobject cQueue::remove;
%newobject cQueue::pop;
