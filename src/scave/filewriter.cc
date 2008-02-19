//=========================================================================
//  FILEWRITER.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "channel.h"
#include "filewriter.h"

#ifdef CHECK
#undef CHECK
#endif

USING_NAMESPACE
#define CHECK(fprintf)    if (fprintf<0) throw opp_runtime_error("Cannot write output vector file `%s'", fileName.c_str())


FileWriterNode::FileWriterNode(const char *filename, const char *banner)
{
    f = NULL;
    this->fileName = filename;
    this->banner = (banner ? banner : "");
    this->prec = DEFAULT_PRECISION;
}

FileWriterNode::~FileWriterNode()
{
}

bool FileWriterNode::isReady() const
{
    return in()->length()>0;
}

void FileWriterNode::process()
{
    // open file if needed
    if (!f)
    {
        f = fopen(fileName.c_str(), "w");
        if (!f)
            throw opp_runtime_error("cannot open `%s' for write", fileName.c_str());

        // print file header
        CHECK(fprintf(f,"%s\n\n", banner.c_str()));
    }

    int n = in()->length();
    for (int i=0; i<n; i++)
    {
        Datum a;
        in()->read(&a,1);
        CHECK(fprintf(f,"%.*g\t%.*g\n", prec, a.x, prec, a.y));
    }

    if (in()->closing())
        fclose(f);
}

bool FileWriterNode::finished() const
{
    return in()->eof();
}

//--------

const char *FileWriterNodeType::description() const
{
    return "Writes the output into a two-column text file.";
}

void FileWriterNodeType::getAttributes(StringMap& attrs) const
{
    attrs["filename"] = "name of the output file";
}

Node *FileWriterNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    const char *fileName = attrs["filename"].c_str();

    Node *node = new FileWriterNode(fileName);
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}

