//==========================================================================
//  NEDXMLPARSER.CC -
//            part of OMNeT++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2002 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `terms' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include "saxparser.h"
#include "nedsaxhandler.h"

NEDElement *parseXML(const char *filename)
{
    // open file
    FILE *f = fopen(filename,"r");
    if (!f)
    {
        fprintf(stderr, "cannot open %s", filename); // FIXME error handling
        return 0;
    }

    NEDSAXHandler sh(filename);
    SAXParser parser;

    parser.setHandler(&sh);
    parser.parse(f);

    return sh.getTree();
}
