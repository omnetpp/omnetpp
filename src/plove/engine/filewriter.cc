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

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include "channel.h"
#include "filewriter.h"


FileWriterNode::FileWriterNode(const char *filename)
{
    fname = filename;
    f = NULL;
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
        f = fopen(fname.c_str(), "w");
        if (!f)
            throw new Exception("cannot open `%s' for write", fname.c_str());
    }

    int n = in()->length();
    for (int i=0; i<n; i++)
    {
        Datum a;
        in()->read(&a,1);
        fprintf(f,"%lg\t%lg\n", a.x, a.y); // FIXME max precision needed!!!
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

Node *FileWriterNodeType::create(DataflowManager *, StringMap& attrs) const
{
    checkAttrNames(attrs);

    const char *fname = attrs["filename"].c_str();

    Node *node = new FileWriterNode(fname);
    node->setNodeType(this);
    return node;
}

