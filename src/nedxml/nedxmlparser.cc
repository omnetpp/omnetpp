//==========================================================================
//  NEDXMLPARSER.CC - part of
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


#include "saxparser.h"
#include "nedsaxhandler.h"
#include "nederror.h"

NEDElement *parseXML(const char *filename)
{
    NEDSAXHandler sh(filename);
    SAXParser parser;

    parser.setHandler(&sh);
    bool ok = parser.parse(filename);
    if (!ok)
    {
        NEDError(NULL, "Error reading `%s': %s", filename, parser.getErrorMessage());
        return 0;
    }
    return sh.getTree();
}
