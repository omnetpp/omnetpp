
%pragma(java) jniclasscode=%{
  static {
//    try {
      System.loadLibrary("opplibs");
//    } catch (UnsatisfiedLinkError e) {
//      System.err.println("Native code library failed to load.\n" + e);
//      System.exit(1);
//    }
  }
%}


