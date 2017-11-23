//==========================================================================
//  NEDPARSER.CC - part of
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
#include "nedparser.h"

#include "errorstore.h"
#include "nedelements.h"
#include "nedyydefs.h"
#include "sourcedocument.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace nedxml {

#define MAGIC_PREFIX    "@expr@"  // note: must agree with ned2.lex

NEDParser *np;

//--------

const char *NEDParser::getBuiltInDeclarations()
{
    return
        "package ned;\n"
        "@namespace(\"omnetpp\");\n"
        "\n"
        "channel IdealChannel\n"
        "{\n"
        "    @class(cIdealChannel);\n"
        "}\n"
        "\n"
        "channel DelayChannel\n"
        "{\n"
        "    @class(cDelayChannel);\n"
        "    @signal[messageSent](type=omnetpp::cMessage);\n"
        "    @signal[messageDiscarded](type=omnetpp::cMessage);\n"
        "    @statistic[messages](source=\"constant1(messageSent)\";record=count?;interpolationmode=none);\n"
        "    @statistic[messagesDiscarded](source=\"constant1(messageDiscarded)\";record=count?;interpolationmode=none);\n"
        "    bool disabled = default(false);\n"
        "    double delay = default(0s) @unit(s); // propagation delay\n"
        "}\n"
        "\n"
        "channel DatarateChannel\n"
        "{\n"
        "    @class(cDatarateChannel);\n"
        "    @signal[channelBusy](type=long);\n"
        "    @signal[messageSent](type=omnetpp::cMessage);\n"
        "    @signal[messageDiscarded](type=omnetpp::cMessage);\n"
        "    @statistic[busy](source=channelBusy;record=vector?;interpolationmode=sample-hold);\n"
        "    @statistic[utilization](source=\"timeavg(channelBusy)\";record=last?);\n"
        "    @statistic[packets](source=\"constant1(messageSent)\";record=count?;interpolationmode=none);\n"
        "    @statistic[packetBytes](source=\"packetBytes(messageSent)\";record=sum?;unit=B;interpolationmode=none);\n"
        "    @statistic[packetsDiscarded](source=\"constant1(messageDiscarded)\";record=count?;interpolationmode=none);\n"
        "    @statistic[throughput](source=\"sumPerDuration(packetBits(messageSent))\";record=last?;unit=bps);\n"
        "    bool disabled = default(false);\n"
        "    double delay = default(0s) @unit(s); // propagation delay\n"
        "    double datarate = default(0bps) @unit(bps); // bits per second; 0=infinite\n"
        "    double ber = default(0); // bit error rate (BER)\n"
        "    double per = default(0); // packet error rate (PER)\n"
        "}\n"
        "\n"
        "moduleinterface IBidirectionalChannel\n"
        "{\n"
        "    gates:\n"
        "        inout a;\n"
        "        inout b;\n"
        "}\n"
        "\n"
        "moduleinterface IUnidirectionalChannel\n"
        "{\n"
        "    gates:\n"
        "        input i;\n"
        "        output o;\n"
        "}\n"
    ;
}

//--------

NEDParser::NEDParser(ErrorStore *e)
{
    nedsource = nullptr;
    filename = nullptr;
    parseexpr = true;
    storesrc = false;
    msgNewSyntax = true;
    errors = e;
}

NEDParser::~NEDParser()
{
    delete nedsource;
}

ASTNode *NEDParser::parseNEDFile(const char *osfname, const char *fname)
{
    if (!loadFile(osfname, fname))
        return nullptr;
    return parseNED();
}

ASTNode *NEDParser::parseNEDText(const char *nedtext, const char *fname)
{
    if (!loadText(nedtext, fname))
        return nullptr;
    return parseNED();
}

ASTNode *NEDParser::parseNEDExpression(const char *nedexpression)
{
    parseexpr = true;
    std::string source = std::string(MAGIC_PREFIX) + "\n" + nedexpression;
    ASTNode *tree = parseNEDText(source.c_str(), "buffer");
    return tree ? tree->getFirstChild() : nullptr;  // unwrap from NedFileElement
}

ASTNode *NEDParser::parseMSGFile(const char *osfname, const char *fname)
{
    if (!loadFile(osfname, fname))
        return nullptr;
    return parseMSG();
}

ASTNode *NEDParser::parseMSGText(const char *nedtext, const char *fname)
{
    if (!loadText(nedtext, fname))
        return nullptr;
    return parseMSG();
}

bool NEDParser::loadFile(const char *osfname, const char *fname)
{
    if (!fname)
        fname = osfname;

    // init class members
    if (nedsource)
        delete nedsource;
    nedsource = new SourceDocument();
    filename = fname;
    errors->clear();

    // resolve "~" in file name
    char osfname2[1000];
    if (osfname[0] == '~') {
        sprintf(osfname2, "%s%s", getenv("HOME"), osfname+1);
    }
    else {
        strcpy(osfname2, osfname);
    }

    // load whole file into memory
    if (!nedsource->readFile(osfname2)) {
        errors->addError("", "cannot read %s", fname);
        return false;
    }
    return true;
}

bool NEDParser::loadText(const char *nedtext, const char *fname)
{
    if (!fname)
        fname = "buffer";

    // init vars
    if (nedsource)
        delete nedsource;
    nedsource = new SourceDocument();
    filename = fname;
    errors->clear();

    // prepare nedsource object
    if (!nedsource->setData(nedtext)) {
        errors->addError("", "unable to allocate work memory");
        return false;
    }
    return true;
}

ASTNode *NEDParser::parseNED()
{
    errors->clear();
    return ::doParseNED2(this, nedsource->getFullText());
}

ASTNode *NEDParser::parseMSG()
{
    errors->clear();
    msgLexerSetRecognizeImportKeyword(msgNewSyntax);
    msgLexerSetRecognizeObsoleteKeywords(!msgNewSyntax);
    return ::doParseMSG2(this, nedsource->getFullText());
}

void NEDParser::error(const char *msg, int line)
{
    std::stringstream os;
    os << filename << ":" << line;
    errors->addError(os.str().c_str(), "%s", msg);
}

}  // namespace nedxml
}  // namespace omnetpp

