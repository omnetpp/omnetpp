//==========================================================================
//  OSXPROCTYPE.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <Carbon/Carbon.h>

//
// The following is required to display the menu correctly on Mac OS X 10.5.
// An alternative solution would be to create a bundled application;
// as much as putting the executable into a directory called Contents/MacOS,
// or even creating Contents directory next to the executable would do,
// but its not an elegant solution.
//
void OSXTransformProcess()
{
    ProcessSerialNumber psn;
    GetCurrentProcess( &psn );
    TransformProcessType( &psn, kProcessTransformToForegroundApplication);
}
