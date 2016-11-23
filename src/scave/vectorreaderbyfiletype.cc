//=========================================================================
//  VECTORREADERBYFILETYPE.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Tamas Borbely, Zoltan Bojthe
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "common/opp_ctype.h"
#include "omnetpp/platdep/platmisc.h"
#include "channel.h"
#include "scaveutils.h"
#include "indexedvectorfilereader.h"
#include "sqlitevectorreader.h"
#include "vectorreaderbyfiletype.h"

using namespace std;
using namespace omnetpp::common;

namespace omnetpp {
namespace scave {

bool isSqliteFile(const char *fileName); // TODO add proper header file

#define LL    INT64_PRINTF_FORMAT


const char *VectorReaderByFileTypeNodeType::getDescription() const
{
    return "Reads output vector files. (Detects filetype)";
}

void VectorReaderByFileTypeNodeType::getAttributes(StringMap& attrs) const
{
    attrs["filename"] = "name of the output vector file (.vec)";
}

Node *VectorReaderByFileTypeNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    const char *fname = attrs["filename"].c_str();

    Node *node = nullptr;
    if (isSqliteFile(fname))
        node = new SqliteVectorReaderNode(fname);
    else
        node = new IndexedVectorFileReaderNode(fname);
    node->setNodeType(this);    //FIXME ????
    mgr->addNode(node);
    return node;
}

Port *VectorReaderByFileTypeNodeType::getPort(Node *node, const char *portname) const
{
    // vector id is used as port name
    if (SqliteVectorReaderNode *node1 = dynamic_cast<SqliteVectorReaderNode *>(node)) {
        VectorResult vector;
        if (!parseInt(portname, vector.vectorId))
            throw opp_runtime_error("indexed file reader node: port should be a vector id, received: %s", portname);
        return node1->addVector(vector);
    }

    if (IndexedVectorFileReaderNode *node1 = dynamic_cast<IndexedVectorFileReaderNode *>(node)) {
        VectorResult vector;
        if (!parseInt(portname, vector.vectorId))
            throw opp_runtime_error("indexed file reader node: port should be a vector id, received: %s", portname);
        return node1->addVector(vector);
    }

    throw opp_runtime_error("node type should be 'SqliteVectorReaderNode' or '?????'");
}

}  // namespace scave
}  // namespace omnetpp
