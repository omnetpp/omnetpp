//=========================================================================
//  VECTORFILEREADERTEST.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <iostream>
#include <fstream>
#include <set>
#include <vector>

#include "exception.h"
#include "resultfilemanager.h"
#include "idlist.h"
#include "node.h"
#include "nodetype.h"
#include "nodetyperegistry.h"
#include "dataflowmanager.h"
#include "vectorfilereader.h"
#include "vectorfilewriter.h"
#include "arraybuilder.h"
#include "testutil.h"

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
    NodeTypeRegistry *registry = NodeTypeRegistry::instance();

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
    for (size_t i = 0; i < size; ++i)
    {
        ID id = vectors.get(i);
        const VectorResult &vector = resultfilemanager.getVector(id);
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
    NodeTypeRegistry *registry = NodeTypeRegistry::instance();

    NodeType *readerType = registry->getNodeType("vectorfilereader");
    NodeType *builderType = registry->getNodeType("arraybuilder");
    StringMap attrs;
    attrs["filename"] = inputfile;
    VectorFileReaderNode *reader = (VectorFileReaderNode *)readerType->create(&manager, attrs);

    resultfilemanager.loadFile(inputfile);
    IDList vectors = resultfilemanager.getAllVectors();
    size_t size = vectors.size();
    attrs.clear();
    vector<ArrayBuilderNode*> builders;

    for (size_t i = 0; i < size; ++i)
    {
        ID id = vectors.get(i);
        const VectorResult &vector = resultfilemanager.getVector(id);
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
    for (size_t i = 0; i < size; ++i)
    {
        ID id = vectors.get(i);
        const VectorResult &vector = resultfilemanager.getVector(id);
        ArrayBuilderNode *builder = builders[i];
        XYArray *array = builder->getArray();
        int s = array->length();
        for (int i = 0; i < s; ++i)
        {
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

void testReader(const char *inputfile, int *vectorIds, int count)
{
    NodeTypeRegistry *registry = NodeTypeRegistry::instance();
    NodeType *readerType = registry->getNodeType("vectorfilereader");
    NodeType *builderType = registry->getNodeType("arraybuilder");
    
    ResultFileManager resultfilemanager;
    resultfilemanager.loadFile(inputfile);
    set<int> vectorIdSet(vectorIds, vectorIds+count);
    IDList vectors = resultfilemanager.getAllVectors();
    IDList vectorsToBeRead; 
    for (int i = 0; i < vectors.size(); ++i)
    {
    	ID id = vectors.get(i);
    	const VectorResult &vector = resultfilemanager.getVector(id);
    	if (vectorIdSet.find(vector.vectorId) != vectorIdSet.end())
    		vectorsToBeRead.add(id);
    }

    DataflowManager manager;
    StringMap attrs;
    attrs["filename"] = inputfile;
    VectorFileReaderNode *reader = (VectorFileReaderNode *)readerType->create(&manager, attrs);

    attrs.clear();
    char vectorIdStr[12];
    
    for (int i = 0; i < vectorsToBeRead.size(); ++i)
    {
    	const VectorResult &vector = resultfilemanager.getVector(vectorsToBeRead.get(i));
        Node *builder = builderType->create(&manager, attrs);
        Port *src = reader->addVector(vector);
        Port *dest = builderType->getPort(builder, "in");
        manager.connect(src, dest);
    }
    
    {
    	MeasureTime m;
    	manager.execute();
    }
}

