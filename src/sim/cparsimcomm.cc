//=========================================================================
//  CPARCOMM.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Written by:  Andras Varga, 2003
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2003-2005 Andras Varga
  Monash University, Dept. of Electrical and Computer Systems Eng.
  Melbourne, Australia

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include "errmsg.h"
#include "ccommbuffer.h"
#include "cparsimcomm.h"
#include "cenvir.h"
#include "cexception.h"


void cParsimCommunications::broadcast(cCommBuffer *buffer, int tag)
{
    // Default implementation: send to everyone. Try to do as much of the job
    // as possible -- if there're exceptions, throw on only one of them.
    cException *ex = NULL;
    int n = getNumPartitions();
    int myProcId = getProcId();
    for (int i=0; i<n; i++)
    {
        try
        {
            if (myProcId != i)
                send(buffer, tag, i);
        }
        catch (cException *e)
        {
            if (ex)
               delete e;
            else
               ex = e;
        }
    }

    if (ex)
        throw ex;
}


