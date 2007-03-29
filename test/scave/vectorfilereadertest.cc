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

using namespace std;

/**
 * Builds a dataflow network from a reader and writer node and executes it.
 * The reader reads all vectors from inputfile, and the writer writes all
 * vectors to the outputfile.
 */
static void testWithVectorFileWriter(const char *inputfile, const char *outputfile)
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
static void testWithArrayBuilder(const char *inputfile, const char *outputfile)
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

static void usage(char *message)
{
    if (message)
        cerr << "Error: " << message << "\n\n";

    cerr << "Usage:\n";
    cerr << "   vectorfilereadertest <input-file-name> <output-file-name> <output-node-type>\n";
    cerr << "where <output-node-type> is 'a' (arraybuilder) or 'v' (vectorfilewriter)\n\n";
}

int main(int argc, char **argv)
{
    try {
        if (argc < 4) {
            usage("Not enough arguments specified");
            return -1;
        }
        else {
            if (strcmp(argv[3], "v") == 0)
                testWithVectorFileWriter(argv[1], argv[2]);
            else if (strcmp(argv[3], "a") == 0)
                testWithArrayBuilder(argv[1], argv[2]);
            else {
                usage("Unknown <output-node-type>");
                return -1;
            }

            cout << "PASS\n";
            return 0;
        }
    }
    catch (std::exception& e) {
        cout << "FAIL: " << e.what();
        return -2;
    }
}
