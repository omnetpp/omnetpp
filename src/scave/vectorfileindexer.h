//=========================================================================
//  VECTORFILEINDEXER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Tamas Borbely
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _IVECTORFILEINDEXER_H_
#define _IVECTORFILEINDEXER_H_

#include <string>
#include "resultfilemanager.h"
#include "progressmonitor.h"

NAMESPACE_BEGIN

/**
 * FIXME comment
 */
class SCAVE_API VectorFileIndexer
{
    public:
        void generateIndex(const char *filename, IProgressMonitor *monitor = NULL);

        void rebuildVectorFile(const char *filename, IProgressMonitor *monitor = NULL);
};

NAMESPACE_END


#endif

