//=========================================================================
//  GETVECTORS.CC - part of
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

#include <stdlib.h>
#include "getvectors.h"
#include "filetokenizer.h"
#include "util.h"



void getVectors(const char *fname, OutVectorArray& array)
{
    FileTokenizer ftok(fname);

    while (ftok.readLine())
    {
        int numtokens = ftok.numTokens();
        char **vec = ftok.tokens();

        if (numtokens>0 && vec[0][0]=='v' && !strncmp(vec[0],"vector",6))
        {
            // vector line
            if (numtokens<4)
                throw new Exception("invalid vector file syntax: too few items on 'vector' line, line %d", ftok.lineNum());

            VecData vecdata;

            // vectorId
            char *e;
            vecdata.vectorId = (int) strtol(vec[1],&e,10);
            if (*e)
                throw new Exception("invalid vector file syntax: invalid vector id in vector definition, line %d", ftok.lineNum());

            // module name, vector name
            vecdata.moduleName = vec[2];
            vecdata.vectorName = vec[3];

            array.push_back(vecdata);
        }
    }

    // ignore "incomplete last line" error, because we might be reading
    // a vec file currently being written by a simulation
    if (!ftok.eof() && ftok.errorCode()!=FileTokenizer::INCOMPLETELINE)
        throw new Exception(ftok.errorMsg().c_str());
}

