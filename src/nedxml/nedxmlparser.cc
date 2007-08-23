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
#include "nedxmlparser.h"

NEDElement *parseXML(const char *filename, NEDErrorStore *errors)
{
    NEDSAXHandler sh(filename, errors);
    SAXParser parser;

    parser.setHandler(&sh);
    bool ok = parser.parse(filename);
    if (!ok)
    {
        errors->addError("", "error reading `%s': %s", filename, parser.getErrorMessage());
        return 0;
    }
    return sh.getTree();
}
