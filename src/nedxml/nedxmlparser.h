//==========================================================================
//  NEDXMLPARSER.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#ifndef __NEDXMLPARSER_H
#define __NEDXMLPARSER_H

#include "nedelement.h"

/**
 * Simple front-end to NED XML parsing: invokes SAXParser with NEDSAXHandler.
 *
 * @ingroup XMLParser
 */
NEDElement *parseXML(const char *filename);

#endif

