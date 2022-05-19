//==========================================================================
//  MSGPARSER.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <sstream>
#include <string>
#include "common/opp_ctype.h"
#include "msgparser.h"
#include "errorstore.h"
#include "sourcedocument.h"
#include "msgelements.h"
#include "yydefs.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace nedxml {

MsgParser::MsgParser(ErrorStore *e)
{
    np.errors = e;
}

MsgParser::~MsgParser()
{
    delete np.source;
}

ASTNode *MsgParser::parseMsgFile(const char *osfname, const char *fname)
{
    if (!loadFile(osfname, fname))
        return nullptr;
    return parseMsg();
}

ASTNode *MsgParser::parseMsgText(const char *text, const char *fname)
{
    if (!loadText(text, fname))
        return nullptr;
    return parseMsg();
}

bool MsgParser::loadFile(const char *osfname, const char *fname)
{
    if (!fname)
        fname = osfname;

    // init class members
    if (np.source)
        delete np.source;
    np.source = new SourceDocument();
    np.filename = fname;
    np.errors->clear();

    // resolve "~" in file name
    std::string osfname2 = (osfname[0] == '~') ? std::string(getenv("HOME")) + (osfname+1) : osfname;

    // load whole file into memory
    if (!np.source->readFile(osfname2.c_str())) {
        np.errors->addError("", "cannot read %s", fname);
        return false;
    }
    return true;
}

bool MsgParser::loadText(const char *text, const char *fname)
{
    if (!fname)
        fname = "buffer";

    // init vars
    if (np.source)
        delete np.source;
    np.source = new SourceDocument();
    np.filename = fname;
    np.errors->clear();

    // prepare np.source object
    if (!np.source->setData(text)) {
        np.errors->addError("", "unable to allocate work memory");
        return false;
    }
    return true;
}

ASTNode *MsgParser::parseMsg()
{
    np.errors->clear();
    return ::doParseMsg(&np);
}

}  // namespace nedxml
}  // namespace omnetpp

