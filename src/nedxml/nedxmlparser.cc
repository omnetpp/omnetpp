//==========================================================================
//  NEDXMLPARSER.CC -
//            part of OMNeT++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include "saxparser.h"
#include "nedsaxhandler.h"
#include "nederror.h"

NEDElement *parseXML(const char *filename)
{
    // open file
    FILE *f = fopen(filename,"r");
    if (!f)
    {
        NEDError(NULL, "Cannot open file '%s'", filename);
        return 0;
    }

    NEDSAXHandler sh(filename);
    SAXParser parser;

    parser.setHandler(&sh);
    bool ok = parser.parse(f);
    if (!ok)
    {
        NEDError(NULL, "Error parsing `%s': %s", filename, parse.getErrorMessage());
        return 0;
    }
    return sh.getTree();
}
