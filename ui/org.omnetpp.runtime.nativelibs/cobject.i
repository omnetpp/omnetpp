//
// Object management:
// ==================
// When a C++ object gets deleted, we want to mark the corresponding
// Java wrapper object az zombie, so that Java code can throw an
// exception instead of crashing.
//
// We maintain a table of swig wrapper objects in WrapperTable in Javaenv.
// We use weak references to allow wrappers to be garbage collected.
// ev.objectDeleted() will cause the zap() method of the cObject Java
// class to be called, which sets the swigIsZombie flag. The Java code can
// test a wrapper object using isZombie(). Any method call on a zombie
// wrapper object will cause an IllegalStateException.
//
// NOTE: We must use a separate boolean and cannot just null out swigCPtr,
// because that would change the hash code of the object! (We use swigCPtr
// as hash code.) We would not be able to use them in Java HashMaps or sets.
//
// Throwing IllegalStateException on zombie object access requires
// testing swigIsZombie on every swigCPtr access. Unfortunately SWIG does
// not have a syntax for customizing swigCPtr access, so we use a Perl script
// to patch the generated Java wrapper classes (see Makefile).
//
// One complication is that derived classes contain MANY swigCPtrs:
// one for each class in the inheritance chain!!! (I.e. not only cobject
// contains swigCPtr, but all subclasses as well). Their values may differ,
// especially if multiple inheritance comes in. That's why swig doesn't
// generate a getCPtr() instance method, only a static one: it needs
// to know which ancestor class' swigCPtr you want to access!
//
// Therefore, swigCPtr MUST ALWAYS BE ACCESSED STATICALLY.
//

%typemap(javabody) cObject %{
  private long swigCPtr;
  protected boolean swigCMemOwn;
  protected boolean swigIsZombie = false;

  /* standard SWIG functions */

  protected $javaclassname(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;

    Javaenv env = Javaenv.cast(cSimulation.getActiveEnvir());
    env.swigWrapperCreated(this, this);
  }

  protected static long getCPtr($javaclassname obj) {
    return (obj == null) ? 0 : obj.swigCPtr;  // "obj.swigCPtr" gets replaced with "obj.checkZombie(obj.swigCPtr)" in all classes during the build process
  }

  /* zombie detection */

  protected void zap() {  // called from C++ code via JNI (WrapperTable) when C++ object gets deleted - does not need to be public
    swigIsZombie = true; // don't touch swigCPtr -- it would change the object's hash code!
  }

  /* optional: override finalize() and add:
    if (swigCPtr != 0) {
      Javaenv env = Javaenv.cast(cSimulation.getActiveEnvir());
      env.swigWrapperFinalized(this, this);
    }
  */

  public boolean isZombie() { // returns true if the underlying C++ object has already been deleted
    return swigIsZombie;
  }

  protected long checkZombie(long cPtr) { // used with all swigCPtr accesses (wrapper classes get patched by the build process: every "swigCPtr" becomes "checkZombie(swigCPtr)")
    if (isZombie())
      throw new java.lang.IllegalStateException("underlying C++ object already deleted");
    return cPtr; // note: cannot return swigCPtr because there are many of them (one per ancestor class)
  }

  public cObject disown() {
    swigCMemOwn = false;
    return this;
  }

  /* utility functions */

  public cObject[] getChildObjects() {
    cCollectChildrenVisitor visitor = new cCollectChildrenVisitor(this);
    visitor.process(this);
    int m = visitor.getArraySize();
    cObject[] result2 = new cObject[m];
    for (int i=0; i<m; i++)
      result2[i] = visitor.get(i);
    return result2;
  }

  @Override
  public boolean equals(Object obj) {
    if (this == obj)
      return true;
    if (obj == null)
      return false;
    if (!(obj instanceof cObject))  // note: cannot do getClass()==obj.getClass() because we don't have polymorphic return types! (e.g. getOwner() always returns a cObject)
      return false;
    return swigCPtr == ((cObject)obj).swigCPtr;
  }

  @Override
  public int hashCode() {
    return (int)((swigCPtr>>2)&0xffffffff) + 31*(int)(swigCPtr >> 34);
  }

  @Override
  public String toString() {
    if (isZombie())
        return getClass().getSimpleName() + "<deleted>";
    return "("+getClassName()+")"+getFullPath();
  }
%}

%extend cObject {
  bool hasChildObjects() {
    cCollectChildrenVisitor visitor(self);
    visitor.setSizeLimit(1);
    visitor.process(self);
    return visitor.getArraySize() > 0;
  }
}


