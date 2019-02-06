//=========================================================================
//  VECTORFILEREADERTEST.CC - part of
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
#include <iostream>
#include <fstream>
#include <set>
#include <vector>

#include <common/exception.h>
#include <scave/resultfilemanager.h>
#include <scave/idlist.h>
#include <scave/node.h>
#include <scave/nodetype.h>
#include <scave/nodetyperegistry.h>
#include <scave/dataflowmanager.h>
#include <scave/indexfile.h>
#include <scave/vectorfilereader.h>
#include <scave/vectorfilewriter.h>
#include <scave/arraybuilder.h>
#include "testutil.h"

using namespace omnetpp;
using namespace std;

/**
 * Builds a dataflow network from a reader and writer node and executes it.
 * The reader reads all vectors from inputfile, and the writer writes all
 * vectors to the outputfile.
 */
void testReaderWriter(const char *inputfile, const char *outputfile)
{
    ResultFileManager resultfilemanager;
    DataflowManager manager;
    NodeTypeRegistry *registry = NodeTypeRegistry::getInstance();

    NodeType *readerType = registry->getNodeType("vectorfilereader");
    NodeType *writerType = registry->getNodeType("vectorfilewriter");
    StringMap attrs;
    attrs["filename"] = inputfile;
    VectorFileReaderNode *reader = (VectorFileReaderNode *)readerType->create(&manager, attrs);
    attrs["filename"] = outputfile;
    VectorFileWriterNode *writer = (VectorFileWriterNode *)writerType->create(&manager, attrs);

    resultfilemanager.loadFile(inputfile);
    IDList vectors = resultfilemanager.getAllVectors();
    size_t size = vectors.size();
    for (size_t i = 0; i < size; ++i) {
        ID id = vectors.get(i);
        const VectorResult& vector = resultfilemanager.getVector(id);
        Port *src = reader->addVector(vector);
        Port *dest = writer->addVector(vector);
        manager.connect(src, dest);
    }

    manager.execute();
}

/**
 * Builds a dataflow network from a reader and an array builder node and executes it.
 * The reader reads all vectors from inputfile, and the data collected by the
 * array builder are written to the outputfile.
 */
void testReaderBuilder(const char *inputfile, const char *outputfile)
{
    ResultFileManager resultfilemanager;
    DataflowManager manager;
    NodeTypeRegistry *registry = NodeTypeRegistry::getInstance();

    NodeType *readerType = registry->getNodeType("vectorfilereader");
    NodeType *builderType = registry->getNodeType("arraybuilder");
    StringMap attrs;
    attrs["filename"] = inputfile;
    VectorFileReaderNode *reader = (VectorFileReaderNode *)readerType->create(&manager, attrs);

    resultfilemanager.loadFile(inputfile);
    IDList vectors = resultfilemanager.getAllVectors();
    size_t size = vectors.size();
    attrs.clear();
    vector<ArrayBuilderNode *> builders;

    for (size_t i = 0; i < size; ++i) {
        ID id = vectors.get(i);
        const VectorResult& vector = resultfilemanager.getVector(id);
        ArrayBuilderNode *builder = (ArrayBuilderNode *)builderType->create(&manager, attrs);
        builders.push_back(builder);
        Port *src = reader->addVector(vector);
        Port *dest = builderType->getPort(builder, "in");
        manager.connect(src, dest);
    }

    manager.execute();

    ofstream out;
    out.open(outputfile);
    out.precision(15);
    for (size_t i = 0; i < size; ++i) {
        ID id = vectors.get(i);
        const VectorResult& vector = resultfilemanager.getVector(id);
        ArrayBuilderNode *builder = builders[i];
        XYArray *array = builder->getArray();
        int s = array->length();
        for (int i = 0; i < s; ++i) {
            BigDecimal xp = array->getPreciseX(i);
            double x = array->getX(i);
            double y = array->getY(i);
            if (xp.isNil())
                out << vector.vectorId << "\t" << x << "\t" << y << "\n";
            else
                out << vector.vectorId << "\t" << xp << "\t" << y << "\n";
        }
    }
    out.close();
}

void testReader(const char *readerNodeType, const char *inputFile, int *vectorIds, int count)
{
    if ((strcmp(readerNodeType, "indexedvectorfilereader") == 0 ||
         strcmp(readerNodeType, "indexedvectorfilereader2") == 0) &&
        !IndexFile::isIndexFileUpToDate(inputFile))
        throw exception("Index file is not up to date");

    NodeTypeRegistry *registry = NodeTypeRegistry::getInstance();
    NodeType *readerType = registry->getNodeType(readerNodeType);
    NodeType *builderType = registry->getNodeType("arraybuilder");

    DataflowManager manager;
    StringMap attrs;
    attrs["filename"] = inputFile;
    Node *reader = readerType->create(&manager, attrs);

    attrs.clear();
    char vectorIdStr[12];

    for (size_t i = 0; i < count; ++i) {
        sprintf(vectorIdStr, "%d", vectorIds[i]);
        Node *builder = builderType->create(&manager, attrs);
        Port *src = readerType->getPort(reader, vectorIdStr);
        Port *dest = builderType->getPort(builder, "in");
        manager.connect(src, dest);
    }

    {
        MeasureTime m;
        manager.execute();
    }
}

