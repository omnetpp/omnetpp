//
// Load the native library when any method from it is referenced.
//
// Note: check exceptions before. gcc shared libs must NOT be linked with "-static",
// because throw/catch won't work (program exits with "terminate called...").
// See http://www.gnu.org/software/gcc/faq.html#dso
// As a safeguard against incorrectly built libs, we test exceptions first thing
// after loading the library.
// --Andras
//

%{
#include "exception.h"

static void testExceptionHandling() {
    try {
        throw OPP::opp_runtime_error("if you see this exception, omnetpp's native library (\"opplibs\") was built incorrectly");
    } catch (std::exception&) { /*OK!*/ }
}
%}
void testExceptionHandling();

%pragma(java) jniclassimports=%{
import org.eclipse.core.runtime.Platform;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.widgets.Display;
%}

%pragma(java) jniclasscode=%{
  static {
      if (Platform.getOS().equals(Platform.OS_WIN32)) {
          try {
              // mingwm10.dll is a dependency of opplibs.dll. We need to load it
              // explicitly, because for some reason it is not found otherwise
              // when placed next to opplibs.dll. --Andras
              System.loadLibrary("mingwm10");
          }
          catch (UnsatisfiedLinkError e) {
              System.err.println("Warning: could not load mingwm10.dll: " + e.getClass().getName() + ": " + e.toString());
          }
      }
      try {
          System.loadLibrary("opplibs");
      }
      catch (UnsatisfiedLinkError e) {
          displayError(e);
          throw e;
      }
      testExceptionHandling();
      testThreads();
  }

  private static void testThreads() {
      // test that exception handling is thread-safe in our shared lib (gcc -mthreads)
      for (int i=0; i<3; i++) {
          Thread t = new Thread() {
              @Override
              public void run() {
                  long start = System.currentTimeMillis();
                  while (System.currentTimeMillis() - start < 50)
                      testExceptionHandling();
              }
          };
          t.start();
      }
  }

  private static void displayError(final UnsatisfiedLinkError e) {
      runNowOrSyncInUIThread(new Runnable() {
          public void run() {
              MessageDialog.openError(null, "Fatal Error",
                      "FATAL: The OMNeT++ IDE native library failed to load, " +
                      "all OMNeT++-related functionality will be unaccessible.\n\n" +
                      "Details:\n\n" +
                      e.getClass().getSimpleName() + ": " + e.getMessage() + "\n\n" +
                      (Platform.getOS().equals(Platform.OS_LINUX) ?
                              "Try upgrading your Linux installation to a more recent version, " +
                              "or installing newer versions of libc and libstdc++." : "")
              );
          }
      });
  }

  private static void runNowOrSyncInUIThread(Runnable runnable) {
      if (Display.getCurrent() == null)
          Display.getDefault().syncExec(runnable);
      else
          runnable.run();
  }
%}


