//==========================================================================
//   SAXPARSER_NONE.CC -
//            part of OMNeT++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "nederror.h"
#include "saxparser.h"

NAMESPACE_BEGIN


SAXParser::SAXParser()
{
    saxhandler = NULL;
}

void SAXParser::setHandler(SAXHandler *sh)
{
    saxhandler = sh;
    sh->setParser(this);
}

bool SAXParser::parse(const char *filename)
{
    return doParse(filename, NULL);
}

bool SAXParser::parseContent(const char *content)
{
    return doParse(NULL, content);
}

bool SAXParser::doParse(const char *filename, const char *content)
{
    strcpy(errortext, "XML input is not supported: this copy of OMNeT++ was "
                      "compiled without an XML parser (libXML or Expat)");
    return false;
}

int SAXParser::getCurrentLineNumber()
{
    return 0;
}

NAMESPACE_END

