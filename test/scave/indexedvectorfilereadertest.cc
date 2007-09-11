//=========================================================================
//  INDEXEDVECTORFILEREADERTEST.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <exception>
#include <cstdlib>

#include "resultfilemanager.h"
#include "idlist.h"
#include "node.h"
#include "nodetype.h"
#include "nodetyperegistry.h"
#include "dataflowmanager.h"
#include "indexedvectorfilereader.h"
#include "arraybuilder.h"
#include "testutil.h"

using namespace std;

void testIndexedReader(const char *inputFile, int *vectorIds, int count)
{
	if (!IndexFile::isIndexFileUpToDate(inputFile))
		throw exception("Index file is not up to date");
	
    NodeTypeRegistry *registry = NodeTypeRegistry::instance();
    NodeType *readerType = registry->getNodeType("indexedvectorfilereader");
    NodeType *builderType = registry->getNodeType("arraybuilder");

    DataflowManager manager;
    StringMap attrs;
    attrs["filename"] = inputFile;
    Node *reader = readerType->create(&manager, attrs);

    attrs.clear();
    char vectorIdStr[12];
    
    for (size_t i = 0; i < count; ++i)
    {
        Node *builder = builderType->create(&manager, attrs);
        Port *src = readerType->getPort(reader, itoa(vectorIds[i], vectorIdStr, 10));
        Port *dest = builderType->getPort(builder, "in");
        manager.connect(src, dest);
    }
    
    {
    	MeasureTime m;
    	manager.execute();
    }
}
