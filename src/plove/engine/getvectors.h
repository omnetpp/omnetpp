//=========================================================================
//  GETVECTORS.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _GETVECTORS_H_
#define _GETVECTORS_H_

#include <string>
#include <vector>


/**
 * Holds an output vector definition.
 *
 * @see OutVectorArray, getVectors()
 */
struct VecData
{
    int vectorId;
    std::string moduleName;
    std::string vectorName;
};


/**
 * Holds a set of output vector definitions.
 *
 * @see getVectors()
 */
typedef std::vector<VecData> OutVectorArray;


/**
 * Extracts vector definitions from an output vector file.
 *
 * @see OutVectorArray, VecData
 */
void getVectors(const char *fname, OutVectorArray& array);

#endif


