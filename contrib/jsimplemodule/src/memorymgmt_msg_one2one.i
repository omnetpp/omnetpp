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
  // @METHODARGS-OWNERSHIP@, @CASTFROM-OWNERSHIP@  -- do not delete this line
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

  protected void setCPtr(long cPtr) {
    swigCPtr = cPtr;
  }

  /** Not needed for cMessages with this memory management scheme, only for other objects */
  public boolean sameAs($javaclassname obj) {
    return getBaseCPtr() == obj.getBaseCPtr();
  }

  @Override
  public int hashCode() {
    return (int)swigCPtr;
  }
%}

%typemap(javabody_derived) SWIGTYPE %{
  // @METHODARGS-OWNERSHIP@, @CASTFROM-OWNERSHIP@  -- do not delete this line
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

  protected void setCPtr(long cPtr) {
    swigCPtr = cPtr;
    super.setCPtr(cPtr==0 ? 0 : SimkernelJNI.SWIG$javaclassnameUpcast(cPtr));
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

// The following BASECLASS(), DERIVEDCLASS() macros are applied in simkernel.i:
%define BASECLASS(CLASS)
%extend CLASS {
  //static int swigBaseClassOffset() {return 0;}
  jobject swigProxyObject()              {return 0;}
  void swigSetAsProxyObject(jobject obj) {}
  void swigFinalizeProxyObject()         {}
}
%enddef

%define DERIVEDCLASS(CLASS,BASECLASS)
%extend CLASS {
  //static int swigBaseClassOffset() {CLASS *p = (CLASS *)0x10000; return (int)(static_cast<BASECLASS *>(p)) - 0x10000;}
  static CLASS *castFrom(BASECLASS *obj) {return dynamic_cast<CLASS *>(obj);}
}
%enddef


// ...except this one which has to be here, because the type name contains a comma which confuses the SWIG preprocessor
%extend std::map<std::string,std::string> {
  //static int swigBaseClassOffset() {return 0;}
  jobject swigProxyObject()              {return 0;}
  void swigSetAsProxyObject(jobject obj) {}
  void swigFinalizeProxyObject()         {}
}

// memory management (see also patchownership.pl)
%newobject cMessage::decapsulate;
%newobject cArray::remove;
%newobject cMessage::removeObject;
%newobject cMessage::removeControlInfo;
%newobject cQueue::remove;
%newobject cQueue::pop;


