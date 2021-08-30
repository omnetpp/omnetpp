//==========================================================================
//  modulelayouter.cc - part of
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

#include "modulelayouter.h"

#include <omnetpp/cdisplaystring.h>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QPushButton>
#include "qtenv.h"
#include "layout/graphlayouter.h"
#include "layout/basicspringembedderlayout.h"
#include "layout/forcedirectedgraphlayouter.h"
#include "layouterenv.h"
#include "mainwindow.h"
#include "submoduleitem.h"  // for DEFAULT_ICON

using namespace omnetpp::common;
using namespace omnetpp::layout;

#define emit

namespace omnetpp {
namespace qtenv {

class InteractiveTimeoutBasicGraphLayouterEnvironment : public BasicGraphLayouterEnvironment {

    bool okToProceed() override {
        if (BasicGraphLayouterEnvironment::okToProceed())
            return true;
        else {
            QMessageBox box(getQtenv()->getMainWindow());

            box.setIcon(QMessageBox::Question);
            box.setWindowTitle("Slow Layouting");
            box.setText("Laying out submodule positions is slower than usual.\nDo you want to proceed?");

            // The actual roles don't matter to us, but on different platforms they might be placed/ordered differently
            QPushButton *stopButton = box.addButton("Stop", QMessageBox::ButtonRole::NoRole);
            QPushButton *waitButton = box.addButton("Continue", QMessageBox::ButtonRole::YesRole);
            QPushButton *goButton = box.addButton("Run to completion", QMessageBox::ButtonRole::AcceptRole);

            box.exec();
            QAbstractButton *clickedButton = box.clickedButton();

            if (clickedButton == waitButton) {
                restartTimeout();
                return true;
            } else if (clickedButton == stopButton) {
                return false;
            } else if (clickedButton == goButton) {
                setTimeout(0);
                return true;
            } else
                return false;
        }
    }

};

ModuleLayouter::Constraint ModuleLayouter::getSubmoduleCoords(cModule *submod, double zoomFactor, double imageSizeFactor)
{
    const cDisplayString blank;
    const cDisplayString& ds = submod->hasDisplayString() && submod->parametersFinalized() ? submod->getDisplayString() : blank;

    // get size -- we'll need to return that too, and may be needed for matrix, ring etc. layout
    // -1 means unspecified
    double shapesx = -1, shapesy = -1;
    double iconsx = -1, iconsy = -1;

    // getting the "shape" (box or ellipse) size
    if (ds.containsTag("b")) {
        shapesx = resolveDoubleDispStrArg(ds.getTagArg("b", 0), submod, -1);
        shapesy = resolveDoubleDispStrArg(ds.getTagArg("b", 1), submod, -1);

        // if only one is present, use it for both
        if (shapesy == -1)
            shapesy = shapesx;
        if (shapesx == -1)
            shapesx = shapesy;

        // if none is present, factory defaults
        if (shapesx == -1) {
            shapesx = 40;
            shapesy = 24;
        }
    }

    //TODO do it like in submoduleitem.cc

    // getting the "image" size
    if (ds.containsTag("i")) {
        const char *imgName = ds.getTagArg("i", 0);
        if (*imgName) {
            // will return the unknownImage if imgName not found
            auto img = getQtenv()->icons.getImage(imgName, ds.getTagArg("is", 0));
            iconsx = img->width(); // note: no need to take image scaling factor and zoom level into account
            iconsy = img->height();
        }
    }
    else if (!ds.containsTag("b")) { // if no image nor shape size is specified, using the default submodule icon
        auto img = getQtenv()->icons.getImage(SubmoduleItem::DEFAULT_ICON, ds.getTagArg("is", 0));
        iconsx = img->width();
        iconsy = img->height();
    }

    // applying the scaling factors
    if (iconsx != -1) iconsx *= imageSizeFactor;
    if (iconsy != -1) iconsy *= imageSizeFactor;

    if (shapesx != -1) shapesx *= zoomFactor;
    if (shapesy != -1) shapesy *= zoomFactor;

    // if no shape, then "ignoring" it
    if (shapesx == -1) { // boxsx and boxsy are both defined or both undefined.
        shapesx = iconsx;
        shapesy = iconsy;
    }

    double sx = std::max(iconsx, shapesx);
    double sy = std::max(iconsy, shapesy);

    // first, see if there's an explicit position ("p=" tag) given
    double x = resolveDoubleDispStrArg(ds.getTagArg("p", 0), submod, -1);
    double y = resolveDoubleDispStrArg(ds.getTagArg("p", 1), submod, -1);
    bool explicitCoords = x != -1 && y != -1;

    // set missing coordinates to zero
    if (x == -1)
        x = 0;
    if (y == -1)
        y = 0;

    const char *layout = ds.getTagArg("p", 2);  // matrix, row, column, ring, exact etc.
    bool obeysLayout = !opp_isempty(layout);
    const char *groupName = "";

    // modify x,y using predefined layouts
    if (!obeysLayout) {
        // we're happy
    }
    else if (!strcmp(layout, "e") || !strcmp(layout, "x") || !strcmp(layout, "exact")) {
        double dx = resolveDoubleDispStrArg(ds.getTagArg("p", 3), submod, 0);
        double dy = resolveDoubleDispStrArg(ds.getTagArg("p", 4), submod, 0);
        x += dx;
        y += dy;
    }
    else {
        groupName = ds.getTagArg("g", 0);
        int index = 0, groupSize = 1;
        if (opp_isempty(groupName)) {
            groupName = submod->getName();
            if (submod->isVector()) {
                index = submod->getIndex();
                groupSize = submod->getVectorSize();
            }
        }
        else {
            auto& group = groups[submod->getParentModule()][groupName];
            if (group.indices.find(submod) != group.indices.end())
                index = group.indices[submod];
            else if (group.size == group.indices.size())
                index = group.indices[submod] = group.size++;
            else {
                // find first unused index
                std::vector<bool> indexUsed(group.size, false);
                for (auto& pair : group.indices)
                    indexUsed[pair.second] = true;
                auto it = std::find(indexUsed.begin(), indexUsed.end(), true);
                ASSERT(it != indexUsed.end());
                index = group.indices[submod] = it - indexUsed.begin();
            }
            groupSize = group.size;
        }

        if (!strcmp(layout, "r") || !strcmp(layout, "row")) {
            // perhaps we should use the size of the 1st element in the vector?
            double dx = resolveDoubleDispStrArg(ds.getTagArg("p", 3), submod, 2*sx);
            x += index * dx;
        }
        else if (!strcmp(layout, "c") || !strcmp(layout, "col") || !strcmp(layout, "column")) {
            double dy = resolveDoubleDispStrArg(ds.getTagArg("p", 3), submod, 2*sy);
            y += index * dy;
        }
        else if (!strcmp(layout, "m") || !strcmp(layout, "matrix")) {
            // perhaps we should use the size of the 1st element in the vector?
            int columns = resolveLongDispStrArg(ds.getTagArg("p", 3), submod, 5);
            double dx = resolveDoubleDispStrArg(ds.getTagArg("p", 4), submod, 2*sx);
            double dy = resolveDoubleDispStrArg(ds.getTagArg("p", 5), submod, 2*sy);
            if (columns < 1)
                columns = 1;
            x += (index % columns) * dx;
            y += (index / columns) * dy;
        }
        else if (!strcmp(layout, "i") || !strcmp(layout, "ri") || !strcmp(layout, "ring")) {
            // perhaps we should use the size of the 1st element in the vector?
            double rx = resolveDoubleDispStrArg(ds.getTagArg("p", 3), submod, (sx+sy)*groupSize/4);
            double ry = resolveDoubleDispStrArg(ds.getTagArg("p", 4), submod, rx);

            x += rx - rx * sin(index * 2 * M_PI / groupSize);
            y += ry - ry * cos(index * 2 * M_PI / groupSize);
        }
        else {
            throw cRuntimeError("Invalid layout '%s' in 'p' tag of display string \"%s\"", layout, ds.str());
        }
    }

    x *= zoomFactor;
    y *= zoomFactor;

    return Constraint(explicitCoords, obeysLayout, groupName, x, y, sx, sy);
}

void ModuleLayouter::clearLayout(cModule *module)
{
    for (cModule::SubmoduleIterator it(module); !it.end(); ++it)
        modulePositions.erase(*it);

    auto it = groups.find(module);
    if (it != groups.end())
        groups.erase(it);
}

void ModuleLayouter::forgetPosition(cModule *submodule)
{
    modulePositions.erase(submodule);

    // remove submodule from layout group if it's part of one
    cModule *parent = submodule->getParentModule();
    auto it = groups.find(parent);
    if (it != groups.end()) {
        ModuleGroups& moduleGroups = it->second;
        for (auto pair : moduleGroups) { // try all groups
            Group& group = pair.second;
            auto it = group.indices.find(submodule);
            if (it != group.indices.end()) {
                int index = it->second;
                group.indices.erase(it);
                // adjust group size if needed
                if (index == group.size-1) {
                    group.size = index-1;
                    if (group.size > 0) {
                        // find largest index among remaining ones, and use that+1 as size
                        using pair_type = decltype(group.indices)::value_type;
                        auto maxIt = std::max_element(group.indices.begin(), group.indices.end(),
                                [] (const pair_type& a, const pair_type& b) -> bool { return a.second < b.second; } );
                        group.size = maxIt->second+1;
                    }
                }
            }
        }
    }
}

void ModuleLayouter::refreshPositionFromDS(cModule *submodule)
{
    Constraint r = getSubmoduleCoords(submodule);

    if (r.explicitCoords)
        modulePositions[submodule] = QPointF(r.x, r.y);
}

void ModuleLayouter::incrementLayoutSeed(cModule *module)
{
    std::string fullName = getObjectFullTypeName(module);

    if (layoutSeeds.find(fullName) == layoutSeeds.end())
        layoutSeeds[fullName] = 1; // must not use 0, the LCGRandom PRNG in the advanced layouter can't take that!

    ++layoutSeeds[fullName];
}

void ModuleLayouter::ensureLayouted(cModule *module)
{
    if (!module)
        return;

    bool needsLayout = false;

    // loop through all submodules, and check if there are any that we don't have a valid position for
    for (cModule::SubmoduleIterator it(module); !it.end(); ++it)
        if (modulePositions.find(*it) == modulePositions.end()) {
            needsLayout = true;
            break;
        }

    // if not, we have nothing to do
    if (!needsLayout)
        return;

    // recalculate layout, using coordinates in submodPosMap as "fixed" nodes --
    // only new nodes are re-layouted

    // Note trick avoid calling getDisplayString() directly because it'd cause
    // the display string object inside cModule to spring into existence
    const cDisplayString blank;
    const cDisplayString& ds = module->hasDisplayString() && module->parametersFinalized() ? module->getDisplayString() : blank;

    // create and configure layouter object
    LayouterChoice choice = getQtenv()->opt->layouterChoice;
    if (choice == LAYOUTER_AUTO) {
        const int LIMIT = 20;  // note: on test/anim/dynamic2, Advanced is already very slow with 30-40 modules
        int submodCountLimited = 0;
        for (cModule::SubmoduleIterator it(module); !it.end() && submodCountLimited < LIMIT; ++it)
            submodCountLimited++;
        choice = submodCountLimited >= LIMIT ? LAYOUTER_FAST : LAYOUTER_ADVANCED;
    }
    GraphLayouter *graphLayouter = choice == LAYOUTER_FAST ?
                (GraphLayouter *)new BasicSpringEmbedderLayout() :
                (GraphLayouter *)new ForceDirectedGraphLayouter();


    std::string fullName = getObjectFullTypeName(module);

    if (layoutSeeds.find(fullName) == layoutSeeds.end())
        layoutSeeds[fullName] = 1; // must not use 0, the LCGRandom PRNG in the advanced layouter can't take that!

    graphLayouter->setSeed(layoutSeeds[fullName]);

    // background size
    double sx = resolveDoubleDispStrArg(ds.getTagArg("bgb", 0), module, 0);
    double sy = resolveDoubleDispStrArg(ds.getTagArg("bgb", 1), module, 0);
    double border = 30;
    if (sx != 0 && sx < 2*border)
        border = sx/2;
    if (sy != 0 && sy < 2*border)
        border = sy/2;
    graphLayouter->setSize(sx, sy, border);
    // TODO support "bgp" tag ("background position")

    // collect modules in various layout groups
    auto& moduleGroups = groups[module];
    moduleGroups.clear();
    for (cModule::SubmoduleIterator it(module); !it.end(); ++it) {
        cModule *submod = *it;
        if (submod->hasDisplayString() && submod->parametersFinalized()) {
            const char *groupName = submod->getDisplayString().getTagArg("g", 0);
            if (!opp_isempty(groupName)) {
                auto& group = moduleGroups[groupName];
                group.indices[submod] = group.size++;
            }
        }
    }

    // loop through all submodules, get their sizes and positions and feed them into layouting engine
    for (cModule::SubmoduleIterator it(module); !it.end(); ++it) {
        cModule *submod = *it;

        Constraint r = getSubmoduleCoords(submod);

        // add node into layouter:
        if (r.explicitCoords) {
            // e.g. "p=120,70" or "p=140,30,ring"
            graphLayouter->addFixedNode(submod->getId(), r.x, r.y, r.sx, r.sy);
        }
        else if (modulePositions.find(submod) != modulePositions.end()) {
            // reuse coordinates from previous layout
            QPointF pos = modulePositions[submod];
            graphLayouter->addFixedNode(submod->getId(), pos.x(), pos.y(), r.sx, r.sy);
        }
        else if (r.obeysLayout) {
            // all modules are anchored to the anchor point with the vector's name
            // e.g. "p=,,ring"
            graphLayouter->addAnchoredNode(submod->getId(), r.group.c_str(), r.x, r.y, r.sx, r.sy);
        }
        else {
            graphLayouter->addMovableNode(submod->getId(), r.sx, r.sy);
        }
    }

    // add connections into the layouter, too
    bool atParent = false;
    for (cModule::SubmoduleIterator it(module); !atParent; ++it) {
        cModule *mod = !it.end() ? *it : (atParent = true, module);

        for (cModule::GateIterator git(mod); !git.end(); ++git) {
            cGate *gate = *git;
            cGate *destGate = gate->getNextGate();
            if (gate->getType() == (atParent ? cGate::INPUT : cGate::OUTPUT) && destGate) {
                cModule *destMod = destGate->getOwnerModule();
                if (mod == module && destMod == module) {
                    // nop
                }
                else if (destMod == module) {
                    graphLayouter->addEdgeToBorder(mod->getId());
                }
                else if (destMod->getParentModule() != module) {
                    // connection goes to a module under a different parent!
                    // this in fact violates module encapsulation, but let's
                    // accept it nevertheless. Just skip this connection.
                }
                else if (mod == module) {
                    graphLayouter->addEdgeToBorder(destMod->getId());
                }
                else {
                    graphLayouter->addEdge(mod->getId(), destMod->getId());
                }
            }
        }
    }

    // checking if any of the submodules already have a position
    bool fullLayout = true;
    for (cModule::SubmoduleIterator it(module); !it.end(); ++it)
        if (modulePositions.find(*it) != modulePositions.end())
            fullLayout = false;

    if (fullLayout && getQtenv()->opt->showLayouting) {
        // set up layouter environment (responsible for "Stop" button handling and visualizing the layouting process)
        QGraphicsScene *layoutingScene = new QGraphicsScene;

        { // block is to force destruction of the layouting enironment
            QtenvGraphLayouterEnvironment qtenvEnvironment(module, ds, layoutingScene);
            connect(this, SIGNAL(stop()), &qtenvEnvironment, SLOT(stop()));

            emit layoutVisualisationStarts(module, layoutingScene);

            graphLayouter->setEnvironment(&qtenvEnvironment);

            graphLayouter->execute();

            emit layoutVisualisationEnds(module);
        }

        delete layoutingScene;
    }
    else {
        // we still have to set something for the layouter if visualisation is disabled.
        InteractiveTimeoutBasicGraphLayouterEnvironment basicEnvironment;
        basicEnvironment.setTimeout(5);

        graphLayouter->setEnvironment(&basicEnvironment);

        graphLayouter->execute();
    }

    bool changed = false;
    // fill the map with the results
    for (cModule::SubmoduleIterator it(module); !it.end(); ++it) {
        cModule *submod = *it;

        QPointF pos;
        double x, y;
        graphLayouter->getNodePosition(submod->getId(), x, y);
        pos.setX(x);
        pos.setY(y);

        if (modulePositions.find(submod) == modulePositions.end() || modulePositions[submod] != pos)
            changed = true;
        modulePositions[submod] = pos;
    }

    // XXX is this needed?
    //layoutSeeds[fullName] = graphLayouter->getSeed();

    delete graphLayouter;

    if (changed)
        emit moduleLayoutChanged(module);
}

QPointF ModuleLayouter::getModulePosition(cModule *module, double zoomFactor)
{
    auto it = modulePositions.find(module);
    return it == modulePositions.end() ? QPointF(NAN, NAN) : (it->second * zoomFactor);
}

QRectF ModuleLayouter::getModuleRectangle(cModule *module, double zoomFactor, double imageSizeFactor)
{
    Constraint r = getSubmoduleCoords(module, zoomFactor, imageSizeFactor);
    // using only the size from the above, position from our layouted map

    QPointF center = getModulePosition(module, zoomFactor);

    return QRectF(center.x() - r.sx/2, center.y() - r.sy/2, r.sx, r.sy);
}

void ModuleLayouter::fullRelayout(cModule *module)
{
    clearLayout(module);
    incrementLayoutSeed(module);
    ensureLayouted(module);
}

void ModuleLayouter::loadSeeds()
{
    auto env = getQtenv();
    auto seeds = env->getKeysInPrefGroup("LayoutSeeds");

    for (auto s : seeds)
        layoutSeeds[s.toStdString()] = env->getPref("LayoutSeeds/" + s, 1).toInt();

    /* // DEBUG OUTPUT
    qDebug() << "seeds after load:";
    for (auto p : layoutSeeds)
        qDebug() << p.first.c_str() << " -> " << p.second;
    qDebug() << "----";
    */
}

void ModuleLayouter::saveSeeds()
{
    /* // DEBUG OUTPUT
    qDebug() << "seeds before save:";
    for (auto p : layoutSeeds)
        qDebug() << p.first.c_str() << " -> " << p.second;
    qDebug() << "----";
    */

    auto env = getQtenv();

    for (auto p : layoutSeeds)
        env->setPref(QString("LayoutSeeds/") + p.first.c_str(), p.second);
}

}  // namespace qtenv
}  // namespace omnetpp
