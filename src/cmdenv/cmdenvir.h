//==========================================================================
//  CMDENVIR.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_CMDENV_CMDENVIR_H
#define __OMNETPP_CMDENV_CMDENVIR_H

#include "cmddefs.h"
#include "fakegui.h"
#include "envir/envirbase.h"

namespace omnetpp {
namespace cmdenv {

using namespace omnetpp::envir;

/**
 * Envir class for the command line user interface.
 */
class CMDENV_API CmdEnvir : public EnvirBase
{
   protected:
    std::ostream& out;
    bool& sigintReceived;

    FakeGUI *fakeGUI = nullptr; // owned  --TODO not owned
    bool printEventBanners = false;
    bool autoflush = false;
    bool interactive = false;

   public:
    CmdEnvir(std::ostream& out, bool& sigintReceived);
    virtual ~CmdEnvir() {delete fakeGUI;}
    virtual void configure(cConfiguration *cfg) override;

    void setFakeGUI(FakeGUI *fakeGUI);
    virtual FakeGUI *getFakeGui() const override {return fakeGUI;}

    bool getAutoflush() const {return autoflush;}
    void setAutoflush(bool autoflush = false) {this->autoflush = autoflush;}
    bool isInteractive() const {return interactive;}
    void setInteractive(bool interactive) {this->interactive = interactive;}
    bool getPrintEventBanners() const {return printEventBanners;}
    void setPrintEventBanners(bool printEventBanners) {this->printEventBanners = printEventBanners;}

    virtual void log(cLogEntry *entry) override;
    virtual void alert(const char *msg) override;
    virtual bool askYesNo(const char *question) override;
    virtual void componentInitBegin(cComponent *component, int stage) override;
    virtual std::string input(const char *prompt, const char *defaultReply) override;
    virtual void undisposedObject(cObject *obj) override;

    virtual bool idle() override;
    virtual void pausePoint() override { /* currently no-op */ };

    virtual void getImageSize(const char *imageName, double& outWidth, double& outHeight) override;
    virtual void getTextExtent(const cFigure::Font& font, const char *text, double& outWidth, double& outHeight, double& outAscent) override;
    virtual void appendToImagePath(const char *directory) override;
    virtual void loadImage(const char *fileName, const char *imageName=nullptr) override;
    virtual cFigure::Rectangle getSubmoduleBounds(const cModule *submodule) override;
    virtual std::vector<cFigure::Point> getConnectionLine(const cGate *sourceGate) override;
    virtual double getZoomLevel(const cModule *module) override;
    virtual double getAnimationTime() const override;
    virtual double getAnimationSpeed() const override;
    virtual double getRemainingAnimationHoldTime() const override;

    virtual void configureComponent(cComponent *component) override;
    virtual void askParameter(cPar *par, bool unassigned) override;

    virtual bool ensureDebugger(cRuntimeError *error = nullptr) override;
};

}  // namespace cmdenv
}  // namespace omnetpp

#endif

