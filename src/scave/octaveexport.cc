//=========================================================================
//  OCTAVEEXPORT.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include "octaveexport.h"

#ifdef CHECK
#undef CHECK
#endif
#define CHECK(fprintf)    if (fprintf<0) throw new Exception("Cannot write file `%s'", fileName.c_str())


OctaveExport::OctaveExport(const char *fileName)
{
    f = NULL;
    this->prec = DEFAULT_PRECISION;
    this->fileName = fileName;
}

OctaveExport::~OctaveExport()
{
    close();
}

void OctaveExport::openFileIfNeeded()
{
    if (!f)
    {
        f = fopen(fileName.c_str(), "w");
        if (!f)
            throw new Exception("cannot open `%s' for write", fileName.c_str());

        // print file header
        CHECK(fprintf(f,"# Created by OMNeT++/OMNEST scavetool\n"));
    }
}

void OctaveExport::close()
{
    if (f)
    {
        fclose(f);
        f = NULL;
    }
}

void OctaveExport::writeHeader(const char *name, const char *type, int rows, int columns)
{
    CHECK(fprintf(f,"# name: %s\n"
                    "# type: %s\n"
                    "# rows: %d\n"
                    "# columns: %d\n",
                    name, type, rows, columns));
}

void OctaveExport::saveVector(const char *name, const XYArray *vec, int startIndex, int endIndex)
{
    // write header
    openFileIfNeeded();
    if (endIndex==-1)
        endIndex = vec->length();
    writeHeader(name, "matrix", startIndex - endIndex, 2);

    // write data
    for (int i=startIndex; i<endIndex; i++)
        CHECK(fprintf(f," %.*g %.*g\n", prec, vec->getX(i), prec, vec->getY(i)));
}

void OctaveExport::saveVectorX(const char *name, const XYArray *vec, int startIndex, int endIndex)
{
    // write header
    openFileIfNeeded();
    if (endIndex==-1)
        endIndex = vec->length();
    writeHeader(name, "matrix", startIndex - endIndex, 1);

    // write data
    for (int i=startIndex; i<endIndex; i++)
        CHECK(fprintf(f," %.*g\n", prec, vec->getX(i)));
}

void OctaveExport::saveVectorY(const char *name, const XYArray *vec, int startIndex, int endIndex)
{
    // write header
    openFileIfNeeded();
    if (endIndex==-1)
        endIndex = vec->length();
    writeHeader(name, "matrix", startIndex - endIndex, 1);

    // write data
    for (int i=startIndex; i<endIndex; i++)
        CHECK(fprintf(f," %.*g\n", prec, vec->getY(i)));
}

