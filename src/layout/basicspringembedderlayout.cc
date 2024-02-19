//==========================================================================
//  BASICSPRINGEMBEDDERLAYOUT.CC - part of
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

#include <cstdio>
#include <ctime>
#include <cmath>
#include <cfloat>
#include <cstdlib>
#include <sstream>
#include <set>
#include <deque>
#include <algorithm>

#include "common/commonutil.h"
#include "basicspringembedderlayout.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace layout {

static bool debug = false;

BasicSpringEmbedderLayout::~BasicSpringEmbedderLayout()
{
    for (auto & anchor : anchors)
        delete anchor;
    for (auto & node : nodes)
        delete node;
}

void BasicSpringEmbedderLayout::setEnvironment(GraphLayouterEnvironment *environment)
{
    GraphLayouter::setEnvironment(environment);

    repulsiveForce = environment->getDoubleParameter("bgl", 0, repulsiveForce);
    attractionForce = environment->getDoubleParameter("bgl", 1, attractionForce);
    defaultEdgeLen = environment->getDoubleParameter("bgl", 2, defaultEdgeLen);
    maxIterations = environment->getLongParameter("bgl", 3, maxIterations);
}

BasicSpringEmbedderLayout::Node *BasicSpringEmbedderLayout::findNode(int nodeId)
{
    NodeMap::iterator it = nodeMap.find(nodeId);
    return it == nodeMap.end() ? nullptr : it->second;
}

void BasicSpringEmbedderLayout::addMovableNode(int nodeId, double width, double height)
{
#ifdef TRACE_LAYOUTER
    TRACE_CALL("BasicSpringEmbedderLayout::addMovableNode(nodeId: %d, width: %g, height: %g)", nodeId, width, height);
#endif
    Assert(findNode(nodeId) == nullptr);

    allNodesAreFixed = false;

    Node *n = new Node();
    n->nodeId = nodeId;
    n->fixed = false;
    n->anchor = nullptr;
    n->sx = width/2;
    n->sy = height/2;

    nodes.push_back(n);
    nodeMap[nodeId] = n;
}

void BasicSpringEmbedderLayout::addFixedNode(int nodeId, double x, double y, double width, double height)
{
#ifdef TRACE_LAYOUTER
    TRACE_CALL("BasicSpringEmbedderLayout::addFixedNode(nodeId: %d, x: %g, y: %g, width: %g, height: %g)", nodeId, x, y, width, height);
#endif
    Assert(findNode(nodeId) == nullptr);

    haveFixedNode = true;

    Node *n = new Node();
    n->nodeId = nodeId;
    n->fixed = true;
    n->anchor = nullptr;
    n->x = x;
    n->y = y;
    n->sx = width/2;
    n->sy = height/2;

    nodes.push_back(n);
    nodeMap[nodeId] = n;
}

void BasicSpringEmbedderLayout::addAnchoredNode(int nodeId, const char *anchorname, double offx, double offy, double width, double height)
{
#ifdef TRACE_LAYOUTER
    TRACE_CALL("BasicSpringEmbedderLayout::addAnchoredNode(nodeId: %d, anchorname: %s, offx: %g, offy: %g, width: %g, height: %g)", nodeId, anchorname, offy, offx, width, height);
#endif
    Assert(findNode(nodeId) == nullptr);

    haveAnchoredNode = true;
    allNodesAreFixed = false;

    Node *n = new Node();
    n->nodeId = nodeId;

    Anchor *anchor;
    AnchorList::iterator a;
    for (a = anchors.begin(); a != anchors.end(); ++a)
        if ((*a)->name == anchorname)
            break;

    if (a == anchors.end()) {
        anchors.push_back(anchor = new Anchor());
        anchor->name = std::string(anchorname);
        anchor->refcount = 0;
    }
    else {
        anchor = (*a);
    }

    n->anchor = anchor;
    anchor->refcount++;

    n->fixed = false;
    n->offx = offx;
    n->offy = offy;
    n->sx = width/2;
    n->sy = height/2;

    nodes.push_back(n);
    nodeMap[nodeId] = n;
}

