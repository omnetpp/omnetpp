//=========================================================================
//  OCTAVEEXPORT.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _OCTAVEEXPORT_H_
#define _OCTAVEEXPORT_H_

#include "xyarray.h"

/**
 * Exports data in Octave's "save -text" format.
 */
class OctaveExport
{
    private:
    public:
        OctaveExport();
        virtual ~OctaveExport();
};

#endif


