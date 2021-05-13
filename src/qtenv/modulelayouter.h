//==========================================================================
//  modulelayouter.h - part of
//
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

#ifndef __OMNETPP_QTENV_MODULELAYOUTER_H
#define __OMNETPP_QTENV_MODULELAYOUTER_H

#include <unordered_map>
#include <QtCore/QObject>
#include <QtCore/QPointF>
#include <QtWidgets/QGraphicsScene>
#include "qtenvdefs.h"

namespace omnetpp {

class cModule;

namespace qtenv {

class QTENV_API ModuleLayouter : public QObject {
    Q_OBJECT

    // key is full type name as given by getObjectFullTypeName
    // can't use cModule pointer, these are persistent, would be cumbersome to restore
    std::unordered_map<std::string, int> layoutSeeds;
    // stores the layouted positions of submodules in their parents
    std::unordered_map<const cModule *, QPointF> modulePositions;

    // layout groups
    struct Group { int size = 0; std::unordered_map<const cModule *,int> indices; };
    typedef std::unordered_map<std::string,Group> ModuleGroups; // groupName -> Group
    std::unordered_map<const cModule *, ModuleGroups> groups; // parentModule -> groups

    struct Constraint {
        bool explicitCoords; // =true: "p" tag was given, i.e. is fixed node
        bool obeysLayout; // =true: is anchored node, i.e. (x,y) means anchor-relative position
        std::string group;
        double x, y;    // absolute position or anchor-relative offset (of the shape's center)
        double sx, sy;  // size of bounding box
        Constraint(bool explicitCoords, bool obeysLayout, const char *group, double x, double y, double sx, double sy) :
            explicitCoords(explicitCoords), obeysLayout(obeysLayout), group(group), x(x), y(y), sx(sx), sy(sy) {}
    };

    // Extracts initial (fixed) coordinates from the displaystring of the module,
    // along with some other information about it. Used to feed the layouter, and
    // to provide positions/rectangles for the animator/environment/model.
    Constraint getSubmoduleCoords(cModule *submod, double zoomFactor = 1.0, double imageSizeFactor = 1.0);

signals:
    void layoutVisualisationStarts(cModule *module, QGraphicsScene *layoutingScene);
    void layoutVisualisationEnds(cModule *module);
    void moduleLayoutChanged(cModule *module);

    // this is used as a slot from the outside to forward the stop request to the visualizer environment
    void stop();

public slots:
    void clearLayout(cModule *module);
    void forgetPosition(cModule *submodule);
    void refreshPositionFromDS(cModule *submodule);
    // pointer only for convenience, the seed is shared for all instances of the class!
    void incrementLayoutSeed(cModule *module);
    void ensureLayouted(cModule *module); // with the same seed, reuses previous positions if any

    // equivalent to the above 3 in order
    void fullRelayout(cModule *module);

public:
    void loadSeeds();
    void saveSeeds();

    QPointF getModulePosition(cModule *module, double zoomFactor = 1.0); // the center of the shape/icon. (NAN,NAN) if not yet layouted
    QRectF getModuleRectangle(cModule *module, double zoomFactor = 1.0, double imageSizeFactor = 1.0); // the bounding box of the shape and/or icon. full of NANs if not yet layouted
};

}  // namespace qtenv
}  // namespace omnetpp

#endif // __OMNETPP_QTENV_MODULELAYOUTER_H
