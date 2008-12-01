//=========================================================================
//  OCTAVEEXPORT.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Tamas Borbely
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <locale.h>
#include "opp_ctype.h"
#include "octaveexport.h"

#ifdef CHECK
#undef CHECK
#endif

USING_NAMESPACE
#define CHECK(fprintf)    if (fprintf<0) throw opp_runtime_error("Cannot write file `%s'", fileName.c_str())


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
        // note: we have to open the file in binary mode, because on Windows,
        // cygwin-based Octave chokes on CR-LF...
        f = fopen(fileName.c_str(), "wb");
        if (!f)
            throw opp_runtime_error("cannot open `%s' for write", fileName.c_str());

        setlocale(LC_NUMERIC, "C");
        // print file header
        CHECK(fprintf(f,"# Created by OMNeT++/OMNEST scavetool\n"));
    }
}

void OctaveExport::close()
{
    if (f)
    {
        // close the file
        fclose(f);
        f = NULL;
    }
}

std::string OctaveExport::makeUniqueName(const char *nameHint)
{
    // first, process the name. Only alphanumeric chars are allowed, others
    // are replaced with underscore
    std::string name = nameHint;
    for (int i=0; i<(int)name.length(); i++)
        if (!opp_isalnum(name[i]))
            name[i] = '_';

    // check if it's already unique
    std::set<std::string>::const_iterator it = savedVars.find(name);
    if (it == savedVars.end())
        return name;

    // try appending "_1", "_2", etc until it becomes unique
    for (int i=1; i>0; i++)
    {
        char suffix[32];
        sprintf(suffix,"_%d", i);
        std::string newName = name + suffix;

        std::set<std::string>::const_iterator it = savedVars.find(newName);
        if (it == savedVars.end())
            return newName;
    }
    throw opp_runtime_error("banged head against the sky");
}

void OctaveExport::writeMatrixHeader(const char *name, int rows, int columns)
{
    savedVars.insert(name);

    CHECK(fprintf(f,"# name: %s\n"
                    "# type: matrix\n"
                    "# rows: %d\n"
                    "# columns: %d\n",
                    name, rows, columns));
}

void OctaveExport::writeString(const char *name, const char *value)
{
    CHECK(fprintf(f,"# name: %s\n"
                    "# type: string\n"
                    "# elements: 1\n"
                    "# length: %d\n",
                    name, strlen(value)));
    CHECK(fprintf(f,"%s\n", value));
}

void OctaveExport::writeDescription(const char *name, const char *description)
{
    writeString((std::string(name)+"_descr").c_str(), description);
}

void OctaveExport::saveVector(const char *name, const char *description,
                              const XYArray *vec, int startIndex, int endIndex)
{
    // write header
    openFileIfNeeded();
    if (description)
        writeDescription(name, description);
    if (endIndex==-1)
        endIndex = vec->length();
    writeMatrixHeader(name, endIndex-startIndex, 2);

    // write data
    for (int i=startIndex; i<endIndex; i++)
        CHECK(fprintf(f," %.*g %.*g\n", prec, vec->getX(i), prec, vec->getY(i)));
}

void OctaveExport::saveVectorX(const char *name, const char *description,
                               const XYArray *vec, int startIndex, int endIndex)
{
    // write header
    openFileIfNeeded();
    if (description)
        writeDescription(name, description);
    if (endIndex==-1)
        endIndex = vec->length();
    writeMatrixHeader(name, endIndex-startIndex, 1);

    // write data
    for (int i=startIndex; i<endIndex; i++)
        CHECK(fprintf(f," %.*g\n", prec, vec->getX(i)));
}

void OctaveExport::saveVectorY(const char *name, const char *description,
                               const XYArray *vec, int startIndex, int endIndex)
{
    // write header
    openFileIfNeeded();
    if (description)
        writeDescription(name, description);
    if (endIndex==-1)
        endIndex = vec->length();
    writeMatrixHeader(name, endIndex-startIndex, 1);

    // write data
    for (int i=startIndex; i<endIndex; i++)
        CHECK(fprintf(f," %.*g\n", prec, vec->getY(i)));
}

