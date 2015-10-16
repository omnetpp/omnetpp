//=========================================================================
//  VECTORFILEINDEXERTEST.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <exception>

#include <scave/indexfile.h>
#include <scave/vectorfileindexer.h>
#include "testutil.h"

using namespace omnetpp;
using namespace std;

void testIndexer(const char *inputFile)
{
    if (IndexFile::isIndexFileUpToDate(inputFile))
        throw exception("Already up to date");

    VectorFileIndexer indexer;

    {
        MeasureTime m;
        indexer.generateIndex(inputFile);
    }

    if (!IndexFile::isIndexFileUpToDate(inputFile))
        throw exception("Indexing failed");
}

