//==========================================================================
// nederror.h  - part of the OMNeT++ Discrete System Simulation System
//
// Contents:
//   nedError function
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __NEDERROR_H
#define __NEDERROR_H

#include "nedelement.h"

void NEDError(NEDElement *where, const char *format, ...);

#endif

