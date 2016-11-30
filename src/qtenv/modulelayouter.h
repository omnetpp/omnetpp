//==========================================================================
//  modulelayouter.h - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_QTENV_MODULELAYOUTER_H
#define __OMNETPP_QTENV_MODULELAYOUTER_H

#include <unordered_map>
#include <QObject>
#include <QPointF>
#include <QGraphicsScene>

namespace omnetpp {

class cModule;

namespace qtenv {

class ModuleLayouter : public QObject {
    Q_OBJECT

    // key is full type name as given by getObjectFullTypeName
    // can't use cModule pointer, these are persistent, would be cumbersome to restore
    std::unordered_map<std::string, int> layoutSeeds;
    // stores the layouted positions of submodules in their parents
    std::unordered_map<const cModule *, QPointF> modulePositions;

    // Extracts initial (fixed) coordinates from the displaystring of the module,
    // along with some other information about it. Used to
    void getSubmoduleCoords(cModule *submod, bool& explicitcoords, bool& obeysLayout,
        double& x, double& y, double& sx, double& sy);

signals:
    void layoutVisualisationStarts(cModule *module, QGraphicsScene *layoutingScene);
    void layoutVisualisationEnds(cModule *module);
    void moduleLayoutChanged(cModule *module);

    // this is used as a slot from the outside to forward the stop request to the visualizer environment
    void stop();

public slots:
    void clearLayout(cModule *module);
    // pointer only for convenience, the seed is shared for all instances of the class!
    void incrementLayoutSeed(cModule *module);
    void ensureLayouted(cModule *module); // with the same seed, reuses previous positions if any

    // equivalent to the above 3 in order
    void fullRelayout(cModule *module);

public:
    void loadSeeds();
    void saveSeeds();

    QPointF getModulePosition(cModule *module);
};

} // namespace qtenv
} // namespace omnetpp

#endif // __OMNETPP_QTENV_MODULELAYOUTER_H
