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
#include <stdlib.h>
#include <assert.h>

#include <deque>

#include "graphlayout.h"

#define MIN(a,b) ((a)<(b) ? (a) : (b))
#define MAX(a,b) ((a)<(b) ? (b) : (a))


#define GLRAND_MAX  0x7ffffffeL  /* = 2**31-2 */

double GraphLayouter::privRand01()
{
    const long int a=16807, q=127773, r=2836;
    rndseed=a*(rndseed%q) - r*(rndseed/q);
    if (rndseed<=0) rndseed+=GLRAND_MAX+1;

    return rndseed/(double)(GLRAND_MAX+1);
}


GraphLayouter::GraphLayouter()
{
    rndseed = 1;
    defaultEdgeLen = 40;

    width = height = border = 0;
    sizingMode = Free;

    interp = NULL;
    canvas = NULL;
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

void GraphLayouter::setCanvas(Tcl_Interp *i, const char *c)
{
    interp = i;
    canvas = c;
}

//----

BasicSpringEmbedderLayout::BasicSpringEmbedderLayout()
{
    haveFixedNode = false;
    haveAnchoredNode = false;
    allNodesAreFixed = true; // unless later it proves otherwise

    maxIterations = 500;
    //repulsiveForce = 8;
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

BasicSpringEmbedderLayout::Node *BasicSpringEmbedderLayout::findNode(cModule *mod)
{
    for (NodeList::iterator i=nodes.begin(); i!=nodes.end(); ++i)
        if ((*i)->key == mod)
            return (*i);
    return NULL;
}

void BasicSpringEmbedderLayout::addMovableNode(cModule *mod, int width, int height)
{
    assert(findNode(mod)==NULL);

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
    assert(findNode(mod)==NULL);

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
    assert(findNode(mod)==NULL);

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
    assert(findNode(from)!=NULL && findNode(to)!=NULL);

    Edge e;
    e.from = findNode(from);
    e.to = findNode(to);
    e.len = len>0 ? len : defaultEdgeLen;

    // heuristics to take submodule size into account
    e.len += 2*(MIN(e.from->sx,e.from->sy)+MIN(e.to->sx,e.to->sy));

    edges.push_back(e);
}

void BasicSpringEmbedderLayout::addEdgeToBorder(cModule *, int)
{
    // this layouter algorithm ignores connections to border
}

void BasicSpringEmbedderLayout::getNodePosition(cModule *mod, int& x, int& y)
{
    assert(findNode(mod)!=NULL);

    Node *n = findNode(mod);
    x = (int) n->x;
    y = (int) n->y;
}

void BasicSpringEmbedderLayout::execute()
{
    if (nodes.empty() || allNodesAreFixed)
        return;

    // consume a some values (manually given seeds are usually small!)
    privRand01();
    privRand01();
    privRand01();

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

        debugDraw(i);

        if (maxd<0.05)
            maxdcounter++;
        else
            maxdcounter=0;
    }
    //clock_t end = clock();
    //printf("DBG: layout done in %g secs, %d iterations (%g sec/iter)\n",
    //       (end-beg)/(double)CLOCKS_PER_SEC, i, (end-beg)/(double)CLOCKS_PER_SEC/i);

    // clean up canvas after the drawing
    if (interp && canvas)
    {
        Tcl_VarEval(interp, canvas," delete all\n",
                            canvas," config -scrollregion {0 0 1 1}\n",
                            canvas," xview moveto 0\n",
                            canvas," yview moveto 0\n", NULL);
    }

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
        assert(todoList.size()==0);
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
        double flensq = fx * fx + fy * fy;
        if (flensq > 0)
        {
            //double flen = sqrt(flensq);
            //n1.dx += repulsiveForce * fx / flen;
            //n1.dy += repulsiveForce * fy / flen;
            n1.dx += repulsiveForce * fx;
            n1.dy += repulsiveForce * fy;
        }
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
            n.x += MAX(-50, MIN(50, n.dx)); // speed limit
            n.y += MAX(-50, MIN(50, n.dy));

            n.x += noiseLevel*(privRand01()-0.5); // add noise to push nodes out of local minimums
            n.y += noiseLevel*(privRand01()-0.5);

            n.x = MAX(minx, MIN(maxx, n.x));
            n.y = MAX(miny, MIN(maxy, n.y));
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
        a.x += MAX(-50, MIN(50, a.dx)); // speed limit
        a.y += MAX(-50, MIN(50, a.dy));

        a.x += noiseLevel*(privRand01()-0.5); // add noise to push nodes out of local minimums
        a.y += noiseLevel*(privRand01()-0.5);

        a.x = MAX(minx, MIN(maxx, a.y));
        a.y = MAX(miny, MIN(maxy, a.x));

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
    if (!interp || !canvas) return;
    if (step % 5 != 0) return;
    if (TCL_ERROR==Tcl_VarEval(interp, canvas, " delete all", NULL)) return;
    const char *colors[] = {"black","red","blue","green","yellow","cyan","purple","darkgreen"};
    char coords[100];
#ifdef USE_CONTRACTING_BOX
    sprintf(coords,"%lg %lg %lg %lg", box.x1, box.y1, box.x2, box.y2);
    Tcl_VarEval(interp, canvas, " create rect ", coords, " -outline black -tag box", NULL);
#endif
    for (NodeList::iterator i=nodes.begin(); i!=nodes.end(); ++i)
    {
        Node& n = *(*i);
        sprintf(coords,"%g %g %g %g", n.x-n.sx, n.y-n.sy, n.x+n.sx, n.y+n.sy);
        const char *color = colors[n.color % (sizeof(colors)/sizeof(char*))];
        Tcl_VarEval(interp, canvas, " create rect ",coords," -outline ",color," -tag node", NULL);
    }
    for (EdgeList::iterator j=edges.begin(); j!=edges.end(); ++j)
    {
        Edge& e = *j;
        sprintf(coords,"%g %g %g %g", e.from->x, e.from->y, e.to->x, e.to->y);
        const char *color = colors[e.from->color % (sizeof(colors)/sizeof(char*))];
        Tcl_VarEval(interp, canvas, " create line ",coords," -fill ",color,NULL);
    }
    Tcl_VarEval(interp, canvas, " raise node", NULL);

    char buf[80];
    sprintf(buf,"%d",step);
    Tcl_VarEval(interp, "layouter_debugDraw_finish ", canvas," {after step ",buf,"}", NULL);
}

//----------

void AdvSpringEmbedderLayout::getEdgeVector(const Node& from, const Node& to, double& vx, double& vy, double& len)
{
/* basic version that ignores size:
    vx = to.x - from.x;
    vy = to.y - from.y;
    len = sqrt(vx*vx + vy*vy);
    if (len==0) {
        vx = 1;
        vy = 0;
    } else {
        vx /= len;
        vy /= len;
    }
*/

    // if there's x overlap: vx=0, otherwise the distance of their borders
    int sumsx = to.sx + from.sx;
    if (to.x+sumsx < from.x)
        vx = to.x - from.x + sumsx;
    else if (from.x+sumsx < to.x)
        vx = to.x - from.x - sumsx;
    else
        vx = 0;

    // if there's y overlap: vy=0, otherwise the distance of their borders
    int sumsy = to.sy + from.sy;
    if (to.y+sumsy < from.y)
        vy = to.y - from.y + sumsy;
    else if (from.y+sumsy < to.y)
        vy = to.y - from.y - sumsy;
    else
        vy = 0;

    // create length and normalized (vx,vy) vector
    if (vx==0 && vy==0) {
        len = 0;
        const double sqrt2 = sqrt(2.0);
        vx = (to.x > from.x) ? sqrt2 : -sqrt2;
        vy = (to.y > from.y) ? sqrt2 : -sqrt2;
    } else if (vx==0) {
        len = vy;
        vy = vy<0 ? -1 : 1;
    } else if (vy==0) {
        len = vx;
        vx = vx<0 ? -1 : 1;
    } else {
        len = sqrt(vx*vx + vy*vy);
        vx /= len;
        vy /= len;
    }

}

void AdvSpringEmbedderLayout::getForceVector(const Node& from, const Node& to, double& vx, double& vy, double& len)
{
/* basic version that ignores size:
    vx = to.x - from.x;
    vy = to.y - from.y;
    len = sqrt(vx*vx + vy*vy);
    if (len==0) {
        vx = 1;
        vy = 0;
    } else {
        vx /= len;
        vy /= len;
    }
*/

    getEdgeVector(from, to, vx, vy, len);
}

double AdvSpringEmbedderLayout::relax()
{
    // This is an implementation of the SpringEmbedder layouting algorithm.
    //
    // TBD revise:
    //   - calculates in double (slow)
    //   - ignores connections to parent module
    //   - ignores node sizes altogether

    NodeList::iterator i,j;
    EdgeList::iterator k;

    // edge attraction: calculate if edges are longer or shorter than requested (tension),
    // and modify their (dx,dy) movement vectors accordingly
    for (k=edges.begin(); k!=edges.end(); ++k)
    {
        Edge& e = *k;
        if (e.from->fixed && e.to->fixed)
            continue;

        double vx, vy, len;
        getEdgeVector(*e.from, *e.to, vx, vy, len); // (vx,vy) is unit vector

        // calculate spring force f; (dx,dy) = f * (vx,vy)
        double f = attractionForce * (e.len - len);
        double dx = f * vx;
        double dy = f * vy;

        // update dx, dy of the nodes
        e.to->dx += dx;
        e.to->dy += dy;
        e.from->dx += -dx;
        e.from->dy += -dy;
    }

    // nodes repulse each other, update (dx,dy) with this effect
    for (i=nodes.begin(); i!=nodes.end(); ++i)
    {
        // update node i (unless it's fixed)
        Node& n1 = *(*i);
        if (n1.fixed)
            continue;

        double dx = 0;
        double dy = 0;

        for (j=nodes.begin(); j!=nodes.end(); ++j)
        {
            if (i == j)
                continue;

            Node& n2 = *(*j);

            double vx, vy, len;
            getForceVector(n2,n1, vx,vy,len); // (vx,vy) is unit vector

            // force weakens proportional to the inverse of the square of the distance
            if (len == 0)
            {
                dx += privRand01();
                dy += privRand01();
            }
            else if (len < 1000.0)
            {
                dx += vx / len;
                dy += vy / len;
            }
        }

        // node's (dx,dy): repulsiveforce * direction unit vector
        double dlensqr = dx*dx + dy*dy;
        if (dlensqr > 0)
        {
            double dlen = sqrt(dlensqr);
            n1.dx += repulsiveForce * dx / dlen;
            n1.dy += repulsiveForce * dy / dlen;
        }
    }

    // limit dx,dy into (-50,50); move nodes by (dx,dy);
    // constrain nodes into rectangle (minx, miny, maxx, maxy)
    for (i=nodes.begin(); i!=nodes.end(); ++i)
    {
        Node& n = *(*i);

        if (n.fixed)
        {
            // nop
        }
        else if (n.anchor)
        {
            // move anchor point
            double& anchorx = n.anchor->x;
            double& anchory = n.anchor->y;

            anchorx += MAX(-50, MIN(50, n.dx));
            anchory += MAX(-50, MIN(50, n.dy));

            anchorx = MAX(minx, MIN(maxx, anchorx)); // ignore if (n.x,n.y) goes outside the range
            anchory = MAX(miny, MIN(maxy, anchory));

            n.x = anchorx + n.offx;
            n.y = anchory + n.offy;
        }
        else // movable
        {
            n.x += MAX(-50, MIN(50, n.dx));
            n.y += MAX(-50, MIN(50, n.dy));

            n.x = MAX(minx, MIN(maxx, n.x));
            n.y = MAX(miny, MIN(maxy, n.y));

        }
        n.dx /= 2;
        n.dy /= 2;
    }
    return 10000; //...
}


