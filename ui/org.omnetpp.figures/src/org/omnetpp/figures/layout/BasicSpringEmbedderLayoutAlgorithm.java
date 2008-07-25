package org.omnetpp.figures.layout;

import java.util.ArrayList;
import java.util.List;
import java.util.Stack;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.geometry.Point;

/**
 * Implementation of the Spring Embedder algorithm.
 *
 * Simplifications:
 *  - ignores node size (this is visible when item is very long, e.g.
 *    500 x 10 pixels)
 *  - ignores connections to the parent module (nodes which are connected
 *    to the parent may end up in the middle of the layout)
 *
 * @author andras
 */
public class BasicSpringEmbedderLayoutAlgorithm extends AbstractGraphLayoutAlgorithm
{
    static class Anchor
    {
        String name;      // anchor name
        double x, y;      // position
        int refcount;     // how many nodes are anchored to it
        double dx, dy;    // internal: movement at each step (NOT preserved across iterations!)
    };

    static class Node
    {
        Object key;        // node "handle"
        boolean fixed;     // allowed to move?
        Anchor anchor;     // not NULL for anchored nodes
        double x, y;       // position (of the center of the shape)
        int offx, offy;    // anchored nodes: offset to anchor point (and x,y are calculated)
        int sx, sy;        // half width/height

        double dx, dy;     // internal: movement at each step
        int color;         // internal: connected nodes share the same color
    };

    static class Edge
    {
        Node from;
        Node to;
        int len;
    };

    List<Anchor> anchors = new ArrayList<Anchor>();
    List<Node> nodes = new ArrayList<Node>();;
    List<Edge> edges = new ArrayList<Edge>();;

	//#ifdef USE_CONTRACTING_BOX
	//    static class Box
	//    {
	//        boolean fixed;   // allowed to move?
	//        double x1, y1;   // coordinates
	//        double x2, y2;
	//
	//        double dx1, dy1; // movement
	//        double dx2, dy2;
	//    };
	//
	//    // contracting bounding box
	//    Box box;
	//#endif

    private static int TIMEOUT = 15000;  // 15 sec timeout for the layouter
    int maxIterations;

    boolean haveFixedNode;
    boolean haveAnchoredNode;
    boolean allNodesAreFixed;

    int minx, miny, maxx, maxy;

    double repulsiveForce;
    double attractionForce;

	//#ifdef USE_CONTRACTING_BOX
	//  public:
	//    double boxContractionForce;
	//    double boxRepulsiveForce;
	//    double boxRepForceRatio;
	//#endif

    /**
     * Constructor
     */
    BasicSpringEmbedderLayoutAlgorithm() {
        haveFixedNode = false;
        haveAnchoredNode = false;
        allNodesAreFixed = true; // unless later it proves otherwise

        maxIterations = 500;
        //repulsiveForce = 8;
        repulsiveForce = 50;
        attractionForce = 0.3;
    }

    /**
     * Set repulsive force
     */
    void setRepulsiveForce(double f) {
        repulsiveForce = f;
    }

    /**
     * Set attraction force
     */
    void setAttractionForce(double f) {
        attractionForce = f;
    }

    /**
     * Set max number of iterations
     */
    void setMaxIterations(int n) {
        maxIterations = n;
    }

	@Override
	public void addMovableNode(Object mod, int x, int y, int width, int height) {
	    Assert.isTrue(findNode(mod)==null);

	    allNodesAreFixed = false;

	    Node n = new Node();
	    n.key = mod;
	    n.fixed = false;
	    n.anchor = null;
	    n.sx = width/2;
	    n.sy = height/2;

	    nodes.add(n);
	}

	@Override
	public void addFixedNode(Object mod, int x, int y, int width, int height) {
	    Assert.isTrue(findNode(mod)==null);

	    haveFixedNode = true;

	    Node n = new Node();
	    n.key = mod;
	    n.fixed = true;
	    n.anchor = null;
	    n.x = x;
	    n.y = y;
	    n.sx = width/2;
	    n.sy = height/2;

	    nodes.add(n);
	}

	@Override
	public void addAnchoredNode(Object mod, String anchorname, int offx, int offy, int width, int height) {
	    Assert.isTrue(findNode(mod)==null);

	    haveAnchoredNode = true;
	    allNodesAreFixed = false;

	    Node n = new Node();
	    n.key = mod;

	    Anchor anchor = null;
	    for (Anchor a : anchors) {
	        if (a.name.equals(anchorname)) {
	            anchor = a; break;
	        }
	    }
	    if (anchor==null) {
	    	anchor = new Anchor();
	    	anchors.add(anchor);
	        anchor.name = anchorname;
	        anchor.refcount = 0;
	    }

	    n.anchor = anchor;
	    anchor.refcount++;

	    n.fixed = false;
	    n.offx = offx;
	    n.offy = offy;
	    n.sx = width/2;
	    n.sy = height/2;

	    nodes.add(n);
	}

