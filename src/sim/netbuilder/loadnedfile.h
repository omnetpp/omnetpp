//==========================================================================
//  CNETWORKBUILDER.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `terms' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __LOADNEDFILE_H
#define __LOADNEDFILE_H


/**
 * How does it work?
 *
 * - before network setup, we read some NED files, and register
 *   the modules types and network types in them (by creating cDynamicModuleType
 *   objects and adding them to the 'networktypes' list).
 *
 * - then we can instantiate _any_ network: it may contain module types that were
 *   read in dynamically, it'll work without problem.
 */
void loadNedFile(const char *nedfname, bool isXML);


#endif


