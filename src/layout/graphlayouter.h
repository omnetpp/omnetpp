//==========================================================================
//  GRAPHLAYOUTER.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Levente Meszaros
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __GRAPHLAYOUTER_H
#define __GRAPHLAYOUTER_H

#include "layoutdefs.h"
#include "lcgrandom.h"

NAMESPACE_BEGIN

class cModule;

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

        virtual bool getBoolParameter(const char *tagName, int index, bool defaultValue) = 0;
        virtual long getLongParameter(const char *tagName, int index, long defaultValue) = 0;
        virtual double getDoubleParameter(const char *tagName, int index, double defaultValue) = 0;

        virtual void clearGraphics() = 0;
        virtual void showGraphics(const char *text) = 0;
        virtual void drawText(double x, double y, const char *text, const char *tags, const char *color) = 0;
        virtual void drawLine(double x1, double y1, double x2, double y2, const char *tags, const char *color) = 0;
        virtual void drawRectangle(double x1, double y1, double x2, double y2, const char *tags, const char *color) = 0;
};

/**
 * Abstract base class for graph layouting algorithms.
 */
class LAYOUT_API GraphLayouter
{
  protected:
    LCGRandom lcgRandom;

    GraphLayouterEnvironment *environment;

    int width, height, border;
    enum {Free, Confine, Scale} sizingMode;

    double privRand01() { return lcgRandom.next01(); }

  public:
    /**
     * Ctor, dtor
     */
    //@{
    GraphLayouter();
    virtual ~GraphLayouter() {}
    //@}

    /**
     * Add node that can be moved.
     */
    virtual void addMovableNode(cModule *mod, int width, int height) = 0;

    /**
     * Add fixed node. (x,y) denotes the center of the node.
     */
    virtual void addFixedNode(cModule *mod, int x, int y, int width, int height) = 0;

    /**
     * Add node that is anchored to a freely movable anchor point. Nodes anchored
     * to the same anchor point can only move together. Anchor points are
     * identified by name, and they need not be predeclared (they are registered
     * on demand.) Usage: module vectors in ring, matrix, etc. layout.
     *
     * offx, offy: offset of the node center to the anchor point
     */
    virtual void addAnchoredNode(cModule *mod, const char *anchorname, int offx, int offy, int width, int height) = 0;

    /**
     * Add connection (graph edge). len is the preferred length (0==unspecified)
     */
    virtual void addEdge(cModule *from, cModule *to, int len=0) = 0;

    /**
     * Add connection (graph edge) to enclosing (parent) module. len is the
     * preferred length (0==unspecified)
     */
    virtual void addEdgeToBorder(cModule *from, int len=0) = 0;

    /**
     * Set parameters
     */
    //@{
    virtual void setEnvironment(GraphLayouterEnvironment *environment) {this->environment = environment;}
    void setSeed(int32 seed) { lcgRandom.setSeed(seed);}
    int32 getSeed() { return lcgRandom.getSeed(); }
    void setConfineToArea(int width, int height, int border); // TBD currently ignored
    void setScaleToArea(int width, int height, int border);
    //@}

    /**
     * The layouting algorithm.
     */
    virtual void execute() = 0;

    /**
     * Extracting the results. The returned position is the center of the module.
     */
    virtual void getNodePosition(cModule *mod, int& x, int& y) = 0;
};

NAMESPACE_END


#endif



