//==========================================================================
//   SAXPARSER_NONE.CC -
//            part of OMNeT++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "nederror.h"
#include "saxparser.h"


SAXParser::SAXParser()
{
    saxhandler = NULL;
}

void SAXParser::setHandler(SAXHandler *sh)
{
    saxhandler = sh;
    sh->setParser(this);
}

int SAXParser::parse(FILE *f)
{
    NEDError(NULL, "XML input is not supported: this copy of NEDXML was "
                   "compiled without an XML parser (libXML or Expat)");
    return 0;
}

int SAXParser::getCurrentLineNumber()
{
    return 0;
}


