//==========================================================================
//  GRAPHLAYOUT.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <time.h>
#include <math.h>
#include <float.h>
#include <stdlib.h>
#include <sstream>

#include <deque>

#include "commonutil.h"
#include "graphlayouter.h"
#include "startreeembedding.h"
#include "heapembedding.h"
#include "platmisc.h"

GraphLayouter::GraphLayouter()
{
    width = height = border = 0;
    sizingMode = Free;
}

void GraphLayouter::setConfineToArea(int w, int h, int bd)
{
    width = w; height = h; border = bd;
    sizingMode = Confine;
}

void GraphLayouter::setScaleToArea(int w, int h, int bd)
{
    width = w; height = h; border = bd;
    sizingMode = Scale;
}

//----

BasicSpringEmbedderLayout::BasicSpringEmbedderLayout()
{
    haveFixedNode = false;
    haveAnchoredNode = false;
    allNodesAreFixed = true; // unless later it proves otherwise

    defaultEdgeLen = 40;
    maxIterations = 500;
    repulsiveForce = 50;
    attractionForce = 0.3;
}

BasicSpringEmbedderLayout::~BasicSpringEmbedderLayout()
{
    for (AnchorList::iterator i = anchors.begin(); i!=anchors.end(); ++i)
        delete (*i);
    for (NodeList::iterator j = nodes.begin(); j!=nodes.end(); ++j)
        delete (*j);
}

void BasicSpringEmbedderLayout::setEnvironment(GraphLayouterEnvironment *environment)
{
    GraphLayouter::setEnvironment(environment);

    repulsiveForce = environment->getDoubleParameter("bgl", 0, repulsiveForce);
    attractionForce = environment->getDoubleParameter("bgl", 1, attractionForce);
    defaultEdgeLen = environment->getDoubleParameter("bgl", 2, defaultEdgeLen);
    maxIterations = environment->getLongParameter("bgl", 3 , maxIterations);

#ifdef USE_CONTRACTING_BOX
    boxContractionForce = environment->getDoubleParameter("bpars", 0, 100);
    boxRepulsiveForce = environment->getDoubleParameter("bpars", 1, 100);
    boxRepForceRatio = environment->getDoubleParameter("bpars", 2, 1);
#endif
}

BasicSpringEmbedderLayout::Node *BasicSpringEmbedderLayout::findNode(cModule *mod)
{
    for (NodeList::iterator i=nodes.begin(); i!=nodes.end(); ++i)
        if ((*i)->key == mod)
            return (*i);
    return NULL;
}

void BasicSpringEmbedderLayout::addMovableNode(cModule *mod, int width, int height)
{
    Assert(findNode(mod)==NULL);

    allNodesAreFixed = false;

    Node *n = new Node();
    n->key = mod;
    n->fixed = false;
    n->anchor = NULL;
    n->sx = width/2;
    n->sy = height/2;

    nodes.push_back(n);
}

void BasicSpringEmbedderLayout::addFixedNode(cModule *mod, int x, int y, int width, int height)
{
    Assert(findNode(mod)==NULL);

    haveFixedNode = true;

    Node *n = new Node();
    n->key = mod;
    n->fixed = true;
    n->anchor = NULL;
    n->x = x;
    n->y = y;
    n->sx = width/2;
    n->sy = height/2;

    nodes.push_back(n);
}


