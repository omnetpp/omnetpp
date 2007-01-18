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
#include <assert.h>

#include <deque>

#include "graphlayout.h"
#include "startreeembedding.h"
#include "heapembedding.h"

#define MIN(a,b) ((a)<(b) ? (a) : (b))
#define MAX(a,b) ((a)<(b) ? (b) : (a))

// TODO: shall we move to this into a separate file?
bool resolveBoolDispStrArg(const char *s, bool defaultval)
{
   if (!s || !*s)
       return defaultval;
   return !strcmpi("1", s) || !strcmpi("true", s);
}

long resolveLongDispStrArg(const char *s, cModule *mod, int defaultval)
{
   if (!s || !*s)
       return defaultval;
   if (*s=='$')
       return displayStringPar(s+1, mod, true)->longValue();
   return (long) atol(s);
}

double resolveDoubleDispStrArg(const char *s, double defaultval)
{
   if (!s || !*s)
       return defaultval;
   return atof(s);
}

cPar *displayStringPar(const char *parname, cModule *mod, bool searchparent)
{
   cPar *par = NULL;
   int k = mod->findPar(parname);
   if (k>=0)
      par = &(mod->par(k));

   if (!par && searchparent && mod->parentModule())
   {
      k = mod->parentModule()->findPar( parname );
      if (k>=0)
         par = &(mod->parentModule()->par(k));
   }
   if (!par)
   {
      if (!searchparent)
          throw cRuntimeError("module `%s' has no parameter `%s'", mod->fullPath().c_str(), parname);
      else
          throw cRuntimeError("module `%s' and its parent have no parameter `%s'", mod->fullPath().c_str(), parname);
   }
   return par;
}

