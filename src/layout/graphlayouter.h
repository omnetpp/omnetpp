//==========================================================================
//  GRAPHLAYOUTER.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_LAYOUT_GRAPHLAYOUTER_H
#define __OMNETPP_LAYOUT_GRAPHLAYOUTER_H

#include <string>
#include <map>
#include "common/lcgrandom.h"
#include "common/commonutil.h"
#include "layoutdefs.h"

namespace omnetpp {
namespace layout {

//#define TRACE_LAYOUTER

/**
 * Allows drawing during layouting for debug purposes, and makes it possible to
 * stop the layouter if it's taking too long.
 */
class LAYOUT_API GraphLayouterEnvironment
{
    public:
        virtual ~GraphLayouterEnvironment() {}
        virtual bool inspected() = 0;
        virtual bool okToProceed() = 0;  // should return false if layouting is taking too long

        virtual bool getBoolParameter(const char *name, int index, bool defaultValue) = 0;
        virtual long getLongParameter(const char *name, int index, long defaultValue) = 0;
        virtual double getDoubleParameter(const char *name, int index, double defaultValue) = 0;

        virtual void clearGraphics() = 0;
        virtual void showGraphics(const char *text) = 0;
        virtual void drawText(double x, double y, const char *text, const char *tags, const char *color) = 0;
        virtual void drawLine(double x1, double y1, double x2, double y2, const char *tags, const char *color) = 0;
        virtual void drawRectangle(double x1, double y1, double x2, double y2, const char *tags, const char *color) = 0;
};

/**
 * A universal minimal layouter environment: drawing operations are no-ops,
 * basic support for configurable parameters, timeout support.
 */
class LAYOUT_API BasicGraphLayouterEnvironment : public GraphLayouterEnvironment
{
    private:
        int timeout = 0;
        std::map<std::string,double> parameters;
        time_t startTime;

    public:
        BasicGraphLayouterEnvironment();
        virtual ~BasicGraphLayouterEnvironment() {}
        virtual bool inspected() override {return false;}
        virtual bool okToProceed() override;

        virtual void setTimeout(int seconds) {timeout = seconds;}
        virtual void restartTimeout() {startTime = time(nullptr);}
        virtual void addParameter(const char *name, double value) {parameters[name] = value;}

        virtual bool getBoolParameter(const char *name, int index, bool defaultValue) override;
        virtual long getLongParameter(const char *name, int index, long defaultValue) override;
        virtual double getDoubleParameter(const char *name, int index, double defaultValue) override;

        virtual void clearGraphics() override {}
        virtual void showGraphics(const char *text) override {}
        virtual void drawText(double x, double y, const char *text, const char *tags, const char *color) override {}
        virtual void drawLine(double x1, double y1, double x2, double y2, const char *tags, const char *color) override {}
        virtual void drawRectangle(double x1, double y1, double x2, double y2, const char *tags, const char *color) override {}
};

/**
 * Abstract base class for graph layouting algorithms.
 */
class LAYOUT_API GraphLayouter
{
  protected:
    omnetpp::common::LCGRandom lcgRandom;

    GraphLayouterEnvironment *environment = nullptr;

    double width = 0, height = 0;  // ==0 means unspecified
    double border = 0;

    double privRand01() { return lcgRandom.next01(); }
    double privUniform(double a, double b) { return a + privRand01() * (b - a); }

  public:
    /**
     * Ctor, dtor
     */
    //@{
    GraphLayouter() {}
    virtual ~GraphLayouter() {}
    //@}

    /**
     * Add node that can be moved.
     */
    virtual void addMovableNode(int nodeId, double width, double height) = 0;

    /**
     * Add fixed node. (x,y) denotes the center of the node.
     */
    virtual void addFixedNode(int nodeId, double x, double y, double width, double height) = 0;

    /**
     * Add node that is anchored to a freely movable anchor point. Nodes anchored
     * to the same anchor point can only move together. Anchor points are
     * identified by name, and they need not be predeclared (they are registered
     * on demand.) Usage: module vectors in ring, matrix, etc. layout.
     *
     * offx, offy: offset of the node center to the anchor point
     */
    virtual void addAnchoredNode(int nodeId, const char *anchorname, double offx, double offy, double width, double height) = 0;

    /**
     * Add connection (graph edge). len is the preferred length (0==unspecified)
     */
    virtual void addEdge(int srcNodeId, int destNodeId, double preferredLength=0) = 0;

    /**
     * Add connection (graph edge) to enclosing (parent) module. len is the
     * preferred length (0==unspecified)
     */
    virtual void addEdgeToBorder(int srcNodeId, double preferredLength=0) = 0;

    /**
     * Set parameters
     */
    //@{
    virtual void setEnvironment(GraphLayouterEnvironment *environment) {this->environment = environment;}

    /**
     * Set the random number seed used by the algorithm. The layouter has its
     * own RNG, i.e. does not use the simulation kernel's RNGs. The same seed
     * and same input results in the same layout.
     */
    void setSeed(int32_t seed) {
#ifdef TRACE_LAYOUTER
        TRACE_CALL("GraphLayouter::setSeed(seed: %d)", seed);
#endif
        lcgRandom.setSeed(seed);
    }

    /**
     * Return the seed value
     */
    int32_t getSeed() {
        int32_t seed = lcgRandom.getSeed();
#ifdef TRACE_LAYOUTER
        TRACE_CALL("GraphLayouter::getSeed(): %d", seed);
#endif
        return seed;
    }

    /**
     * Sets the size of the enclosing module. A zero width or height means
     * unspecified value.
     */
    void setSize(double width, double height, double border);
    //@}

    /**
     * The layouting algorithm.
     */
    virtual void execute() = 0;

    /**
     * Extracting the results. The returned position is the center of the module.
     */
    virtual void getNodePosition(int nodeId, double& x, double& y) = 0;
};

}  // namespace layout
}  // namespace omnetpp


#endif



