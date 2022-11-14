//==========================================================================
//  CMDENVIR.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "omnetpp/cconfigoption.h"
#include "omnetpp/cproperties.h"
#include "omnetpp/cproperty.h"
#include "cmdenvir.h"

using namespace omnetpp::common;
using namespace omnetpp::internal;

namespace omnetpp {
namespace cmdenv {

Register_GlobalConfigOptionU(CFGID_CMDENV_EXTRA_STACK, "cmdenv-extra-stack", "B", "8KiB", "Specifies the extra amount of stack that is reserved for each `activity()` simple module when the simulation is run under Cmdenv.")
Register_GlobalConfigOption(CFGID_CMDENV_INTERACTIVE, "cmdenv-interactive", CFG_BOOL, "false", "Defines what Cmdenv should do when the model contains unassigned parameters. In interactive mode, it asks the user. In non-interactive mode (which is more suitable for batch execution), Cmdenv stops with an error.")
Register_GlobalConfigOption(CFGID_CMDENV_LOG_PREFIX, "cmdenv-log-prefix", CFG_STRING, "[%l]\t", "Specifies the format string that determines the prefix of each log line. The format string may contain format directives in the syntax `%x` (a `%` followed by a single format character).  For example `%l` stands for log level, and `%J` for source component. See the manual for the list of available format characters.");
Register_GlobalConfigOption(CFGID_CMDENV_FAKE_GUI, "cmdenv-fake-gui", CFG_BOOL, "false", "Causes Cmdenv to lie to simulations that is a GUI (isGui()=true), and to periodically invoke refreshDisplay() during simulation execution.");
Register_PerObjectConfigOption(CFGID_CMDENV_LOGLEVEL, "cmdenv-log-level", KIND_MODULE, CFG_STRING, "TRACE", "Specifies the per-component level of detail recorded by log statements, output below the specified level is omitted. Available values are (case insensitive): `off`, `fatal`, `error`, `warn`, `info`, `detail`, `debug` or `trace`. Note that the level of detail is also controlled by the globally specified runtime log level and the `COMPILETIME_LOGLEVEL` macro that is used to completely remove log statements from the executable.")

extern cConfigOption *CFGID_CMDENV_EXPRESS_MODE;
extern cConfigOption *CFGID_CMDENV_AUTOFLUSH;
extern cConfigOption *CFGID_CMDENV_EVENT_BANNERS;

CmdEnvir::CmdEnvir(std::ostream& out, bool& sigintReceived) : out(out), sigintReceived(sigintReceived)
{
}

void CmdEnvir::configure(cConfiguration *cfg)
{
    EnvirBase::configure(cfg);

    setExpressMode(cfg->getAsBool(CFGID_CMDENV_EXPRESS_MODE));
    setAutoflush(cfg->getAsBool(CFGID_CMDENV_AUTOFLUSH));
    setInteractive(cfg->getAsBool(CFGID_CMDENV_INTERACTIVE));
    setPrintEventBanners(cfg->getAsBool(CFGID_CMDENV_EVENT_BANNERS));
    setLogFormat(cfg->getAsString(CFGID_CMDENV_LOG_PREFIX).c_str());
    setExtraStackForEnvir((size_t)cfg->getAsDouble(CFGID_CMDENV_EXTRA_STACK));

    bool useFakeGUI = cfg->getAsBool(CFGID_CMDENV_FAKE_GUI);
    FakeGUI *fakeGUI = useFakeGUI ? new FakeGUI() : nullptr;
    setFakeGUI(fakeGUI);
    setIsGUI(useFakeGUI);

    if (fakeGUI)
        fakeGUI->configure(getSimulation(), cfg);
}

void CmdEnvir::setFakeGUI(FakeGUI *fakeGUI)
{
    delete this->fakeGUI;
    this->fakeGUI = fakeGUI;
    if (fakeGUI)
        out << "\n*** WARNING: FAKEGUI IS AN EXPERIMENTAL FEATURE -- DO NOT RELY ON FINGERPRINTS GENERATED UNDER FAKEGUI UNTIL CODE IS FINALIZED!\n" << endl;
}

void CmdEnvir::componentInitBegin(cComponent *component, int stage)
{
    EnvirBase::componentInitBegin(component, stage);

    // TODO: make this an EV_INFO in the component?
    if (!expressMode && printEventBanners && component->getLogLevel() != LOGLEVEL_OFF)
        out << "Initializing " << (component->isModule() ? "module" : "channel") << " " << component->getFullPath() << ", stage " << stage << endl;
}

void CmdEnvir::configureComponent(cComponent *component)
{
    EnvirBase::configureComponent(component);

    LogLevel level = cLog::resolveLogLevel(cfg->getAsString(component->getFullPath().c_str(), CFGID_CMDENV_LOGLEVEL).c_str());
    component->setLogLevel(level);
}

void CmdEnvir::askParameter(cPar *par, bool unassigned)
{
    bool success = false;
    while (!success) {
        cProperties *props = par->getProperties();
        cProperty *prop = props->get("prompt");
        std::string prompt = prop ? prop->getValue(cProperty::DEFAULTKEY) : "";
        std::string reply;

        // ask the user. note: input() will signal "cancel" by throwing an exception
        if (!prompt.empty())
            reply = input(prompt.c_str(), par->str().c_str());
        else
            // DO NOT change the "Enter parameter" string. The IDE launcher plugin matches
            // against this string for detecting user input
            reply = this->input((std::string("Enter parameter '")+par->getFullPath()+"' ("+(unassigned ? "unassigned" : "ask")+"):").c_str(), par->str().c_str());

        try {
            par->parse(reply.c_str());
            success = true;
        }
        catch (std::exception& e) {
            out << "<!> " << e.what() << " -- please try again" << endl;
        }
    }
}

void CmdEnvir::alert(const char *msg)
{
    out << "\n<!> " << msg << endl << endl;
}

void CmdEnvir::log(cLogEntry *entry)
{
    EnvirBase::log(entry);

    if (!logFormatter.isBlank())
        out << logFormatter.formatPrefix(entry);

    out.write(entry->text, entry->textLength);
    if (autoflush)
        out.flush();
}

std::string CmdEnvir::input(const char *prompt, const char *defaultReply)
{
    if (!interactive)
        throw cRuntimeError("The simulation attempted to prompt for user input, set cmdenv-interactive=true to allow it: \"%s\"", prompt);

    out << prompt;
    if (!opp_isempty(defaultReply))
        out << "(default: " << defaultReply << ") ";
    out.flush();

    std::string buffer;
    std::getline(std::cin, buffer);
    if (buffer == "\x1b")  // ESC?
        throw cRuntimeError(E_CANCEL);
    return buffer;
}

bool CmdEnvir::askYesNo(const char *question)
{
    if (!interactive)
        throw cRuntimeError("Simulation needs user input in non-interactive mode (prompt text: \"%s (y/n)\")", question);

    for (;;) {
        out << question <<" (y/n) ";
        out.flush();
        std::string buffer;
        std::getline(std::cin, buffer);
        if (buffer == "\x1b")  // ESC?
            throw cRuntimeError(E_CANCEL);
        if (buffer == "y" || buffer == "Y")
            return true;
        else if (buffer == "n" || buffer == "N")
            return false;
        else
            out << "Please type 'y' or 'n'!" << endl;
    }
}

void CmdEnvir::undisposedObject(cObject *obj)
{
    // overridden to use the "right" stream
    if (printUndisposed)
        out << "undisposed object: (" << obj->getClassName() << ") " << obj->getFullPath() << " -- check module destructor" << endl;
}

bool CmdEnvir::idle()
{
    return sigintReceived;
}

double CmdEnvir::getAnimationTime() const
{
    return fakeGUI ? fakeGUI->getAnimationTime() : 0;
}

double CmdEnvir::getAnimationSpeed() const
{
    return fakeGUI ? fakeGUI->getAnimationSpeed() : 0;
}

double CmdEnvir::getRemainingAnimationHoldTime() const
{
    return fakeGUI ? fakeGUI->getRemainingAnimationHoldTime() : 0;
}

void CmdEnvir::getImageSize(const char *imageName, double& outWidth, double& outHeight)
{
    if (fakeGUI)
        fakeGUI->getImageSize(imageName, outWidth, outHeight);
    else
        outWidth = outHeight = 32;
}

void CmdEnvir::getTextExtent(const cFigure::Font& font, const char *text, double& outWidth, double& outHeight, double& outAscent)
{
    if (!*text)
        outWidth = outHeight = outAscent = 0;
    else if (fakeGUI)
        fakeGUI->getTextExtent(font, text, outWidth, outHeight, outAscent);
    else {
        outWidth = 10 * strlen(text);
        outHeight = 12;
        outAscent = 8;
    }
}

void CmdEnvir::appendToImagePath(const char *directory)
{
    if (fakeGUI)
        fakeGUI->appendToImagePath(directory);
}

void CmdEnvir::loadImage(const char *fileName, const char *imageName)
{
    if (fakeGUI)
        fakeGUI->loadImage(fileName, imageName);
}

cFigure::Rectangle CmdEnvir::getSubmoduleBounds(const cModule *submodule)
{
    return fakeGUI ? fakeGUI->getSubmoduleBounds(submodule) : cFigure::Rectangle(NAN, NAN, NAN, NAN);
}

std::vector<cFigure::Point> CmdEnvir::getConnectionLine(const cGate *sourceGate)
{
    return fakeGUI ? fakeGUI->getConnectionLine(sourceGate) : std::vector<cFigure::Point>();
}

double CmdEnvir::getZoomLevel(const cModule *module)
{
    return fakeGUI ? fakeGUI->getZoomLevel(module) : NAN;
}

bool CmdEnvir::ensureDebugger(cRuntimeError *error)
{
    AppBase *app = AppBase::getActiveApp();  //TODO should not rely on active App
    return app ? app->ensureDebugger(error) : false;
}

}  // namespace cmdenv
}  // namespace omnetpp

