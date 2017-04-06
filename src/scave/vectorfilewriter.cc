//=========================================================================
//  VECTORFILEWRITER.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdlib>
#include <clocale>

#include "common/stringutil.h"
#include "omnetpp/platdep/platmisc.h"  // PRId64
#include "channel.h"
#include "vectorfilewriter.h"

#define VECTOR_FILE_VERSION    2

#ifdef CHECK
#undef CHECK
#endif

using namespace omnetpp::common;

namespace omnetpp {
namespace scave {

#define CHECK(fprintf)    if ((fprintf)<0) throw opp_runtime_error("Cannot write output vector file '%s'", fileName.c_str())

VectorFileWriterNode::VectorFileWriterNode(const char *fileName, const char *fileHeader)
{
    f = nullptr;
    this->prec = DEFAULT_RESULT_PRECISION;
    this->fileName = fileName;
    this->fileHeader = (fileHeader ? fileHeader : "");
}

VectorFileWriterNode::~VectorFileWriterNode()
{
}

Port *VectorFileWriterNode::addVector(const VectorResult& vector)
{
    ports.push_back(Pair(vector.getVectorId(), vector.getModuleName().c_str(), vector.getName().c_str(), vector.getColumns().c_str(), this));
    return &(ports.back().port);
}

bool VectorFileWriterNode::isReady() const
{
    for (PortVector::const_iterator it = ports.begin(); it != ports.end(); ++it)
        if (it->port()->length() > 0)
            return true;

    return false;
}

void VectorFileWriterNode::process()
{
    // open file if needed
    if (!f) {
        f = fopen(fileName.c_str(), "w");
        if (!f)
            throw opp_runtime_error("Cannot open '%s' for write", fileName.c_str());

        setlocale(LC_NUMERIC, "C");

        // print file header and vector declarations
        CHECK(fprintf(f, "%s\n\n", fileHeader.c_str()));
        CHECK(fprintf(f, "version %d\n", VECTOR_FILE_VERSION));

        for (PortVector::iterator it = ports.begin(); it != ports.end(); ++it)
            CHECK(fprintf(f, "vector %d  %s  %s  %s\n", it->id,
                            QUOTE(it->moduleName.c_str()),
                            QUOTE(it->name.c_str()), it->columns.c_str()));
    }

    for (PortVector::iterator it = ports.begin(); it != ports.end(); ++it) {
        Channel *chan = it->port();
        int n = chan->length();
        std::string& columns = it->columns;
        int colno = columns.size();
        Datum a;
        char buf[64];
        char *endp;

        if (colno == 2 && columns[0] == 'T' && columns[1] == 'V') {
            for (int i = 0; i < n; i++) {
                chan->read(&a, 1);
                if (a.xp.isNil()) {
                    CHECK(fprintf(f, "%d\t%.*g\t%.*g\n", it->id, prec, a.x, prec, a.y));
                }
                else {
                    CHECK(fprintf(f, "%d\t%s\t%.*g\n", it->id, BigDecimal::ttoa(buf, a.xp, endp), prec, a.y));
                }
            }
        }
        else if (colno == 3 && columns[0] == 'E' && columns[1] == 'T' && columns[2] == 'V') {
            for (int i = 0; i < n; i++) {
                chan->read(&a, 1);
                if (a.xp.isNil()) {
                    CHECK(fprintf(f, "%d\t%" PRId64 "\t%.*g\t%.*g\n", it->id, a.eventNumber, prec, a.x, prec, a.y));
                }
                else {
                    CHECK(fprintf(f, "%d\t%" PRId64 "\t%s\t%.*g\n", it->id, a.eventNumber, BigDecimal::ttoa(buf, a.xp, endp), prec, a.y));
                }
            }
        }
        else {
            for (int i = 0; i < n; i++) {
                chan->read(&a, 1);
                CHECK(fprintf(f, "%d", it->id));
                for (int j = 0; j < colno; ++j) {
                    CHECK(fputc('\t', f));
                    switch (columns[j]) {
                        case 'T':
                            if (a.xp.isNil()) {
                                CHECK(fprintf(f, "%.*g", prec, a.x));
                            }
                            else {
                                CHECK(fprintf(f, "%s", BigDecimal::ttoa(buf, a.xp, endp)));
                            }
                            break;

                        case 'V':
                            CHECK(fprintf(f, "%.*g", prec, a.y));
                            break;

                        case 'E':
                            CHECK(fprintf(f, "%" PRId64, a.eventNumber));
                            break;

                        default:
                            throw opp_runtime_error("Unknown column type: '%c' while writing %s", columns[j], fileName.c_str());
                    }
                }
                CHECK(fputc('\n', f));
            }
        }
    }
}

bool VectorFileWriterNode::isFinished() const
{
    for (PortVector::const_iterator it = ports.begin(); it != ports.end(); ++it)
        if (!it->port()->isClosing() || it->port()->length() > 0)
            return false;

    return true;
}

//-------

const char *VectorFileWriterNodeType::getDescription() const
{
    return "Writes the output (several streams) into an output vector file.";
}

void VectorFileWriterNodeType::getAttributes(StringMap& attrs) const
{
    attrs["filename"] = "name of the output vector file (.vec)";
}

Node *VectorFileWriterNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    const char *fileName = attrs["filename"].c_str();

    Node *node = new VectorFileWriterNode(fileName);
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}

Port *VectorFileWriterNodeType::getPort(Node *node, const char *portname) const
{
/*FIXME THIS CODE IS COMPLETELY BROKEN --ANDRAS
    // vector id is used as port name
    VectorFileWriterNode *node1 = dynamic_cast<VectorFileWriterNode *>(node);
    VectorResult vector;
    std::string moduleName = "n/a", name = "n/a";
    vector.vectorId = atoi(portname);  // FIXME check it's numeric at all
    vector.getModuleName() = &moduleName;
    vector.getName() = &name;
    vector.columns = "TV";  // old vector file format
    return node1->addVector(vector);
*/
    return nullptr;
}

}  // namespace scave
}  // namespace omnetpp

