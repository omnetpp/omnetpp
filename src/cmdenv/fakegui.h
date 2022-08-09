//==========================================================================
//  FAKEGUI.H - part of
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

#ifndef __OMNETPP_CMDENV_FAKEGUI_H
#define __OMNETPP_CMDENV_FAKEGUI_H

#include "cmddefs.h"
#include "envir/ifakegui.h"
#include "common/lcgrandom.h"
#include "common/expression.h"

namespace omnetpp {

class cConfigOption;

namespace cmdenv {

using namespace omnetpp::envir;

/**
 * Fake GUI for Cmdenv.
 */
class CMDENV_API FakeGUI : public IFakeGUI
{
  private:
    cSimulation *simulation = nullptr;
    bool debug = false;
    double beforeEventProbability;
    double afterEventProbability;
    double onHoldProbability;
    common::Expression onHoldNumSteps;
    double onSimtimeProbability;
    common::Expression onSimtimeNumSteps;

    // state
    common::LCGRandom rng; // for randomizing calls to refreshDisplay()
    double animationTime = 0;
    simtime_t lastUpdateSimTime;

    // value for the "dt" expression variable, see Translator
    bool isWithinHold; //TODO fill
    simtime_t simulationTimeDelta;
    double animationTimeDelta;

    class Translator : public common::Expression::BasicAstTranslator {
        FakeGUI *fakeGui;
      public:
        Translator(FakeGUI *fakeGui) : fakeGui(fakeGui) {}
        virtual common::Expression::ExprNode *createIdentNode(const char *varName, bool withIndex) override;
        virtual common::Expression::ExprNode *createFunctionNode(const char *functionName, int argCount) override;
    };

   protected:
     virtual void parseExpression(cConfiguration *cfg, cConfigOption *configOption, common::Expression& expression);
     virtual double getAnimationHoldEndTime() const;
     virtual void animateHold();
     virtual void animateUntil(simtime_t targetSimTime);
     virtual int getHoldNumFrames(double dt);
     virtual int getSimulationNumFrames(simtime_t dt);
     virtual void callRefreshDisplay(const char *reason);
     virtual double getCurrentTimeDelta() const {return isWithinHold ? animationTimeDelta : simulationTimeDelta.dbl();}

   public:
     FakeGUI() {}
     virtual ~FakeGUI() {}
     cSimulation *getSimulation() const {return simulation;}
     virtual void configure(cSimulation *simulation, cConfiguration *cfg) override;
     virtual void beforeEvent(cEvent *event) override;
     virtual void afterEvent() override;
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
};

}  // namespace cmdenv
}  // namespace omnetpp

#endif