void BasicSpringEmbedderLayout::addEdge(int srcNodeId, int destNodeId, double len)
{
    Assert(findNode(srcNodeId) != nullptr && findNode(destNodeId) != nullptr);

    Edge e;
    e.src = findNode(srcNodeId);
    e.dest = findNode(destNodeId);
    e.len = len > 0 ? len : defaultEdgeLen;

    // heuristics to take submodule size into account
    e.len += std::min(e.src->sx, e.src->sy)/2 + std::min(e.dest->sx, e.dest->sy)/2;

    edges.push_back(e);
}

void BasicSpringEmbedderLayout::addEdgeToBorder(int, double)
{
    // this layouter algorithm ignores connections to border
}

void BasicSpringEmbedderLayout::getNodePosition(int nodeId, double& x, double& y)
{
    Assert(findNode(nodeId) != nullptr);

    Node *n = findNode(nodeId);
    x = n->x;
    y = n->y;
}

void BasicSpringEmbedderLayout::execute()
{
#ifdef TRACE_LAYOUTER
    TRACE_CALL("BasicSpringEmbedderLayout::execute()");
#endif
    Assert(environment != nullptr);

    if (nodes.empty() || allNodesAreFixed)
        return;

    Assert(width == 0 || width >= 2*border);  // also ensured in setSize()
    Assert(height == 0 || height >= 2*border);

    computeAnchorBoundingBoxes();

    // assign random start positions to non-fixed nodes; we distribute them
    // over an area that is proportional to the number of nodes, and also
    // takes into account the area already covered by fixed nodes.
    assignInitialPositions();

    // set area
    if (!haveFixedNode) {
        // layout on an infinite area, because we can scale/shift the nodes
        // into the box after layouting
        minx = -100000000;
        miny = -100000000;
        maxx = 100000000;
        maxy = 100000000;
    }
    else {
        // we assume the top-left corner is (0,0), as bgp, "bg position" tag is not yet implemented
        minx = border;
        miny = border;
        maxx = width == 0 ? 100000000 : (width - border);
        maxy = height == 0 ? 100000000 : (height - border);
    }

    // partition the graph
    int numColors = doColoring();
    markNodesConnectedToFixed(numColors);

    // now the real job -- stop if max moved distance is <0.05 at least 20 times in a row
    clock_t beg = clock();
    int i, maxdcounter = 0;
    for (i = 1; i < maxIterations && maxdcounter < 20 && environment->okToProceed(); i++) {
        double maxd = relax();

        if (environment->inspected())
            debugDraw(i);

        if (maxd < 0.05)
            maxdcounter++;
        else
            maxdcounter = 0;
    }
    clock_t end = clock();
    if (debug)
        printf("DBG: layout done in %g secs, %d iterations (%g sec/iter)\n", (end-beg)/(double)CLOCKS_PER_SEC, i, (end-beg)/(double)CLOCKS_PER_SEC/i);

    // if area width or height was specified by the user, we may need to scale back
    // the positions so that the picture fita into the given area -- BUT we can only
    // do that if we don't have any fixed (or anchored) nodes, because we don't want
    // to change explicitly given coordinates (or distances between anchored nodes)
    if (!haveFixedNode) {
        // determine bounding box
        double x1, y1, x2, y2;
        computeBoundingBox(x1, y1, x2, y2, &anyNode);

        // rescale and shift. We don't rescale (only shift) if:
        //   - width (height) was unspecified;
        //   - if x1==x2 (to avoid division by zero);
        //   - there are anchored nodes (their spacing must be preserved)
        double xfact = (width == 0 || x1 == x2 || haveAnchoredNode) ? 1.0 : (width-2*border) / (x2-x1);
        double yfact = (height == 0 || y1 == y2 || haveAnchoredNode) ? 1.0 : (height-2*border) / (y2-y1);
        for (auto & node : nodes) {
            Node& n = *node;
            n.x = border + (n.x-x1)*xfact;
            n.y = border + (n.y-y1)*yfact;
        }
    }
    else if (width == 0 && height == 0) {
        // if no bgsize is given, we've let some movable nodes (those not connected to
        // any fixed node directly or indirectly) escape towards top and left for a better layout;
        // now we need to shift them back.
        double x1, y1, x2, y2;
        computeBoundingBox(x1, y1, x2, y2, &isNotConnectedToFixed);

        // if bounding box is off the screen top and/or left, shift them back
        double dx = 0, dy = 0;
        if (x1 < border)
            dx = border-x1;
        if (y1 < border)
            dy = border-y1;

        for (auto & node : nodes) {
            Node& n = *node;
            if (!n.connectedToFixed) {
                n.x += dx;
                n.y += dy;
            }
        }
    }
}

