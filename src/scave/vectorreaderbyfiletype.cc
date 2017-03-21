//=========================================================================
//  VECTORREADERBYFILETYPE.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Tamas Borbely, Zoltan Bojthe
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "common/opp_ctype.h"
#include "omnetpp/platdep/platmisc.h"
#include "channel.h"
#include "scaveutils.h"
#include "indexedvectorfilereader.h"
#include "sqlitevectorreader.h"
#include "sqliteresultfileutils.h"
#include "vectorreaderbyfiletype.h"

using namespace std;
using namespace omnetpp::common;

namespace omnetpp {
namespace scave {

const char *VectorReaderByFileTypeNodeType::getDescription() const
{
    return "Reads output vector files. (Detects filetype)";
}

void VectorReaderByFileTypeNodeType::getAttributes(StringMap& attrs) const
{
    attrs["filename"] = "name of the output vector file (.vec)";
    attrs["allowindexing"] = "whether adding an index on the vectordata table is allowed (true/false)";
}

Node *VectorReaderByFileTypeNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    const char *fname = attrs["filename"].c_str();
    bool allowIndexing = attrs["allowindexing"] == "true";

    Node *node = nullptr;
    if (SqliteResultFileUtils::isSqliteFile(fname))
        node = new SqliteVectorReaderNode(fname, allowIndexing);
    else
        node = new IndexedVectorFileReaderNode(fname);
    node->setNodeType(this);  // note: both classes should be prepared to accept this class as node type
    mgr->addNode(node);
    return node;
}

Port *VectorReaderByFileTypeNodeType::getPort(Node *node, const char *portname) const
{
    // vector id is used as port name
    if (SqliteVectorReaderNode *node1 = dynamic_cast<SqliteVectorReaderNode *>(node)) {
        int vectorId;
        if (!parseInt(portname, vectorId))
            throw opp_runtime_error("indexed file reader node: port should be a vector id, received: %s", portname);
        return node1->addVector(vectorId);
    }

    if (IndexedVectorFileReaderNode *node1 = dynamic_cast<IndexedVectorFileReaderNode *>(node)) {
        int vectorId;
        if (!parseInt(portname, vectorId))
            throw opp_runtime_error("indexed file reader node: port should be a vector id, received: %s", portname);
        return node1->addVector(vectorId);
    }

    throw opp_runtime_error("SqliteVectorReaderNode or IndexedVectorFileReaderNode expected");
}

}  // namespace scave
}  // namespace omnetpp
