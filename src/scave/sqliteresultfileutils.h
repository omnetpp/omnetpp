//=========================================================================
//  SQLITERESULTFILEUTILS.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_SQLITERESULTFILEUTILS_H
#define __OMNETPP_SCAVE_SQLITERESULTFILEUTILS_H

#include <map>
#include <string>
#include "scavedefs.h"
#include "common/sqlite3.h"


namespace omnetpp {
namespace scave {

class SCAVE_API SqliteResultFileUtils
{
    public:
        static bool isSqliteFile(const char *fileName);
        static void addIndexToVectorData(const char *fileName); // unless index already exists
};


}  // namespace scave
}  // namespace omnetpp


#endif
