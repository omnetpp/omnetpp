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
#include "sourcedocument.h"
#include "nedelements.h"
#include "yydefs.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace nedxml {

#define MAGIC_PREFIX    "@expr@"  // note: must agree with ned2.lex


const char *NedParser::getBuiltInDeclarations()
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
        "    bool disabled @mutable = default(false);\n"
        "    double delay @mutable = default(0s) @unit(s); // propagation delay\n"
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
        "    bool disabled @mutable = default(false);\n"
        "    double delay @mutable = default(0s) @unit(s); // propagation delay\n"
        "    double datarate @mutable = default(0bps) @unit(bps); // bits per second; 0=infinite\n"
        "    double ber @mutable = default(0); // bit error rate (BER)\n"
        "    double per @mutable = default(0); // packet error rate (PER)\n"
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

NedParser::NedParser(ErrorStore *e)
{
    np.errors = e;
}

NedParser::~NedParser()
{
    delete np.source;
}

ASTNode *NedParser::parseNedFile(const char *osfname, const char *fname)
{
    if (!loadFile(osfname, fname))
        return nullptr;
    return parseNed();
}

ASTNode *NedParser::parseNedText(const char *nedtext, const char *fname)
{
    if (!loadText(nedtext, fname))
        return nullptr;
    return parseNed();
}

bool NedParser::loadFile(const char *osfname, const char *fname)
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

bool NedParser::loadText(const char *nedtext, const char *fname)
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
    if (!np.source->setData(nedtext)) {
        np.errors->addError("", "unable to allocate work memory");
        return false;
    }
    return true;
}

ASTNode *NedParser::parseNed()
{
    np.errors->clear();
    return ::doParseNed(&np);
}

bool NedParser::isValidNedExpression(const char *expr)
{
    std::string source = std::string(MAGIC_PREFIX) + "\n" + expr;
    delete parseNedText(source.c_str(), "expression");
    return !np.errors->containsError();
}

}  // namespace nedxml
}  // namespace omnetpp

