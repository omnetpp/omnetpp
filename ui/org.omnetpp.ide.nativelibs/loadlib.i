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
        throw opp_runtime_error("if you see this exception, omnetpp's native library (\"opplibs\") was built incorrectly"); 
    } catch (std::exception&) { /*OK!*/ }
}
%}
void testExceptionHandling();


%pragma(java) jniclasscode=%{
    static {
        // we might catch UnsatisfiedLinkError here, but Eclipse will display it a dialog and log it anyway.
        System.loadLibrary("opplibs");
        testExceptionHandling();
    }
%}