void BasicSpringEmbedderLayout::assignInitialPositions()
{
    // assign random start positions to non-fixed nodes. We distribute them
    // over the area already occupied by fixed nodes or over an area calculated
    // from the number of nodes (x square-pixels per node) whichever is larger.
    // The former is esp. impotant, because during incremental layout
    // (i.e. when placing newly created modules while the simulation is running)
    // all existing modules are taken as fixed.
    //
    double initialAreaWidth, initialAreaHeight;
    double initialAreaOffsetX, initialAreaOffsetY;

    if (width != 0 && height != 0) {
        initialAreaWidth = width - 2*border;
        initialAreaHeight = height - 2*border;
        initialAreaOffsetX = initialAreaOffsetY = border;
    }
    else {
        // compute initialAreaWidth/height/offsetX/offsetY
        double area = std::max(60.0 * 60.0 * nodes.size(), 600.0 * 400.0);  // assume 1 node needs 60x60 pixels of space
        double aspectRatio = 1.5;
        initialAreaWidth = sqrt(area)*aspectRatio;
        initialAreaHeight = sqrt(area)/aspectRatio;

        double margin = 0.25 * std::min(initialAreaWidth, initialAreaHeight);
        initialAreaOffsetX = initialAreaOffsetY = margin;  // leave some space (25%) top and left side, to reduce movable nodes getting pushed against the wall
        if (haveFixedNode) {
            // compute bounding box of fixed nodes
            double x1, y1, x2, y2;
            computeBoundingBox(x1, y1, x2, y2, &isFixedNode);

            // shrink this bb somewhat, so that nodes don't get placed near the borders of this area
            // (because other nodes could push them out, therefore resulting in a larger bounding box
            // in the next round of incremental layouting)
            double xmargin = std::min(150.0, (x2-x1)/2);
            double ymargin = std::min(150.0, (y2-y1)/2);
            x1 += xmargin;
            x2 -= xmargin;
            y1 += ymargin;
            y2 -= ymargin;

            // union with initial area
            double initialAreaRight = initialAreaOffsetX + initialAreaWidth;
            double initialAreaBottom = initialAreaOffsetY + initialAreaHeight;
            initialAreaOffsetX = std::min(x1, initialAreaOffsetX);
            initialAreaOffsetY = std::min(y1, initialAreaOffsetY);
            initialAreaRight = std::max(x2, initialAreaRight);
            initialAreaBottom = std::max(y2, initialAreaBottom);
            initialAreaWidth = initialAreaRight - initialAreaOffsetX;
            initialAreaHeight = initialAreaBottom - initialAreaOffsetY;
        }
    }

    // initialize variables (also randomize start positions over the initial area)
    for (auto & anchor : anchors) {
        Anchor& a = *anchor;
        double nodesBBWidth = a.x2off - a.x1off;
        double nodesBBHeight = a.y2off - a.y1off;
        double nodesBBLeft = initialAreaOffsetX + std::max(0.0, initialAreaWidth-nodesBBWidth) * privRand01();
        double nodesBBTop = initialAreaOffsetY + std::max(0.0, initialAreaHeight-nodesBBHeight) * privRand01();
        a.x = nodesBBLeft - a.x1off;
        a.y = nodesBBTop - a.y1off;
        a.vx = a.vy = 0;
    }
    for (auto & node : nodes) {
        Node& n = *node;
        if (n.fixed) {
            // nop
        }
        else if (n.anchor) {
            n.x = n.anchor->x + n.offx;
            n.y = n.anchor->y + n.offy;
        }
        else {  // movable
            n.x = initialAreaOffsetX + initialAreaWidth * privRand01();
            n.y = initialAreaOffsetY + initialAreaHeight * privRand01();
        }
        n.vx = n.vy = 0;
    }
}