GraphLayouter::GraphLayouter()
{
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

    defaultEdgeLen = 40;
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

void BasicSpringEmbedderLayout::setDisplayString(const cDisplayString& ds, cModule *parentmodule)
{
    int repf = resolveLongDispStrArg(ds.getTagArg("bgl",0), parentmodule, -1);
    if (repf>0) repulsiveForce = repf;

    int attf = resolveLongDispStrArg(ds.getTagArg("bgl",1), parentmodule, -1);
    if (attf>0) attractionForce = attf;

    int edgelen = resolveLongDispStrArg(ds.getTagArg("bgl",2), parentmodule, -1);
    if (edgelen>0) defaultEdgeLen = edgelen; // this should come before adding edges

    int maxiter = resolveLongDispStrArg(ds.getTagArg("bgl",3), parentmodule, -1);
    if (maxiter>0) maxIterations = maxiter;

#ifdef USE_CONTRACTING_BOX
    boxContractionForce = resolveNumericDispStrArg(ds.getTagArg("bpars",0), parentmodule, 100);
    boxRepulsiveForce = resolveNumericDispStrArg(ds.getTagArg("bpars",1), parentmodule, 100);
    boxRepForceRatio = resolveNumericDispStrArg(ds.getTagArg("bpars",2), parentmodule, 1);
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
    Tcl_VarEval(interp, canvas, " create rect ", coords, " -outline black -tag {box bbox}", NULL);
#endif
    for (NodeList::iterator i=nodes.begin(); i!=nodes.end(); ++i)
    {
        Node& n = *(*i);
        sprintf(coords,"%g %g %g %g", n.x-n.sx, n.y-n.sy, n.x+n.sx, n.y+n.sy);
        const char *color = colors[n.color % (sizeof(colors)/sizeof(char*))];
        Tcl_VarEval(interp, canvas, " create rect ",coords," -outline ",color," -tag {node bbox}", NULL);
    }
    for (EdgeList::iterator j=edges.begin(); j!=edges.end(); ++j)
    {
        Edge& e = *j;
        sprintf(coords,"%g %g %g %g", e.from->x, e.from->y, e.to->x, e.to->y);
        const char *color = colors[e.from->color % (sizeof(colors)/sizeof(char*))];
        Tcl_VarEval(interp, canvas, " create line ",coords," -fill ",color," -tag {edge bbox}",NULL);
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

/*******************************************************/

ForceDirectedGraphLayouter::ForceDirectedGraphLayouter()
{
    finalized = false;
    bordersAdded = false;
    hasFixedNode = false;
    hasMovableNode = false;

    topBorder = NULL;
    bottomBorder = NULL;
    leftBorder = NULL;
    rightBorder = NULL;
}

void ForceDirectedGraphLayouter::setDisplayString(const cDisplayString& ds, cModule *parentmodule)
{
    embedding.parameters = embedding.getParameters(lcgRandom.getSeed());
    embedding.parameters.defaultPointLikeDistance = resolveBoolDispStrArg(ds.getTagArg("pld", 0), embedding.parameters.defaultPointLikeDistance);
    embedding.parameters.defaultSpringCoefficient = resolveDoubleDispStrArg(ds.getTagArg("sc", 0), embedding.parameters.defaultSpringCoefficient);
    embedding.parameters.defaultSpringReposeLength = resolveDoubleDispStrArg(ds.getTagArg("srl", 0), embedding.parameters.defaultSpringReposeLength);
    embedding.parameters.electricRepulsionCoefficient = resolveDoubleDispStrArg(ds.getTagArg("erc", 0), embedding.parameters.electricRepulsionCoefficient);
    embedding.parameters.frictionCoefficient = resolveDoubleDispStrArg(ds.getTagArg("fc", 0), embedding.parameters.frictionCoefficient);
    embedding.parameters.minTimeStep = resolveDoubleDispStrArg(ds.getTagArg("mits", 0), embedding.parameters.minTimeStep);
    embedding.parameters.maxTimeStep = resolveDoubleDispStrArg(ds.getTagArg("mats", 0), embedding.parameters.maxTimeStep);
    embedding.parameters.minAccelerationError = resolveDoubleDispStrArg(ds.getTagArg("miae", 0), embedding.parameters.minAccelerationError);
    embedding.parameters.maxAccelerationError = resolveDoubleDispStrArg(ds.getTagArg("maae", 0), embedding.parameters.maxAccelerationError);
    embedding.parameters.velocityRelaxLimit = resolveDoubleDispStrArg(ds.getTagArg("vrl", 0), embedding.parameters.velocityRelaxLimit);
    embedding.parameters.maxCalculationTime = resolveDoubleDispStrArg(ds.getTagArg("mct", 0), embedding.parameters.maxCalculationTime);

    threeDFactor = resolveDoubleDispStrArg(ds.getTagArg("3d", 0), std::max(0.0, privRand01() - 0.5)); // default: 0.1

    preEmbedding = resolveBoolDispStrArg(ds.getTagArg("pe", 0), privRand01() > 0.5); // default: true
    forceDirectedEmbedding = resolveBoolDispStrArg(ds.getTagArg("fde", 0), true);

    // debug parameters
    debugWaitTime = resolveDoubleDispStrArg(ds.getTagArg("dwt", 0), 0);
    showForces = resolveBoolDispStrArg(ds.getTagArg("sf", 0), false);
    showSummaForce = resolveBoolDispStrArg(ds.getTagArg("ssf", 0), false);
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

void ForceDirectedGraphLayouter::setRandomPositions(double size)
{
    const std::vector<IBody *>& bodies = embedding.getBodies();

    if (size == -1) {
        size = pow(embedding.parameters.defaultSpringReposeLength, 2) * bodies.size();

	    for (int i = 0; i < bodies.size(); i++) {
		    IBody *body = bodies[i];

            if (!dynamic_cast<WallBody *>(body))
                size += 10 * body->getSize().getArea();
        }

        size = sqrt(size);
    }

    ASSERT(!isNaN(size));

    const std::vector<Variable *>& variables = embedding.getVariables();
	for (int i = 0; i < variables.size(); i++) {
        Pt pt = variables[i]->getPosition();

        if (isNaN(pt.x))
            pt.x = privRand01() * size;

        if (isNaN(pt.y))
            pt.y = privRand01() * size;

        if (isNaN(pt.z))
            pt.z = (privRand01() - 0.5) * size * threeDFactor;

        variables[i]->assignPosition(pt);
    }
}

void ForceDirectedGraphLayouter::setInitialPositions(double distance) {
    GraphComponent childrenComponentsStar;
    Vertex *childrenComponentsStarRoot = NULL;

    for (std::vector<GraphComponent *>::iterator it = graphComponent.coherentSubComponents.begin(); it != graphComponent.coherentSubComponents.end(); it++) {
	    GraphComponent *childComponent = *it;
        childComponent->calculateSpanningTree();

        if (childComponent->getEdgeCount() == childComponent->getVertexCount() - 1) {
            StarTreeEmbedding starTreeEmbedding(childComponent, distance);
            starTreeEmbedding.embed();
        }
        else {
            HeapEmbedding heapEmbedding(childComponent, distance);
            heapEmbedding.embed();
        }

        Vertex *childComponentVertex = new Vertex(Pt::getNil(), childComponent->getSize(), NULL);
        if (!childrenComponentsStarRoot)
            childrenComponentsStarRoot = childComponentVertex;

        childrenComponentsStar.addVertex(childComponentVertex);
        childrenComponentsStar.addEdge(new Edge(childrenComponentsStarRoot, childComponentVertex));
    }

    childrenComponentsStar.calculateSpanningTree();
    HeapEmbedding heapEmbedding(&childrenComponentsStar, distance);
    heapEmbedding.embed();

    for (int i = 0; i < graphComponent.coherentSubComponents.size(); i++) {
        Vertex *childComponentVertex = childrenComponentsStar.getVertex(i);
	    GraphComponent *childComponent = graphComponent.coherentSubComponents[i];

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
    // TODO: move this to the embedding, so that it can be called from java
    if (!bordersAdded) {
        double coefficient = 0.1;

		topBorder = new WallBody(true, NaN);
		bottomBorder = new WallBody(true, NaN);
		leftBorder = new WallBody(false, NaN);
		rightBorder = new WallBody(false, NaN);

        const std::vector<IBody *>& bodies = embedding.getBodies();
		for (int i = 0; i < bodies.size(); i++) {
			IBody *body = bodies[i];
			embedding.addForceProvider(new VerticalElectricRepulsion(topBorder, body));
			embedding.addForceProvider(new VerticalElectricRepulsion(bottomBorder, body));
			embedding.addForceProvider(new HorizontalElectricRepulsion(leftBorder, body));
			embedding.addForceProvider(new HorizontalElectricRepulsion(rightBorder, body));
		}

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
    // TODO: move this to the embedding, so that it can be called from java
    double distance = 100;
	double top = DBL_MAX, bottom = DBL_MIN;
	double left = DBL_MAX, right = DBL_MIN;

    const std::vector<IBody *>& bodies = embedding.getBodies();
	for (int i = 0; i < bodies.size(); i++) {
		IBody *body = bodies[i];

        if (!dynamic_cast<WallBody *>(body)) {
            top = std::min(top, body->getTop());
		    bottom = std::max(bottom, body->getBottom());
		    left = std::min(left, body->getLeft());
		    right = std::max(right, body->getRight());
        }
	}

    topBorder->setPosition(top - distance);
    bottomBorder->setPosition(bottom + distance);
    leftBorder->setPosition(left - distance);
    rightBorder->setPosition(right + distance);
}

void ForceDirectedGraphLayouter::addElectricRepulsions()
{
    // TODO: move this to the embedding, so that it can be called from java
    const std::vector<IBody *>& bodies = embedding.getBodies();
    for (int i = 0; i < bodies.size(); i++)
	    for (int j = i + 1; j < bodies.size(); j++) {
    		IBody *body1 = bodies[i];
	    	IBody *body2 = bodies[j];
            Variable *variable1 = body1->getVariable();
            Variable *variable2 = body2->getVariable();

		    if (!dynamic_cast<WallBody *>(body1) &&
                !dynamic_cast<WallBody *>(body2) &&
			    variable1 != variable2)
            {
                Vertex *vertex1 = graphComponent.findVertex(variable1);
                Vertex *vertex2 = graphComponent.findVertex(variable2);

                ASSERT(vertex1);
                ASSERT(vertex2);

                if (vertex1->coherentSubComponent != vertex2->coherentSubComponent)
	    		    embedding.addForceProvider(new ElectricRepulsion(body1, body2, 100, 200));
                else
    			    embedding.addForceProvider(new ElectricRepulsion(body1, body2));
            }
	    }
}

void ForceDirectedGraphLayouter::addBasePlaneSprings()
{
    const std::vector<IBody *>& bodies = embedding.getBodies();
    for (int i = 0; i < bodies.size(); i++) {
  		IBody *body = bodies[i];

        if (!dynamic_cast<WallBody *>(body))
    		embedding.addForceProvider(new BasePlaneSpring(body, 5, 0));
    }
}

void ForceDirectedGraphLayouter::ensureFinalized()
{
    if (!finalized) {
        addElectricRepulsions();

        if (threeDFactor > 0)
            addBasePlaneSprings();

        embedding.addForceProvider(new Drag());

        setRandomPositions();

        if (bordersAdded)
            setBorderPositions();

        finalized = true;
    }
}

void ForceDirectedGraphLayouter::normalize()
{
	double top = DBL_MAX;
	double left = DBL_MAX;

    const std::vector<IBody *>& bodies = embedding.getBodies();
	for (int i = 0; i < bodies.size(); i++) {
		IBody *body = bodies[i];

        if (!dynamic_cast<WallBody *>(body)) {
            top = std::min(top, body->getTop());
		    left = std::min(left, body->getLeft());
        }
	}

    const std::vector<Variable *>& variables = embedding.getVariables();
	for (int i = 0; i < variables.size(); i++) {
        variables[i]->assignPosition(variables[i]->getPosition().subtract(Pt(left - border, top - border, 0)));
    }
}

void ForceDirectedGraphLayouter::addMovableNode(cModule *mod, int width, int height)
{
    hasMovableNode = true;

    Variable *variable = new Variable(Pt::getNil());
    addBody(mod, new Body(variable, Rs(width, height)));

    graphComponent.addVertex(new Vertex(Pt::getNil(), Rs(width, height), variable));
}

void ForceDirectedGraphLayouter::addFixedNode(cModule *mod, int x, int y, int width, int height)
{
    hasFixedNode = true;

    Variable *variable = new PointConstrainedVariable(Pt(x + width / 2, y + height / 2, NaN));
    IBody *body = new Body(variable, Rs(width, height));
    addBody(mod, body);

    graphComponent.addVertex(new Vertex(Pt(x, y, NaN), Rs(width, height), variable));
}

void ForceDirectedGraphLayouter::addAnchoredNode(cModule *mod, const char *anchorname, int offx, int offy, int width, int height)
{
    Variable *variable = ensureAnchorVariable(anchorname);
    addBody(mod, new RelativelyPositionedBody(variable, Pt(offx, offy, 0), Rs(width, height)));

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
    Spring *spring = new Spring(findBody(from), findBody(to), -1, !len ? -1 : len);
    embedding.addForceProvider(spring);
    graphComponent.addEdge(new Edge(
			graphComponent.findVertex(spring->getBody1()->getVariable()),
			graphComponent.findVertex(spring->getBody2()->getVariable())));
}

void ForceDirectedGraphLayouter::addEdgeToBorder(cModule *from, int len)
{
    ensureBorders();

    IBody *body = findBody(from);

    std::vector<AbstractSpring *> springs;
    springs.push_back(new VerticalSpring(topBorder, body, -1, !len ? -1 : len));
    springs.push_back(new VerticalSpring(bottomBorder, body, -1, !len ? -1 : len));
    springs.push_back(new HorizonalSpring(leftBorder, body, -1, !len ? -1 : len));
    springs.push_back(new HorizonalSpring(rightBorder, body, -1, !len ? -1 : len));

    embedding.addForceProvider(new LeastExpandedSpring(springs));
}

void ForceDirectedGraphLayouter::execute()
{
    bool inspected = interp && canvas;

    // call pre embedding if requested
    graphComponent.calculateCoherentSubComponents();
    if (hasMovableNode && preEmbedding)
        setInitialPositions(std::min(40.0, embedding.parameters.defaultSpringReposeLength));

    if (inspected)
        debugDraw();

    // call force directed embedding if requested
    if (hasMovableNode && forceDirectedEmbedding) {
        ensureFinalized();

        //embedding.debug = 3;
        embedding.inspected = inspected;
        embedding.reinitialize();

        if (embedding.inspected) {
            int step = 0;
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

    if (inspected)
    {
        Tcl_VarEval(interp, canvas," delete node\n",
                            canvas," delete edge\n",
                            canvas," delete force\n",
                            canvas," config -scrollregion {0 0 1 1}\n",
                            canvas," xview moveto 0\n",
                            canvas," yview moveto 0\n",
                            NULL);
    }

    if (!hasFixedNode)
        normalize();
}

void ForceDirectedGraphLayouter::getNodePosition(cModule *mod, int& x, int& y)
{
    IBody *body = findBody(mod);
    Pt pt = body->getPosition();
    x = pt.x;
    y = pt.y;
}

void ForceDirectedGraphLayouter::debugDraw()
{
    if (TCL_ERROR==Tcl_VarEval(interp, canvas, " delete all", NULL)) return;
    const char *colors[] = {"black","red","blue","green","yellow","cyan","purple","darkgreen"};
    char coords[100];

    const std::vector<IBody *>& bodies = embedding.getBodies();
	for (int i = 0; i < bodies.size(); i++) {
		IBody *body = bodies[i];
        Pt pt = body->getPosition();
        Rs rs = body->getSize();
        pt.subtract(Pt(rs.width / 2, rs.height / 2, 0));

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

        sprintf(coords,"%g %g %g %g", pt.x, pt.y, pt.x + rs.width, pt.y + rs.height);
        const char *color = "black";
        Tcl_VarEval(interp, canvas, " create rect ", coords, " -outline ", color, " -tag {node bbox}", NULL);
        sprintf(coords,"%g %g", pt.x, pt.y);
        char text[100];
        sprintf(text,"%g", pt.z);
        Tcl_VarEval(interp, canvas, " create text ", coords, " -text ", text, " -tag {node bbox}", NULL);
    }

    const std::vector<IForceProvider *>& forceProviders = embedding.getForceProviders();
	for (int i = 0; i < forceProviders.size(); i++) {
		Spring *spring = dynamic_cast<Spring *>(forceProviders[i]);

        if (spring) {
            Pt pt1 = spring->getBody1()->getPosition();
            Pt pt2 = spring->getBody2()->getPosition();
            sprintf(coords,"%g %g %g %g", pt1.x, pt1.y, pt2.x, pt2.y);
            const char *color = "black";
            Tcl_VarEval(interp, canvas, " create line ", coords, " -fill ", color, " -tag {edge bbox}", NULL);
        }
    }

    double forceScale = 10;
    const std::vector<Variable *>& variables = embedding.getVariables();
    for (int i = 0; i < variables.size(); i++) {
        Variable *variable = variables[i];
        Pt pt1 = variable->getPosition();

        if (showForces) {
            std::vector<Pt> forces = variable->getForces();

	        for (int j = 0; j < forces.size(); j++) {
                Pt pt2(pt1);
                Pt force(forces[j]);
                force.multiply(forceScale);
                pt2.add(force);
                sprintf(coords,"%g %g %g %g", pt1.x, pt1.y, pt2.x, pt2.y);
                const char *color = "red";
                Tcl_VarEval(interp, canvas, " create line ", coords, " -fill ", color, " -tag force", NULL);
            }
        }

        if (showSummaForce) {
            Pt pt2(pt1);
            Pt force(variable->getForce());
            force.multiply(forceScale);
            pt2.add(force);
            sprintf(coords,"%g %g %g %g", pt1.x, pt1.y, pt2.x, pt2.y);
            const char *color = "blue";
            Tcl_VarEval(interp, canvas, " create line ", coords, " -fill ", color, " -tag force", NULL);
        }
    }

    std::stringstream info;
    embedding.writeDebugInformation(info);
    Tcl_VarEval(interp, canvas, " raise node", NULL);
    Tcl_VarEval(interp, "layouter_debugDraw_finish ", canvas, " {", info.str().c_str(), "} ", NULL);
    _sleep(debugWaitTime);
}
