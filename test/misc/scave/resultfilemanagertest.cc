// =========================================================================
//  RESULTFILEMANAGERTEST.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
// =========================================================================

/*--------------------------------------------------------------*
   Copyright (C) 1992-2015 Andras Varga

   This file is distributed WITHOUT ANY WARRANTY. See the file
   `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <iostream>
#include <fstream>
#include <common/exception.h>
#include <scave/resultfilemanager.h>
#include <scave/idlist.h>

using namespace omnetpp;

static void testUnload(const char *inputfile)
{
    ResultFileManager manager;
    ResultFile *file = manager.loadFile(inputfile);
    if (file == nullptr)
        throw std::exception("Cannot load file.\n");
    manager.unloadFile(file);
    IDList vectors = manager.getAllVectors();
    if (vectors.size() > 0)
        throw std::exception("Cannot unload file\n");
}

void testResultFileManager(const char *inputfile)
{
    testUnload(inputfile);
}