int BasicSpringEmbedderLayout::doColoring()
{
    NodeList::iterator i;
    for (i = nodes.begin(); i != nodes.end(); ++i)
        (*i)->color = -1;

    int currentColor = 0;
    std::deque<Node *> todoList;
    for (i = nodes.begin(); i != nodes.end(); ++i) {
        Node *n = *i;
        if (n->color != -1)
            continue;  // already assigned

        // breadth-width search to color all connected nodes (transitive closure)
        Assert(todoList.empty());
        todoList.push_back(n);  // start at this node
        while (!todoList.empty()) {
            Node *n = todoList.back();
            todoList.pop_back();

            n->color = currentColor;

            // color and add to list all nodes connected to n.
            // (edge list data structure is not really good for this, but execution
            // time is still negligable compared to relax() iterations)
            EdgeList::iterator k;
            for (k = edges.begin(); k != edges.end(); ++k) {
                Edge& e = *k;
                if (e.src == n && e.dest->color == -1)
                    todoList.push_back(e.dest);
                else if (e.dest == n && e.src->color == -1)
                    todoList.push_back(e.src);
            }
        }

        // next color
        currentColor++;
    }

    // return the number of colors used
    return currentColor;
}

void BasicSpringEmbedderLayout::markNodesConnectedToFixed(int numColors)
{
    // compute the set of colors (connected graph partitions) that contain at least one fixed node
    std::set<int> colorsWithFixedNodes;
    for (auto n : nodes) {
        if (n->fixed)
            colorsWithFixedNodes.insert(n->color);
    }

    //  set the connectedToFixed fields of nodes
    for (auto n : nodes) {
        n->connectedToFixed = colorsWithFixedNodes.find(n->color) != colorsWithFixedNodes.end();
    }
}

void BasicSpringEmbedderLayout::computeAnchorBoundingBoxes()
{
    // fills in x1off, y1off, x2off, y2off fields of anchors
    for (auto & anchor : anchors) {
        Anchor& a = *anchor;
        a.x1off = POSITIVE_INFINITY;
        a.y1off = POSITIVE_INFINITY;
        a.x2off = NEGATIVE_INFINITY, a.y2off = NEGATIVE_INFINITY;
    }

    for (auto & node : nodes) {
        Node& n = *node;
        if (n.anchor) {
            Anchor& a = *(n.anchor);
            if (n.offx-n.sx < a.x1off)
                a.x1off = n.offx - n.sx;
            if (n.offy-n.sy < a.y1off)
                a.y1off = n.offy - n.sy;
            if (n.offx+n.sx > a.x2off)
                a.x2off = n.offx + n.sx;
            if (n.offy+n.sy > a.y2off)
                a.y2off = n.offy + n.sy;
        }
    }
}

void BasicSpringEmbedderLayout::computeBoundingBox(double& x1, double& y1, double& x2, double& y2, bool (*predicate)(Node *))
{
    x1 = POSITIVE_INFINITY;
    y1 = POSITIVE_INFINITY;
    x2 = NEGATIVE_INFINITY;
    y2 = NEGATIVE_INFINITY;

    for (auto & node : nodes) {
        Node& n = *node;
        if (predicate(&n)) {
            if (n.x-n.sx < x1)
                x1 = n.x-n.sx;
            if (n.y-n.sy < y1)
                y1 = n.y-n.sy;
            if (n.x+n.sx > x2)
                x2 = n.x+n.sx;
            if (n.y+n.sy > y2)
                y2 = n.y+n.sy;
        }
    }
}

