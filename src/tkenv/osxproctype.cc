#include <Carbon/Carbon.h>

// required to display the menu correctly on Mac OS X 10.5
void OSXTransformProcess() 
{ 
    ProcessSerialNumber psn;
    GetCurrentProcess( &psn );
    TransformProcessType( &psn, kProcessTransformToForegroundApplication);
} 