	@Override
	public void addEdge(Object from, Object to, int len) {
	   Assert.isTrue(findNode(from)!=null && findNode(to)!=null);

	    Edge e = new Edge();
	    e.from = findNode(from);
	    e.to = findNode(to);
	    e.len = len>0 ? len : defaultEdgeLen;
	    Assert.isTrue(e.to!=null && e.from!=null);

	    // heuristics to take submodule size into account
	    e.len += 2*(Math.min(e.from.sx,e.from.sy)+Math.min(e.to.sx,e.to.sy));

	    edges.add(e);
	}

	@Override
	public void addEdgeToBorder(Object from, int len) {
	    // XXX this layouter algorithm ignores connections to border
	}

	@Override
	public void execute() {
        long startMillis = System.currentTimeMillis();
		
	    if (nodes.isEmpty() || allNodesAreFixed)
	        return;

	    if (debug) 
			System.out.println("running layouter: " + nodes.size() + " nodes, " + "allNodesAreFixed=" + allNodesAreFixed + ", haveAnchoredNode=" + haveAnchoredNode + ", haveFixedNode=" + haveFixedNode);
	    
	    // consume a some values (manually given seeds are usually small!)
	    privRand01();
	    privRand01();
	    privRand01();

	    // initialize variables (also randomize start positions)
	    for (Anchor a : anchors)
	    {
	        a.x = 100 * privRand01();
	        a.y = 100 * privRand01();
	        a.dx = a.dy = 0;
	    }
	    for (Node n : nodes)
	    {
	        if (n.fixed)
	        {
	            // nop
	        }
	        else if (n.anchor!=null)
	        {
	            n.x = n.anchor.x + n.offx;
	            n.y = n.anchor.y + n.offy;
	        }
	        else // movable
	        {
	            n.x = 100 * privRand01();
	            n.y = 100 * privRand01();
	        }
	        n.dx = n.dy = 0;
	    }

		//	#ifdef USE_CONTRACTING_BOX
		//	    // initial box (slightly bigger than bounding box of nodes):
		//	    box.x1 = -10;
		//	    box.y1 = -10;
		//	    box.x2 = 110;
		//	    box.y2 = 110;
		//	    box.dx1 = box.dy1 = box.dx2 = box.dy2 = 0;
		//	#endif

	    // set area
	    if (sizingMode==SIZINGMODE_CONFINE)
	    {
	        minx = border;
	        miny = border;
	        maxx = width - border;
	        maxy = height - border;
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
	    // or we spent more time on layouting then the preset timeout
	    int i, maxdcounter=0;
	    for (i=1; i<maxIterations && maxdcounter<20 && (System.currentTimeMillis()-startMillis)<TIMEOUT; i++)
	    {
	        double maxd = relax();

	        if (maxd<0.05)
	            maxdcounter++;
	        else
	            maxdcounter=0;

	        if (debug) {
	        	System.out.println("iteration " + i + ":");
	        	debugPrintState();
	        }
	    }

	    if (debug) 
			System.out.println("layout done: " + nodes.size() + " nodes, " + i + " iterations");

	    // scale back if too big -- BUT scale back only non fixed nodes.
	    // fixed nodes do not change position
	    if (sizingMode==SIZINGMODE_SCALE)
	    {
	        // calculate bounding box
	        double x1, y1, x2, y2;
	        Node n0 = nodes.get(0);
	        x1 = x2 = n0.x;
	        y1 = y2 = n0.y;
	        for (Node n : nodes)
	        {
	        	// skip the fixed nodes
	        	if (n.fixed) continue;
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
	    	    if (debug) 
	    			System.out.println("layout scaled back by (" + (1/xfact) + ", " + (1/yfact) + ")");
	            for (Node n : nodes)
	            {
		        	// skip the fixed nodes
		        	if (n.fixed) continue;
	                n.x = bx + (n.x-x1)*xfact;
	                n.y = by + (n.y-y1)*yfact;
	            }
	        }
	        else
	        {
	            // don't want to rescale with anchored nodes, just shift bounding box to (bx,by)
	            for (Node n : nodes)
	            {
		        	// skip the fixed nodes
		        	if (n.fixed) continue;
	                n.x = bx + n.x - x1;
	                n.y = by + n.y - y1;
	            }
	        }
	    }
	}

	@Override
	public Point getNodePosition(Object mod) {
	    Node n = findNode(mod);
	    if (n == null)
	        return null;

	    return new Point(n.x, n.y);
	}

    // utility
	protected Node findNode(Object mod)
	{
	    for (Node node : nodes)
	        if (node.key == mod)
	            return node;
	    return null;
	}

    /**
     * Mark connected nodes with same color (needed by relax())
     */
    protected void doColoring()
    {
        for (Node n : nodes)
            n.color = -1;

        int currentColor = 0;
        Stack<Node> todoList = new Stack<Node>();
        for (Node n : nodes)
        {
            if (n.color!=-1) continue;  // already assigned

            // breadth-width search to color all connected nodes (transitive closure)
            Assert.isTrue(todoList.size()==0);
            todoList.add(n); // start at this node
            while (!todoList.isEmpty())
            {
                Node nn = todoList.pop();
                nn.color = currentColor;

                // color and add to list all nodes connected to n.
                // (edge list data structure is not really good for this, but execution
                // time is still negligible compared to relax() iterations)
                for (Edge e : edges)
                {
                    if (e.from==nn && e.to.color==-1)
                        todoList.add(e.to);
                    else if (e.to==nn && e.from.color==-1)
                        todoList.add(e.from);
                }
            }

            // next color
            currentColor++;
        }
    }

    /**
     * Main algorithm (modified spring embedder)
     */
    protected double relax()
    {
        // TBD revise:
        //   - calculates in double (slow)
        //   - ignores connections to parent module
        //   - ignores node sizes altogether

        double noiseLevel = 0; // noise doesn't seem to help

        // edge attraction: calculate if edges are longer or shorter than requested (tension),
        // and modify their (dx,dy) movement vector accordingly
        for (Edge e : edges)
        {
            if (e.from.fixed && e.to.fixed)
                continue;
            double vx = e.to.x - e.from.x;
            double vy = e.to.y - e.from.y;
            double len = Math.sqrt(vx * vx + vy * vy);
            len = len == 0 ? 1.0 : len;
            double f = attractionForce * (e.len - len) / len;
            double dx = f * vx;
            double dy = f * vy;

            e.to.dx += dx;
            e.to.dy += dy;
            e.from.dx += -dx;
            e.from.dy += -dy;
        }

        // nodes repulse each other, update (dx,dy) with this effect
        //
        // modification to the original algorithm: only nodes that share the
        // same color (i.e., are connected) repulse each other -- repulsion between
        // nodes of *different* colors ceases after a short distance. (This is done
        // to avoid "blow-up" of non-connected graphs.)
        //
        for (Node n1 : nodes)
        {
            if (n1.fixed)
                continue;

            double fx = 0;
            double fy = 0;

            // TBD performance improvement: use (i=0..N, j=i+1..N) loop unless more than N/2 nodes are fixed
            for (Node n2 : nodes)
            {
                if (n1 == n2)
                    continue;

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

		//    #ifdef USE_CONTRACTING_BOX
		//        // box contraction
		//        box.dx1 += boxContractionForce;
		//        box.dy1 += boxContractionForce;
		//        box.dx2 -= boxContractionForce;
		//        box.dy2 -= boxContractionForce;
		//
		//        // repulsion between box and nodes
		//        for (i=nodes.begin(); i!=nodes.end(); ++i)
		//        {
		//            Node& n = *(*i);
		//
		//            /*
		//            double fx1 = boxRepulsiveForce / sqrt(fabs(n.x - box.x1));
		//            double fx2 = -boxRepulsiveForce / sqrt(fabs(n.x - box.x2));
		//            double fy1 = boxRepulsiveForce / sqrt(fabs(n.y - box.y1));
		//            double fy2 = -boxRepulsiveForce / sqrt(fabs(n.y - box.y2));
		//            */
		//            double fx1 = boxRepulsiveForce / (n.x - box.x1);  // div by zero?
		//            double fx2 = boxRepulsiveForce / (n.x - box.x2);
		//            double fy1 = boxRepulsiveForce / (n.y - box.y1);
		//            double fy2 = boxRepulsiveForce / (n.y - box.y2);
		//
		//            n.dx += fx1;
		//            box.dx1 -= fx1;
		//
		//            n.dx += fx2;
		//            box.dx2 -= fx2;
		//
		//            n.dy += fy1;
		//            box.dy1 -= fy1;
		//
		//            n.dy += fy2;
		//            box.dy2 -= fy2;
		//        }
		//
		//        box.dx1 /= boxRepForceRatio;
		//        box.dy1 /= boxRepForceRatio;
		//        box.dx2 /= boxRepForceRatio;
		//        box.dy2 /= boxRepForceRatio;
		//    #endif

        // limit dx,dy into (-50,50); move nodes by (dx,dy);
        // constrain nodes into rectangle (minx, miny, maxx, maxy)
        double maxd = 0;
        for (Node n : nodes)
        {
            if (n.fixed || n.anchor!=null)
            {
                // fixed nodes don't need to be moved, and anchored nodes are
                // handled separately (see below)
            }
            else // movable
            {
                n.x += Math.max(-50, Math.min(50, n.dx)); // speed limit
                n.y += Math.max(-50, Math.min(50, n.dy));

                n.x += noiseLevel*(privRand01()-0.5); // add noise to push nodes out of local minimums
                n.y += noiseLevel*(privRand01()-0.5);

                n.x = Math.max(minx, Math.min(maxx, n.x));
                n.y = Math.max(miny, Math.min(maxy, n.y));
            }

            // this is used for stopping condition
            if (maxd<n.dx) maxd=n.dx;
            if (maxd<n.dy) maxd=n.dy;

            // "friction" -- nodes stop eventually if not driven by a force
            n.dx /= 2;
            n.dy /= 2;
        }

        // sum up movements of anchor nodes
        for (Anchor a : anchors)
        {
            a.dx = 0;
            a.dy = 0;
        }
        for (Node n : nodes)
        {
            if (n.anchor!=null)
            {
                n.anchor.dx += n.dx;
                n.anchor.dy += n.dy;
            }
        }
        // move anchor points
        for (Anchor a : anchors)
        {
            // double c = sqrt(n.anchor.refcount);
            a.x += Math.max(-50, Math.min(50, a.dx)); // speed limit
            a.y += Math.max(-50, Math.min(50, a.dy));

            a.x += noiseLevel*(privRand01()-0.5); // add noise to push nodes out of local minimums
            a.y += noiseLevel*(privRand01()-0.5);

            a.x = Math.max(minx, Math.min(maxx, a.y));
            a.y = Math.max(miny, Math.min(maxy, a.x));

            // this is used for stopping condition
            if (maxd<a.dx) maxd=a.dx;
            if (maxd<a.dy) maxd=a.dy;

            // "friction" -- nodes stop eventually if not driven by a force
            a.dx /= 2;
            a.dy /= 2;
        }
        // refresh positions of anchored nodes (and distribute anchor's dx,dy among its nodes)
        for (Node n : nodes)
        {
            if (n.anchor!=null)
            {
                n.x = n.anchor.x + n.offx;
                n.y = n.anchor.y + n.offy;

                n.dx = n.anchor.dx / n.anchor.refcount;
                n.dy = n.anchor.dy / n.anchor.refcount;
            }
        }

		//    #ifdef USE_CONTRACTING_BOX
		//        // move box by its dx1,dy1,dx2,dy2
		//        box.x1 += box.dx1;
		//        box.y1 += box.dy1;
		//        box.x2 += box.dx2;
		//        box.y2 += box.dy2;
		//
		//        box.dx1 /= 2;
		//        box.dy1 /= 2;
		//        box.dx2 /= 2;
		//        box.dy2 /= 2;
		//
		//        // calculate bounding rectange and adjust box to be bigger than that
		//        double x1, y1, x2, y2;
		//        Node n0 = nodes.get(0);
		//        x1 = x2 = n0.x;
		//        y1 = y2 = n0.y;
		//        for (Node n : nodes)
		//        {
		//            if (n.x-n.sx < x1) x1 = n.x-n.sx;
		//            if (n.y-n.sy < y1) y1 = n.y-n.sy;
		//            if (n.x+n.sx > x2) x2 = n.x+n.sx;
		//            if (n.y+n.sy > y2) y2 = n.y+n.sy;
		//        }
		//        if (box.x1 >= x1) box.x1 = x1;
		//        if (box.y1 >= y1) box.y1 = y1;
		//        if (box.x2 <= x2) box.x2 = x2;
		//        if (box.y2 <= y2) box.y2 = y2;
		//    #endif
        return maxd;
    }

    protected void debugPrintState() {
    	for (Node n : nodes) {
    		System.out.println("  " + n.key + ": color=" + n.color + " x=" + n.x + " y=" + n.y + " dx=" + n.dx + " dy=" + n.dy);
    	}
    }
    
    @Override
    public Point getAnchorPosition(String anchor) {
        // TODO NOT YET IMPLEMENTED
        return null;
    }

    @Override
    public void setAnchorPosition(String anchor, int x, int y) {
        // TODO NOT YET IMPLEMENTED
    }
}