double BasicSpringEmbedderLayout::relax()
{
    // TBD revise:
    //   - calculates in double (slow)
    //   - ignores connections to parent module
    //   - ignores node sizes altogether

    // Note: USE_CONTRACTING_BOX code was removed in version 4.1 -- if needed, it can be retrieved from earlier version

    NodeList::iterator i, j;
    EdgeList::iterator k;
    AnchorList::iterator l;

    // edge attraction: calculate if edges are longer or shorter than requested (tension),
    // and modify their (vx,vy) movement vector accordingly
    for (k = edges.begin(); k != edges.end(); ++k) {
        Edge& e = *k;
        if (e.src->fixed && e.dest->fixed)
            continue;
        if (e.src->anchor && e.src->anchor == e.dest->anchor)
            continue;
        double deltax = e.dest->x - e.src->x;
        double deltay = e.dest->y - e.src->y;
        double dist = sqrt(deltax * deltax + deltay * deltay);
        dist = (dist == 0) ? 1.0 : dist;
        double f = attractionForce * double(e.len - dist) / dist;
        double vx = f * deltax;
        double vy = f * deltay;

        e.dest->vx += vx;
        e.dest->vy += vy;
        e.src->vx -= vx;
        e.src->vy -= vy;
    }

    // nodes repulse each other, update (vx,vy) with this effect
    //
    // modification to the original algorithm: only nodes that share the
    // same color (i.e., are connected) repulse each other -- repulsion between
    // nodes of *different* colors ceases after a short distance. (This is done
    // to avoid "blow-up" of non-connected graphs.)
    //
    for (i = nodes.begin(); i != nodes.end(); ++i) {
        Node& n1 = *(*i);
        if (n1.fixed)
            continue;

        double fx = 0;
        double fy = 0;

        // TBD performance improvement: use (i=0..N, j=i+1..N) loop unless more than N/2 nodes are fixed
        for (j = nodes.begin(); j != nodes.end(); ++j) {
            if (i == j)
                continue;

            Node& n2 = *(*j);
            if (n1.anchor && n1.anchor == n2.anchor)
                continue;

            double deltax = n1.x - n2.x;
            double deltay = n1.y - n2.y;
            double distsq = deltax * deltax + deltay * deltay;

            // different colors repulse only up to 100 units, so that unconnected networks do not blow up
            if (n1.color == n2.color || distsq < 100*100) {
                if (distsq < 1.0) {
                    // use 1.0 instead of distsq, to avoid division by (near) zero;
                    // plus add random noise to help nodes mode aways from each other
                    fx += deltax + privRand01()-0.5;
                    fy += deltay + privRand01()-0.5;
                }
                else {
                    fx += deltax / distsq;
                    fy += deltay / distsq;
                }
            }
        }

        n1.vx += repulsiveForce * fx;
        n1.vy += repulsiveForce * fy;
    }

    if (debug) {
        for (int i = 0; i < (int)nodes.size(); ++i) {
            Node& n = *nodes[i];
            printf("%d:  pos=(%g,%g) v=(%g,%g)\n", i, n.x, n.y, n.vx, n.vy);
        }
        printf("\n");
    }

    bool bgsizeGiven = (width != 0 || height != 0);

    // limit vx,vy into (-50,50); move nodes by (vx,vy);
    // constrain nodes into rectangle (minx, miny, maxx, maxy)
    double maxd = 0;
    for (i = nodes.begin(); i != nodes.end(); ++i) {
        Node& n = *(*i);
        if (n.fixed || n.anchor) {
            // fixed nodes don't need to be moved, and anchored nodes are
            // handled separately (see below)
        }
        else {  // movable
            n.x += std::max(-50.0, std::min(50.0, n.vx));  // speed limit (actually, movement limit because (vx,vy) is not changed)
            n.y += std::max(-50.0, std::min(50.0, n.vy));

/*
            // move node within bounds. Additional random term is supposed to help prevent
            // many nodes lining up against the wall, by providing a force component
            // that is perpendicular to the wall; but it seems ineffectual.
            if (n.x < minx)
                n.x = minx + privRand01();
            if (n.x > maxx)
                n.x = maxx - privRand01();
            if (n.y < miny)
                n.y = miny + privRand01();
            if (n.y > maxy)
                n.y = maxy - privRand01();
*/
            // move node within bounds
            // Note: minx2, miny2 exist because when no bg size is given, we want to allow
            // nodes NOT connected to any fixed node to slip out at the top and left,
            // in order to achieve a better layout. We'd shift them back eventually.
            bool letThemOutTopLeft = !bgsizeGiven && !n.connectedToFixed;
            double minx2 = letThemOutTopLeft ? -100000000 : minx;
            double miny2 = letThemOutTopLeft ? -100000000 : miny;
            n.x = std::max(minx2, std::min(maxx, n.x));
            n.y = std::max(miny2, std::min(maxy, n.y));
        }

        // this is used for stopping condition
        if (maxd < n.vx)
            maxd = n.vx;
        if (maxd < n.vy)
            maxd = n.vy;

        // "friction" or "drag" -- nodes stop eventually if not driven by a force
        n.vx /= 2;
        n.vy /= 2;
    }

    // sum up movements of anchor nodes
    for (l = anchors.begin(); l != anchors.end(); ++l) {
        Anchor& a = *(*l);
        a.vx = 0;
        a.vy = 0;
    }
    for (i = nodes.begin(); i != nodes.end(); ++i) {
        Node& n = *(*i);
        if (n.anchor) {
            n.anchor->vx += n.vx / n.anchor->refcount;
            n.anchor->vy += n.vy / n.anchor->refcount;
        }
    }

    // move anchor points
    for (l = anchors.begin(); l != anchors.end(); ++l) {
        Anchor& a = *(*l);
        a.x += std::max(-50.0, std::min(50.0, a.vx));  // speed limit (actually, movement limit because vx,vy is not changed)
        a.y += std::max(-50.0, std::min(50.0, a.vy));

        // move nodes back within bounds; check right/bottom first, so if room is too small, top/left gets aligned
        if (a.x+a.x2off > maxx)
            a.x = maxx - a.x2off;
        if (a.y+a.y2off > maxy)
            a.y = maxy - a.y2off;
        if (a.x+a.x1off < minx)
            a.x = minx - a.x1off;
        if (a.y+a.y1off < miny)
            a.y = miny - a.y1off;

        // this is used for stopping condition
        if (maxd < a.vx)
            maxd = a.vx;
        if (maxd < a.vy)
            maxd = a.vy;

        // "friction" or "drag" -- nodes stop eventually if not driven by a force
        a.vx /= 2;
        a.vy /= 2;
    }

    // refresh positions of anchored nodes
    for (i = nodes.begin(); i != nodes.end(); ++i) {
        Node& n = *(*i);
        if (n.anchor) {
            n.x = n.anchor->x + n.offx;
            n.y = n.anchor->y + n.offy;

            n.vx = n.anchor->vx;
            n.vy = n.anchor->vy;
        }
    }

    return maxd;
}

void BasicSpringEmbedderLayout::debugDraw(int step)
{
    if (step % 5 != 0)
        return;

    environment->clearGraphics();

    const char *colors[] = {
        "black", "red", "blue", "green", "yellow", "cyan", "purple", "darkgreen"
    };
    for (auto & node : nodes) {
        Node& n = *node;
        const char *color = colors[n.color % (sizeof(colors)/sizeof(char *))];
        environment->drawRectangle(n.x-n.sx, n.y-n.sy, n.x+n.sx, n.y+n.sy, "{node bbox}", color);
    }

    for (auto & e : edges) {
        const char *color = colors[e.src->color % (sizeof(colors)/sizeof(char *))];
        environment->drawLine(e.src->x, e.src->y, e.dest->x, e.dest->y, "{edge bbox}", color);
    }

    char buf[80];
    snprintf(buf, sizeof(buf), "after step %d", step);
    environment->showGraphics(buf);
}

}  // namespace layout
}  // namespace omnetpp