void BasicSpringEmbedderLayout::addAnchoredNode(cModule *mod, const char *anchorname, int offx, int offy, int width, int height)
{
    Assert(findNode(mod)==NULL);

    haveAnchoredNode = true;
    allNodesAreFixed = false;

    Node *n = new Node();
    n->key = mod;

    Anchor *anchor;
    AnchorList::iterator a;
    for (a=anchors.begin(); a!=anchors.end(); ++a)
        if ((*a)->name == anchorname)
            break;
    if (a==anchors.end())
    {
        anchors.push_back(anchor = new Anchor());
        anchor->name = std::string(anchorname);
        anchor->refcount = 0;
    }
    else
    {
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
}

void BasicSpringEmbedderLayout::addEdge(cModule *from, cModule *to, int len)
{
    Assert(findNode(from)!=NULL && findNode(to)!=NULL);

    Edge e;
    e.from = findNode(from);
    e.to = findNode(to);
    e.len = len>0 ? len : defaultEdgeLen;

    // heuristics to take submodule size into account
    e.len += 2*(std::min(e.from->sx,e.from->sy)+std::min(e.to->sx,e.to->sy));

    edges.push_back(e);
}

void BasicSpringEmbedderLayout::addEdgeToBorder(cModule *, int)
{
    // this layouter algorithm ignores connections to border
}

void BasicSpringEmbedderLayout::getNodePosition(cModule *mod, int& x, int& y)
{
    Assert(findNode(mod)!=NULL);

    Node *n = findNode(mod);
    x = (int) n->x;
    y = (int) n->y;
}

void BasicSpringEmbedderLayout::execute()
{
    if (nodes.empty() || allNodesAreFixed)
        return;

    // initialize variables (also randomize start positions)
    for (AnchorList::iterator l=anchors.begin(); l!=anchors.end(); ++l)
    {
        Anchor& a = *(*l);
        a.x = 100 * privRand01();
        a.y = 100 * privRand01();
        a.dx = a.dy = 0;
    }
    for (NodeList::iterator k=nodes.begin(); k!=nodes.end(); ++k)
    {
        Node& n = *(*k);
        if (n.fixed)
        {
            // nop
        }
        else if (n.anchor)
        {
            n.x = n.anchor->x + n.offx;
            n.y = n.anchor->y + n.offy;
        }
        else // movable
        {
            n.x = 100 * privRand01();
            n.y = 100 * privRand01();
        }
        n.dx = n.dy = 0;
    }

#ifdef USE_CONTRACTING_BOX
    // initial box (slightly bigger than bounding box of nodes):
    box.x1 = -10;
    box.y1 = -10;
    box.x2 = 110;
    box.y2 = 110;
    box.dx1 = box.dy1 = box.dx2 = box.dy2 = 0;
#endif

    // set area
    if (sizingMode==Confine)
    {
        minx = border;
        miny = border;
        maxx = 2*minx + width;
        maxy = 2*miny + height;
    }
    else
    {
        minx = -100000000;
        miny = -100000000;
        maxx =  100000000;
        maxy =  100000000;
    }

    // partition graph
    doColoring();

    // now the real job -- stop if max moved distance is <0.05 at least 20 times in a row
    //clock_t beg = clock();
    int i, maxdcounter=0;
    for (i=1; i<maxIterations && maxdcounter<20; i++)
    {
        double maxd = relax();

        if (environment->inspected())
            debugDraw(i);

        if (maxd<0.05)
            maxdcounter++;
        else
            maxdcounter=0;
    }
    //clock_t end = clock();
    //printf("DBG: layout done in %g secs, %d iterations (%g sec/iter)\n",
    //       (end-beg)/(double)CLOCKS_PER_SEC, i, (end-beg)/(double)CLOCKS_PER_SEC/i);

    // scale back if too big -- BUT only if we don't have any fixed (or anchored) nodes,
    // because we don't want to change explicitly given coordinates (or distances
    // between anchored nodes)
    if (sizingMode==Scale && !haveFixedNode)
    {
        // calculate bounding box
        double x1, y1, x2, y2;
        Node& n = *(*nodes.begin());
        x1 = x2 = n.x;
        y1 = y2 = n.y;
        for (NodeList::iterator i=nodes.begin(); i!=nodes.end(); ++i)
        {
            Node& n = *(*i);
            if (n.x-n.sx < x1) x1 = n.x-n.sx;
            if (n.y-n.sy < y1) y1 = n.y-n.sy;
            if (n.x+n.sx > x2) x2 = n.x+n.sx;
            if (n.y+n.sy > y2) y2 = n.y+n.sy;
        }

        double bx = border, by = border;
        if (!haveAnchoredNode)
        {
            // rescale
            double xfact = (width-2*border) / (x2-x1);
            double yfact = (height-2*border) / (y2-y1);
            if (xfact>1) {xfact=1;} // only scale down if needed, but never magnify
            if (yfact>1) {yfact=1;}
            for (NodeList::iterator j=nodes.begin(); j!=nodes.end(); ++j)
            {
                Node& n = *(*j);
                n.x = bx + (n.x-x1)*xfact;
                n.y = by + (n.y-y1)*yfact;
            }
        }
        else
        {
            // don't want to rescale with anchored nodes, just shift bounding box to (bx,by)
            for (NodeList::iterator j=nodes.begin(); j!=nodes.end(); ++j)
            {
                Node& n = *(*j);
                n.x = bx + n.x - x1;
                n.y = by + n.y - y1;
            }
        }
    }
}

void BasicSpringEmbedderLayout::doColoring()
{
    NodeList::iterator i;
    for (i=nodes.begin(); i!=nodes.end(); ++i)
        (*i)->color = -1;

    int currentColor = 0;
    std::deque<Node*> todoList;
    for (i=nodes.begin(); i!=nodes.end(); ++i)
    {
        Node *n = *i;
        if (n->color!=-1) continue;  // already assigned

        // breadth-width search to color all connected nodes (transitive closure)
        Assert(todoList.size()==0);
        todoList.push_back(n); // start at this node
        while (!todoList.empty())
        {
            Node *n = todoList.back();
            todoList.pop_back();

            n->color = currentColor;

            // color and add to list all nodes connected to n.
            // (edge list data structure is not really good for this, but execution
            // time is still negligable compared to relax() iterations)
            EdgeList::iterator k;
            for (k=edges.begin(); k!=edges.end(); ++k)
            {
                Edge& e = *k;
                if (e.from==n && e.to->color==-1)
                    todoList.push_back(e.to);
                else if (e.to==n && e.from->color==-1)
                    todoList.push_back(e.from);
            }
        }

        // next color
        currentColor++;
    }
}

double BasicSpringEmbedderLayout::relax()
{
    // TBD revise:
    //   - calculates in double (slow)
    //   - ignores connections to parent module
    //   - ignores node sizes altogether

    NodeList::iterator i,j;
    EdgeList::iterator k;
    AnchorList::iterator l;

    double noiseLevel = 0; // noise doesn't seem to help

    // edge attraction: calculate if edges are longer or shorter than requested (tension),
    // and modify their (dx,dy) movement vector accordingly
    for (k=edges.begin(); k!=edges.end(); ++k)
    {
        Edge& e = *k;
        if (e.from->fixed && e.to->fixed)
            continue;
        double vx = e.to->x - e.from->x;
        double vy = e.to->y - e.from->y;
        double len = sqrt(vx * vx + vy * vy);
        len = (len == 0) ? 1.0 : len;
        double f = attractionForce * double(e.len - len) / len;
        double dx = f * vx;
        double dy = f * vy;

        e.to->dx += dx;
        e.to->dy += dy;
        e.from->dx += -dx;
        e.from->dy += -dy;
    }

    // nodes repulse each other, update (dx,dy) with this effect
    //
    // modification to the original algorithm: only nodes that share the
    // same color (i.e., are connected) repulse each other -- repulsion between
    // nodes of *different* colors ceases after a short distance. (This is done
    // to avoid "blow-up" of non-connected graphs.)
    //
    for (i=nodes.begin(); i!=nodes.end(); ++i)
    {
        Node& n1 = *(*i);
        if (n1.fixed)
            continue;

        double fx = 0;
        double fy = 0;

        // TBD performance improvement: use (i=0..N, j=i+1..N) loop unless more than N/2 nodes are fixed
        for (j=nodes.begin(); j!=nodes.end(); ++j)
        {
            if (i == j)
                continue;

            Node& n2 = *(*j);
            double vx = n1.x - n2.x;
            double vy = n1.y - n2.y;
            double lensq = vx * vx + vy * vy;
            if (n1.color==n2.color)
            {
                // most frequently firing condition first
                if (lensq > 2000*2000) // don't repulse if very far
                {
                }
                else if (lensq <= 1.0)
                {
                    fx += privRand01();
                    fy += privRand01();
                }
                else
                {
                    fx += vx / lensq;
                    fy += vy / lensq;
                }
            }
            else // different colors
            {
                // most frequently firing condition first
                if (lensq > 100*100)  // don't repulse if farther than 100
                {
                }
                else if (lensq <= 1.0)
                {
                    fx += privRand01();
                    fy += privRand01();
                }
                else
                {
                    fx += vx / lensq;
                    fy += vy / lensq;
                }
            }
        }

        // we only  use the direction of (dx,dy) -- node dx,dy is (force * unit vector)
        //double flensq = fx * fx + fy * fy;
        //if (flensq > 0)
        //{
            //double flen = sqrt(flensq);
            //n1.dx += repulsiveForce * fx / flen;
            //n1.dy += repulsiveForce * fy / flen;
            n1.dx += repulsiveForce * fx;
            n1.dy += repulsiveForce * fy;
        //}
    }

#ifdef USE_CONTRACTING_BOX
    // box contraction
    box.dx1 += boxContractionForce;
    box.dy1 += boxContractionForce;
    box.dx2 -= boxContractionForce;
    box.dy2 -= boxContractionForce;

    // repulsion between box and nodes
    for (i=nodes.begin(); i!=nodes.end(); ++i)
    {
        Node& n = *(*i);

        /*
        double fx1 = boxRepulsiveForce / sqrt(fabs(n.x - box.x1));
        double fx2 = -boxRepulsiveForce / sqrt(fabs(n.x - box.x2));
        double fy1 = boxRepulsiveForce / sqrt(fabs(n.y - box.y1));
        double fy2 = -boxRepulsiveForce / sqrt(fabs(n.y - box.y2));
        */
        double fx1 = boxRepulsiveForce / (n.x - box.x1);  // div by zero?
        double fx2 = boxRepulsiveForce / (n.x - box.x2);
        double fy1 = boxRepulsiveForce / (n.y - box.y1);
        double fy2 = boxRepulsiveForce / (n.y - box.y2);

        n.dx += fx1;
        box.dx1 -= fx1;

        n.dx += fx2;
        box.dx2 -= fx2;

        n.dy += fy1;
        box.dy1 -= fy1;

        n.dy += fy2;
        box.dy2 -= fy2;
    }

    box.dx1 /= boxRepForceRatio;
    box.dy1 /= boxRepForceRatio;
    box.dx2 /= boxRepForceRatio;
    box.dy2 /= boxRepForceRatio;
#endif

    // limit dx,dy into (-50,50); move nodes by (dx,dy);
    // constrain nodes into rectangle (minx, miny, maxx, maxy)
    double maxd = 0;
    for (i=nodes.begin(); i!=nodes.end(); ++i)
    {
        Node& n = *(*i);
        if (n.fixed || n.anchor)
        {
            // fixed nodes don't need to be moved, and anchored nodes are
            // handled separately (see below)
        }
        else // movable
        {
            n.x += std::max(-50.0, std::min(50.0, n.dx)); // speed limit
            n.y += std::max(-50.0, std::min(50.0, n.dy));

            n.x += noiseLevel*(privRand01()-0.5); // add noise to push nodes out of local minimums
            n.y += noiseLevel*(privRand01()-0.5);

            n.x = std::max(minx, std::min(maxx, n.x));
            n.y = std::max(miny, std::min(maxy, n.y));
        }

        // this is used for stopping condition
        if (maxd<n.dx) maxd=n.dx;
        if (maxd<n.dy) maxd=n.dy;

        // "friction" -- nodes stop eventually if not driven by a force
        n.dx /= 2;
        n.dy /= 2;
    }

    // sum up movements of anchor nodes
    for (l = anchors.begin(); l!=anchors.end(); ++l)
    {
        Anchor& a = *(*l);
        a.dx = 0;
        a.dy = 0;
    }
    for (i=nodes.begin(); i!=nodes.end(); ++i)
    {
        Node& n = *(*i);
        if (n.anchor)
        {
            n.anchor->dx += n.dx;
            n.anchor->dy += n.dy;
        }
    }
    // move anchor points
    for (l = anchors.begin(); l!=anchors.end(); ++l)
    {
        Anchor& a = *(*l);
        // double c = sqrt(n.anchor->refcount);
        a.x += std::max(-50.0, std::min(50.0, a.dx)); // speed limit
        a.y += std::max(-50.0, std::min(50.0, a.dy));

        a.x += noiseLevel*(privRand01()-0.5); // add noise to push nodes out of local minimums
        a.y += noiseLevel*(privRand01()-0.5);

        a.x = std::max(minx, std::min(maxx, a.y));
        a.y = std::max(miny, std::min(maxy, a.x));

        // this is used for stopping condition
        if (maxd<a.dx) maxd=a.dx;
        if (maxd<a.dy) maxd=a.dy;

        // "friction" -- nodes stop eventually if not driven by a force
        a.dx /= 2;
        a.dy /= 2;
    }
    // refresh positions of anchored nodes (and distribute anchor's dx,dy among its nodes)
    for (i=nodes.begin(); i!=nodes.end(); ++i)
    {
        Node& n = *(*i);
        if (n.anchor)
        {
            n.x = n.anchor->x + n.offx;
            n.y = n.anchor->y + n.offy;

            n.dx = n.anchor->dx / n.anchor->refcount;
            n.dy = n.anchor->dy / n.anchor->refcount;
        }
    }

#ifdef USE_CONTRACTING_BOX
    // move box by its dx1,dy1,dx2,dy2
    box.x1 += box.dx1;
    box.y1 += box.dy1;
    box.x2 += box.dx2;
    box.y2 += box.dy2;

    box.dx1 /= 2;
    box.dy1 /= 2;
    box.dx2 /= 2;
    box.dy2 /= 2;

    // calculate bounding rectange and adjust box to be bigger than that
    double x1, y1, x2, y2;
    Node& n = *(*nodes.begin());
    x1 = x2 = n.x;
    y1 = y2 = n.y;
    for (i=nodes.begin(); i!=nodes.end(); ++i)
    {
        Node& n = *(*i);
        if (n.x-n.sx < x1) x1 = n.x-n.sx;
        if (n.y-n.sy < y1) y1 = n.y-n.sy;
        if (n.x+n.sx > x2) x2 = n.x+n.sx;
        if (n.y+n.sy > y2) y2 = n.y+n.sy;
    }
    if (box.x1 >= x1) box.x1 = x1;
    if (box.y1 >= y1) box.y1 = y1;
    if (box.x2 <= x2) box.x2 = x2;
    if (box.y2 <= y2) box.y2 = y2;
#endif
    return maxd;
}

void BasicSpringEmbedderLayout::debugDraw(int step)
{
    if (step % 5 != 0) return;

    environment->clearGraphics();

#ifdef USE_CONTRACTING_BOX
    environment->drawRectangle(box.x1, box.y1, box.x2, box.y2, "{box bbox}", "black");
#endif

    const char *colors[] = {"black","red","blue","green","yellow","cyan","purple","darkgreen"};
    for (NodeList::iterator i=nodes.begin(); i!=nodes.end(); ++i)
    {
        Node& n = *(*i);
        const char *color = colors[n.color % (sizeof(colors)/sizeof(char*))];
        environment->drawRectangle(n.x-n.sx, n.y-n.sy, n.x+n.sx, n.y+n.sy, "{node bbox}", color);
    }

    for (EdgeList::iterator j=edges.begin(); j!=edges.end(); ++j)
    {
        Edge& e = *j;
        const char *color = colors[e.from->color % (sizeof(colors)/sizeof(char*))];
        environment->drawLine(e.from->x, e.from->y, e.to->x, e.to->y, "{edge bbox}", color);
    }

    char buf[80];
    sprintf(buf,"after step %d",step);
    environment->showGraphics(buf);
}

/*******************************************************/

ForceDirectedGraphLayouter::ForceDirectedGraphLayouter()
{
    bordersAdded = false;
    hasFixedNode = false;
    hasMovableNode = false;

    topBorder = NULL;
    bottomBorder = NULL;
    leftBorder = NULL;
    rightBorder = NULL;
}

void ForceDirectedGraphLayouter::setParameters()
{
    embedding.parameters = embedding.getParameters(lcgRandom.getSeed());
    embedding.parameters.defaultPointLikeDistance = environment->getBoolParameter("pld", 0, pointLikeDistance);
    embedding.parameters.defaultSpringCoefficient = environment->getDoubleParameter("sc", 0, embedding.parameters.defaultSpringCoefficient);
    embedding.parameters.defaultSpringReposeLength = environment->getDoubleParameter("srl", 0, expectedEdgeLength);
    embedding.parameters.electricRepulsionCoefficient = environment->getDoubleParameter("erc", 0, embedding.parameters.electricRepulsionCoefficient);
    embedding.parameters.frictionCoefficient = environment->getDoubleParameter("fc", 0, embedding.parameters.frictionCoefficient);
    embedding.parameters.minTimeStep = environment->getDoubleParameter("mits", 0, embedding.parameters.minTimeStep);
    embedding.parameters.maxTimeStep = environment->getDoubleParameter("mats", 0, embedding.parameters.maxTimeStep);
    embedding.parameters.minAccelerationError = environment->getDoubleParameter("miae", 0, embedding.parameters.minAccelerationError);
    embedding.parameters.maxAccelerationError = environment->getDoubleParameter("maae", 0, embedding.parameters.maxAccelerationError);
    embedding.parameters.velocityRelaxLimit = environment->getDoubleParameter("vrl", 0, embedding.parameters.velocityRelaxLimit);
    embedding.parameters.maxCalculationTime = environment->getDoubleParameter("mct", 0, embedding.parameters.maxCalculationTime);

    threeDFactor = environment->getDoubleParameter("3d", 0, std::max(0.0, privRand01() - 0.5)); // default: 0.1

    preEmbedding = environment->getBoolParameter("pe", 0, privRand01() > 0.5); // default: true
    forceDirectedEmbedding = environment->getBoolParameter("fde", 0, true);

    // debug parameters
    debugWaitTime = environment->getDoubleParameter("dwt", 0, 0);
    showForces = environment->getBoolParameter("sf", 0, false);
    showSummaForce = environment->getBoolParameter("ssf", 0, false);
}

void ForceDirectedGraphLayouter::addBody(cModule *mod, IBody *body)
{
    embedding.addBody(body);
    moduleToBodyMap[mod] = body;
}

IBody *ForceDirectedGraphLayouter::findBody(cModule *mod)
{
    std::map<cModule *, IBody *>::iterator it = moduleToBodyMap.find(mod);

    if (it != moduleToBodyMap.end())
        return it->second;
    else
        return NULL;
}

Variable *ForceDirectedGraphLayouter::ensureAnchorVariable(const char *anchorName)
{
    std::map<std::string, Variable *>::iterator it = anchorNameToVariableMap.find(anchorName);

    if (it != anchorNameToVariableMap.end())
        return it->second;
    else
        return anchorNameToVariableMap[anchorName] = new Variable(Pt::getNil());
}

void ForceDirectedGraphLayouter::calculateExpectedMeasures()
{
    const std::vector<IBody *>& bodies = embedding.getBodies();

    // body sizes
    int count = 0;
    double maxBodyLength = 0;

    // expected measures
    expectedEdgeLength = 0;
    expectedEmbeddingSize = pow(expectedEdgeLength, 2) * bodies.size();

    for (int i = 0; i < (int)bodies.size(); i++) {
        IBody *body = bodies[i];

        if (!dynamic_cast<WallBody *>(body)) {
            double length = body->getSize().getDiagonalLength();
            count++;
            maxBodyLength = std::max(maxBodyLength, length);
            expectedEdgeLength += length;
            expectedEmbeddingSize += 10 * body->getSize().getArea();
        }
    }

    // pointLikeDistance if there is a body longer than 2 times the average size
    Assert(!isNaN(expectedEdgeLength));
    double averageBodyLength = expectedEdgeLength / count;
    pointLikeDistance = maxBodyLength < 2 * averageBodyLength;

    // minimum length plus averageBodyLength
    expectedEdgeLength = 50 + averageBodyLength;

    // calculate expected embedding size
    expectedEmbeddingSize = sqrt(expectedEmbeddingSize);
    Assert(!isNaN(expectedEmbeddingSize));
}

void ForceDirectedGraphLayouter::setRandomPositions()
{
    // assign values to not yet assigned coordinates
    const std::vector<Variable *>& variables = embedding.getVariables();
    for (int i = 0; i < (int)variables.size(); i++) {
        Pt pt(variables[i]->getPosition());

        if (isNaN(pt.x))
            pt.x = privRand01() * expectedEmbeddingSize;

        if (isNaN(pt.y))
            pt.y = privRand01() * expectedEmbeddingSize;

        if (isNaN(pt.z))
            pt.z = (privRand01() - 0.5) * expectedEmbeddingSize * threeDFactor;

        variables[i]->assignPosition(pt);
    }
}

void ForceDirectedGraphLayouter::setInitialPositions() {
    GraphComponent childrenComponentsStar;
    Vertex *childrenComponentsStarRoot = NULL;

    // first pre embed all connected components, one by one
    for (std::vector<GraphComponent *>::iterator it = graphComponent.connectedSubComponents.begin(); it != graphComponent.connectedSubComponents.end(); it++) {
        GraphComponent *childComponent = *it;
        childComponent->calculateSpanningTree();

        // use tree embedding if connected component is a tree
        if (childComponent->getEdgeCount() == childComponent->getVertexCount() - 1) {
            StarTreeEmbedding starTreeEmbedding(childComponent, expectedEdgeLength);
            starTreeEmbedding.embed();
        }
        else {
            HeapEmbedding heapEmbedding(childComponent, expectedEdgeLength);
            heapEmbedding.embed();
        }

        // add a new vertex to component graph for each embedded sub component
        Vertex *childComponentVertex = new Vertex(Pt::getNil(), childComponent->getBoundingRectangle().rs, NULL);
        if (!childrenComponentsStarRoot)
            childrenComponentsStarRoot = childComponentVertex;

        childrenComponentsStar.addVertex(childComponentVertex);
        childrenComponentsStar.addEdge(new Edge(childrenComponentsStarRoot, childComponentVertex));
    }

    // embed component graph
    childrenComponentsStar.calculateSpanningTree();
    HeapEmbedding heapEmbedding(&childrenComponentsStar, expectedEdgeLength);
    heapEmbedding.embed();

    // position all vertices on the plane so that they do not overlap
    // use both connected component pre embedding coordinates and component graph coordinates
    for (int i = 0; i < graphComponent.connectedSubComponents.size(); i++) {
        Vertex *childComponentVertex = childrenComponentsStar.getVertex(i);
        GraphComponent *childComponent = graphComponent.connectedSubComponents[i];

        for (int i = 0; i < childComponent->getVertexCount(); i++) {
            Vertex *vertex = childComponent->getVertex(i);
            Variable *variable = (Variable *)vertex->identity;
            Pt pt = vertex->rc.getCenterCenter();
            pt.add(childComponentVertex->rc.pt);
            pt.z = NaN;
            variable->assignPosition(pt);
        }
    }
}

void ForceDirectedGraphLayouter::ensureBorders()
{
    if (!bordersAdded) {
        double coefficient = 0.1;

        topBorder = new WallBody(true, NaN);
        bottomBorder = new WallBody(true, NaN);
        leftBorder = new WallBody(false, NaN);
        rightBorder = new WallBody(false, NaN);

        // add electric repulsion to all bodies
        const std::vector<IBody *>& bodies = embedding.getBodies();
        for (int i = 0; i < (int)bodies.size(); i++) {
            IBody *body = bodies[i];
            embedding.addForceProvider(new VerticalElectricRepulsion(topBorder, body));
            embedding.addForceProvider(new VerticalElectricRepulsion(bottomBorder, body));
            embedding.addForceProvider(new HorizontalElectricRepulsion(leftBorder, body));
            embedding.addForceProvider(new HorizontalElectricRepulsion(rightBorder, body));
        }

        // add springs between walls
        embedding.addBody(topBorder);
        embedding.addBody(bottomBorder);
        embedding.addBody(leftBorder);
        embedding.addBody(rightBorder);
        embedding.addForceProvider(new VerticalSpring(topBorder, bottomBorder, coefficient, 0));
        embedding.addForceProvider(new HorizonalSpring(leftBorder, rightBorder, coefficient, 0));

        bordersAdded = true;
    }
}

void ForceDirectedGraphLayouter::setBorderPositions()
{
    double distance = 100;
    double top = DBL_MAX, bottom = DBL_MIN;
    double left = DBL_MAX, right = DBL_MIN;

    const std::vector<IBody *>& bodies = embedding.getBodies();
    for (int i = 0; i < (int)bodies.size(); i++) {
        IBody *body = bodies[i];

        if (!dynamic_cast<WallBody *>(body)) {
            top = std::min(top, body->getTop());
            bottom = std::max(bottom, body->getBottom());
            left = std::min(left, body->getLeft());
            right = std::max(right, body->getRight());
        }
    }

    // ensure all bodies are within
    topBorder->setPosition(top - distance);
    bottomBorder->setPosition(bottom + distance);
    leftBorder->setPosition(left - distance);
    rightBorder->setPosition(right + distance);
}

void ForceDirectedGraphLayouter::addElectricRepulsions()
{
    const std::vector<IBody *>& bodies = embedding.getBodies();
    for (int i = 0; i < (int)bodies.size(); i++)
        for (int j = i + 1; j < (int)bodies.size(); j++) {
            IBody *body1 = bodies[i];
            IBody *body2 = bodies[j];
            Variable *variable1 = body1->getVariable();
            Variable *variable2 = body2->getVariable();

            // ignore wall bodies
            if (!dynamic_cast<WallBody *>(body1) &&
                !dynamic_cast<WallBody *>(body2) &&
                variable1 != variable2)
            {
                Vertex *vertex1 = graphComponent.findVertex(variable1);
                Vertex *vertex2 = graphComponent.findVertex(variable2);

                Assert(vertex1);
                Assert(vertex2);

                // check if the two are in the same connected component
                if (vertex1->connectedSubComponent != vertex2->connectedSubComponent)
                    embedding.addForceProvider(new ElectricRepulsion(body1, body2, expectedEdgeLength, expectedEdgeLength * 2));
                else
                    embedding.addForceProvider(new ElectricRepulsion(body1, body2));
            }
        }
}

void ForceDirectedGraphLayouter::addBasePlaneSprings()
{
    const std::vector<IBody *>& bodies = embedding.getBodies();
    for (int i = 0; i < (int)bodies.size(); i++) {
          IBody *body = bodies[i];

        if (!dynamic_cast<WallBody *>(body))
            embedding.addForceProvider(new BasePlaneSpring(body, 5, 0));
    }
}

void ForceDirectedGraphLayouter::scale()
{
    // find average spring length
    int springCount = 0;
    double averageSpringLength = 0;
    const std::vector<IForceProvider *>& forceProviders = embedding.getForceProviders();
    for (int i = 0; i < (int)forceProviders.size(); i++) {
        Spring *spring = dynamic_cast<Spring *>(forceProviders[i]);

        if (spring) {
            const Pt& pt1 = spring->getBody1()->getPosition();
            const Pt& pt2 = spring->getBody2()->getPosition();
            springCount++;
            averageSpringLength += pt1.getBasePlaneProjectionDistance(pt2);
        }
    }
    averageSpringLength /= springCount;
    Assert(!isNaN(averageSpringLength));

    // scale
    double scale = expectedEdgeLength / averageSpringLength;

    const std::vector<Variable *>& variables = embedding.getVariables();
    for (int i = 0; i < (int)variables.size(); i++) {
        variables[i]->assignPosition(Pt(variables[i]->getPosition()).multiply(scale));
    }
}

void ForceDirectedGraphLayouter::translate()
{
    double top = DBL_MAX;
    double left = DBL_MAX;

    const std::vector<IBody *>& bodies = embedding.getBodies();
    for (int i = 0; i < (int)bodies.size(); i++) {
        IBody *body = bodies[i];

        if (!dynamic_cast<WallBody *>(body)) {
            top = std::min(top, body->getTop());
            left = std::min(left, body->getLeft());
        }
    }

    const std::vector<Variable *>& variables = embedding.getVariables();
    for (int i = 0; i < (int)variables.size(); i++) {
        variables[i]->assignPosition(Pt(variables[i]->getPosition()).subtract(Pt(left - border, top - border, 0)));
    }
}

void ForceDirectedGraphLayouter::addMovableNode(cModule *mod, int width, int height)
{
    hasMovableNode = true;

    // this is a free variable and body
    Variable *variable = new Variable(Pt::getNil());
    addBody(mod, new Body(variable, Rs(width, height)));

    graphComponent.addVertex(new Vertex(Pt::getNil(), Rs(width, height), variable));
}

void ForceDirectedGraphLayouter::addFixedNode(cModule *mod, int x, int y, int width, int height)
{
    hasFixedNode = true;

    // a fix node is a constrained variable which can only move on the z axes
    Variable *variable = new PointConstrainedVariable(Pt(x + width / 2, y + height / 2, NaN));
    IBody *body = new Body(variable, Rs(width, height));
    addBody(mod, body);

    graphComponent.addVertex(new Vertex(Pt(x, y, NaN), Rs(width, height), variable));
}

void ForceDirectedGraphLayouter::addAnchoredNode(cModule *mod, const char *anchorname, int offx, int offy, int width, int height)
{
    // an anchored node is a relatively (to a variable, namely the anchor) positioned body
    Variable *variable = ensureAnchorVariable(anchorname);
    addBody(mod, new RelativelyPositionedBody(variable, Pt(offx, offy, 0), Rs(width, height)));

    // update vertex size
    Vertex *vertex = graphComponent.findVertex(variable);
    if (!vertex)
        graphComponent.addVertex(new Vertex(Pt::getNil(), Rs(offx + width, offy + height), variable));
    else {
        Rs rs = vertex->rc.rs;
        vertex->rc.rs = Rs(std::max(rs.width, (double)(offx + width)), std::max(rs.height, (double)(offy + height)));
    }
}

void ForceDirectedGraphLayouter::addEdge(cModule *from, cModule *to, int len)
{
    // an edge is a spring
    Spring *spring = new Spring(findBody(from), findBody(to), -1, !len ? -1 : len);
    embedding.addForceProvider(spring);

    // and also an edge in the graph
    graphComponent.addEdge(new Edge(
            graphComponent.findVertex(spring->getBody1()->getVariable()),
            graphComponent.findVertex(spring->getBody2()->getVariable())));
}

void ForceDirectedGraphLayouter::addEdgeToBorder(cModule *from, int len)
{
    ensureBorders();

    IBody *body = findBody(from);

    // add a force provider which uses for springs each connected to a wall
    // the least expanded spring will provided an attraction force
    std::vector<AbstractSpring *> springs;
    springs.push_back(new VerticalSpring(topBorder, body, -1, !len ? -1 : len));
    springs.push_back(new VerticalSpring(bottomBorder, body, -1, !len ? -1 : len));
    springs.push_back(new HorizonalSpring(leftBorder, body, -1, !len ? -1 : len));
    springs.push_back(new HorizonalSpring(rightBorder, body, -1, !len ? -1 : len));
    embedding.addForceProvider(new LeastExpandedSpring(springs));
}

void ForceDirectedGraphLayouter::execute()
{
    if (hasMovableNode) {
        calculateExpectedMeasures();
        setParameters();

        // always calculate graph components
        graphComponent.calculateConnectedSubComponents();

        // call pre embedding if requested
        if (preEmbedding) {
            setInitialPositions();

            // draw initial pre embedding result
            if (environment->inspected())
                debugDraw();
        }

        // call force directed embedding if requested
        if (forceDirectedEmbedding) {
            addElectricRepulsions();

            if (threeDFactor > 0)
                addBasePlaneSprings();

            embedding.addForceProvider(new Drag());

            // assign random values to missing positions 
            setRandomPositions();

            if (bordersAdded)
                setBorderPositions();

            //embedding.debug = 3;
            embedding.inspected = environment->inspected();
            embedding.reinitialize();

            if (embedding.inspected) {
                while (!embedding.getFinished()) {
                    embedding.embed();
                    debugDraw();
                }
            }
            else
                embedding.embed();
        }

        // set random positions if no embedding was used
        if (!preEmbedding && !forceDirectedEmbedding)
            setRandomPositions();

        if (!hasFixedNode) {
            scale();
            translate();
        }
    }
}

void ForceDirectedGraphLayouter::getNodePosition(cModule *mod, int& x, int& y)
{
    IBody *body = findBody(mod);
    const Pt& pt = body->getPosition();
    x = (int) pt.x;
    y = (int) pt.y;
}

void ForceDirectedGraphLayouter::debugDraw()
{
    environment->clearGraphics();

    // draw bodies
    const std::vector<IBody *>& bodies = embedding.getBodies();
    for (int i = 0; i < (int)bodies.size(); i++) {
        IBody *body = bodies[i];
        Pt pt(body->getPosition());
        Rs rs = body->getSize();
        pt.subtract(Pt(rs.width / 2, rs.height / 2, 0));

        // wall bodies are long lines
        if (dynamic_cast<WallBody *>(body)) {
            if (rs.width == POSITIVE_INFINITY) {
                pt.x = -1000;
                rs.width = 2000;
            }

            if (rs.height == POSITIVE_INFINITY) {
                pt.y = -1000;
                rs.height = 2000;
            }
        }

        environment->drawRectangle(pt.x, pt.y, pt.x + rs.width, pt.y + rs.height, "{node bbox}", "black");
        char text[100];
        sprintf(text,"%g", pt.z);
        environment->drawText(pt.x, pt.y, text, "{node bbox}", "black");
    }

    // draw springs
    const std::vector<IForceProvider *>& forceProviders = embedding.getForceProviders();
    for (int i = 0; i < (int)forceProviders.size(); i++) {
        Spring *spring = dynamic_cast<Spring *>(forceProviders[i]);

        if (spring) {
            const Pt& pt1 = spring->getBody1()->getPosition();
            const Pt& pt2 = spring->getBody2()->getPosition();
            environment->drawLine(pt1.x, pt1.y, pt2.x, pt2.y, "{edge bbox}", "black");
        }
    }

    // draw forces
    double forceScale = 10;
    const std::vector<Variable *>& variables = embedding.getVariables();
    for (int i = 0; i < (int)variables.size(); i++) {
        Variable *variable = variables[i];
        const Pt& pt1 = variable->getPosition();

        if (showForces) {
            std::vector<Pt> forces = variable->getForces();

            for (int j = 0; j < (int)forces.size(); j++) {
                Pt pt2(pt1);
                Pt force(forces[j]);
                force.multiply(forceScale);
                pt2.add(force);
                environment->drawLine(pt1.x, pt1.y, pt2.x, pt2.y, "force", "red");
            }
        }

        if (showSummaForce) {
            Pt pt2(pt1);
            Pt force(variable->getForce());
            force.multiply(forceScale);
            pt2.add(force);
            environment->drawLine(pt1.x, pt1.y, pt2.x, pt2.y, "force", "blue");
        }
    }

    // write force directed embedding internal state info
    std::stringstream info;
    embedding.writeDebugInformation(info);
    environment->showGraphics(info.str().c_str());

    usleep((int)(debugWaitTime*1000));
}
