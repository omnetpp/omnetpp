/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.figures.layout;

import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.Connection;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.LayoutManager;
import org.eclipse.draw2d.XYLayout;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.common.Debug;
import org.omnetpp.figures.CompoundModuleFigure;
import org.omnetpp.figures.SubmoduleFigure;


/**
 * TODO add documentation
 *
 * @author rhornig
 */
public class SpringEmbedderLayout extends XYLayout {
    private static boolean debug = false;

    private static final Dimension DEFAULT_SIZE = new Dimension(300, 200);
	private static final int DEFAULT_MAX_WIDTH = 740;
	private static final int DEFAULT_MAX_HEIGHT = 500;
	protected long algSeed = 1971;
    private boolean requestAutoLayout = true;
    private boolean incrementalLayout = false;
    private CompoundModuleFigure compoundFigure;

	/**
	 *
	 * @param nodeParent The parent figure of the nodes
	 * @param connectionParent The parent figure of the connection figures
	 */
	public SpringEmbedderLayout(CompoundModuleFigure compoundFigure) {
		super();
		this.compoundFigure = compoundFigure;
 	}

	/**
	 * Calls the autoLayout algorithm using all currently specified constraints.
	 * @param nodeParent The Parent figure of the nodes
	 * @param edgeParent The parent figure of the connections (usually the ConnectionLayer)
	 */
	@SuppressWarnings("unchecked")
	protected AbstractGraphLayoutAlgorithm createAutoLayouter() {
	    IFigure nodeParent = compoundFigure.getSubmoduleLayer(); 
	    IFigure edgeParent = compoundFigure.getConnectionLayer();
	    BasicSpringEmbedderLayoutAlgorithm autoLayouter = new BasicSpringEmbedderLayoutAlgorithm();
		autoLayouter.setDefaultEdgeLength(100);
		autoLayouter.setRepulsiveForce(500.0);
		autoLayouter.setMaxIterations(500);
		// set the layouting area
		int width = compoundFigure.getBackgroundSize().width;
		int height = compoundFigure.getBackgroundSize().height;
		// if the size is not present, use a default size;
		if (width <= 0) width = DEFAULT_MAX_WIDTH;
		if (height <= 0) height = DEFAULT_MAX_HEIGHT;
		autoLayouter.setScaleToArea(width, height, 50);

		// iterate over the nodes and add them to the algorithm
		// all child figures on this layer are considered as node
		for (IFigure node : (List<IFigure>)nodeParent.getChildren()) {
			// get the associated constraint (coordinates) if any
			SubmoduleConstraint constr = (SubmoduleConstraint)getConstraint(node);
			Assert.isNotNull(constr,"A figure must have an associated constraint");

			if(constr.isUnspecified()) {
				// if unspec. use a random start position 
				autoLayouter.addMovableNode(node, (int)(Math.random()*width), (int)(Math.random()*height), 
						constr.width, constr.height);
			} else if (constr.isPinned() || incrementalLayout) {
				// on incremental layout, everything is fixed except unspecified nodes 
				autoLayouter.addFixedNode(node, constr.x, constr.y, constr.width, constr.height);
			}
			else
				autoLayouter.addMovableNode(node, constr.x, constr.y, constr.width, constr.height);
		}

		// iterate over the connections and add them to the algorithm
		// all child figures of type Connection on this layer are considered as edge
		for (IFigure edge : (List<IFigure>)edgeParent.getChildren())
			if (edge instanceof Connection) {
				Connection conn = (Connection)edge;
				IFigure srcFig = conn.getSourceAnchor().getOwner();
				IFigure targetFig = conn.getTargetAnchor().getOwner();
                // add the edge ONLY if it has figures at both side
                if (srcFig == null || targetFig == null)
                    continue;
				// if this is an edge coming from outside to a submodule
				if (srcFig instanceof CompoundModuleFigure) {
					autoLayouter.addEdgeToBorder(targetFig, 0);
				} // else if this is an edge going out from a submodule
				else if (targetFig instanceof CompoundModuleFigure) {
					autoLayouter.addEdgeToBorder(srcFig, 0);
				}
				else {  // both are submodules
					autoLayouter.addEdge(srcFig, targetFig, 0);
				}

			}
		return autoLayouter;
	}


	/**
     * Implements the algorithm to layout the components of the given container figure.
     * Each component is laid out using its own layout constraint specifying its size
     * and position. Copied from XYLayout BUT places the middle point of the children to the
     * specified constraint location.
     *
     * @see LayoutManager#layout(IFigure)
     */
	@Override
    @SuppressWarnings("unchecked")
    public void layout(IFigure parent) {
	    long startTime = System.currentTimeMillis();
	    
        AbstractGraphLayoutAlgorithm alg = null;
        // find the place of movable nodes if auto-layout requested
        if (requestAutoLayout) {
            alg = createAutoLayouter();
            alg.setSeed((int)algSeed);
            alg.execute();
            requestAutoLayout = false;
        }

    	// lay out the children according to the auto-layouter
        Point offset = getOrigin(parent);
        for (IFigure f : (List<IFigure>)parent.getChildren()) {
            Rectangle constr = ((Rectangle)getConstraint(f));
            Assert.isNotNull(constr, "Figure must have an associated constraint");
            
            // by default use the size and location from the constraint object (must create a copy!)

            // get the computed location from the auto-layout algorithm (if there is an algorithm at all)
            if (alg != null) {
            	Point locFromAlg = alg.getNodePosition(f);
            	Assert.isNotNull(locFromAlg, "There is a figure witout a position calculated by the layouting algorythm");
            	// we write back the calculated locations to the constraint so they can be used next
            	// time as starting positions for the movable nodes
            	constr.setLocation(locFromAlg);
            }

            Rectangle newBounds = constr.getCopy();
            newBounds.translate(offset);
            // translate to the middle of the figure
            newBounds.translate(-newBounds.width / 2, -newBounds.height / 2);
            f.setBounds(newBounds);
        }
        if (debug)
            Debug.println("SpringEmbedderLayout: " + (System.currentTimeMillis()-startTime) + "ms");
    }

    /**
     * Returns the point (0,0) as the origin. This is required for correct freeform handling.
     * As we want to use it in a FreeformLayer
     * @see XYLayout#getOrigin(IFigure)
     */
    @Override
    public Point getOrigin(IFigure figure) {
        return new Point();
    }

	/**
	 * Set a new seed for the layouting algorithm. The next layout will use this seed
	 * @param algSeed
	 */
	public void setSeed(long algSeed) {
		this.algSeed = algSeed;
	}

	/**
	 * Returns the current seed value
	 */
	public long getSeed() {
		return algSeed;
	}

	/**
	 * After calling this, the next layout process will call a full layout process. 
	 * Pinned nodes will stay, unpinned nodes will move (with starting position taken
	 * from the current constraint). Nodes with unspecified location will be layouted
	 * by generating a random starting position. 
	 */
	public void requestFullLayout() {
		incrementalLayout = false;
	    requestAutoLayout = true;
	}
	
	/**
	 * After calling this, the next layout process will call an incremental layout process. 
	 * Pinned  and unpinned nodes will stay. Nodes with unspecified location will be layouted
	 * by generating a random starting position. 
	 */
	public void requestIncrementalLayout() {
		incrementalLayout = true;
	    requestAutoLayout = true;
	}
	// XXX this algorithm is using ONLY the figure bounds. It might be much better to move
	// it to submoduleLayer.getPreferredSize
	
	@SuppressWarnings("unchecked")
	@Override
	protected Dimension calculatePreferredSize(IFigure f, int hHint, int wHint) {
		// if we have a size hint, we always use that for the size
		if (hHint >= 0 && wHint >= 0)
			return new Dimension(hHint,wHint);
		
	    Rectangle rect = null;
	    for(IFigure child : (List<IFigure>)f.getChildren()) {
	        Rectangle r = child.getBounds();
	        
	        if (rect == null)
	        	rect = r.getCopy();
	        else
	        	rect.union(r);
	        
	        if (child instanceof SubmoduleFigure) {
	            // add the label bounds to it
	            rect.union(((SubmoduleFigure)child).getLabelBounds());
	        }
	        
	    }
	    
	    // use the default size if no submodule children were present
	    if (rect == null)
	    	return new Dimension(DEFAULT_SIZE);
	    
	    // we use the same amount of space on the up/down and the left/right side
	    Dimension result = new Dimension(rect.x + Math.max(rect.x,0) + rect.width + f.getInsets().getWidth(), rect.y + Math.max(rect.y,0) + rect.height + f.getInsets().getHeight()).
	        union(getBorderPreferredSize(f));
		return result;
	}


//	@SuppressWarnings("unchecked")
//	@Override
//	protected Dimension calculatePreferredSize(IFigure f, int hHint, int wHint) {
//		// if we have a size hint, we always use that for the size
//		if (hHint >= 0 && wHint >= 0)
//			return new Dimension(hHint,wHint);
//		
//	    Rectangle rect = null;
//	    for(IFigure child : (List<IFigure>)f.getChildren()) {
//	    	// FIXME should we use the figure bounds instead of the constraints?
//	        Rectangle r = (Rectangle)constraints.get(child);
//	        if (r == null)
//	            continue;
//	        
//	        // translate the x,y as we use the center of the figure in the constraint
//	        r = r.getTranslated(-r.width/2, -r.height/2);
//	
//	        if (r.width == -1 || r.height == -1) {
//	            Dimension preferredSize = child.getPreferredSize(r.width, r.height);
//	            r = r.getCopy();
//	            if (r.width == -1)
//	                r.width = preferredSize.width;
//	            if (r.height == -1)
//	                r.height = preferredSize.height;
//	        }
//	        
//	        if (child instanceof SubmoduleFigure) {
//	            // add the label bounds to it
//	            r.union(((SubmoduleFigure)child).getLabelBounds());
//	        }
//	        
//	        if (rect == null)
//	        	rect = r.getCopy();
//	        
//	        rect.union(r);
//	    }
//	    
//	    // use the default size if no submodule children were present
//	    if (rect == null)
//	    	return new Dimension(DEFAULT_SIZE);
//	    
//	    // we use the same amount of space on the up/down and the left/right side
//	    Dimension result = new Dimension(rect.x + Math.max(rect.x,0) + rect.width + f.getInsets().getWidth(), rect.y + Math.max(rect.y,0) + rect.height + f.getInsets().getHeight()).
//	        union(getBorderPreferredSize(f));
//	    System.out.println("SpringEmbedderLayout.calculatePreferredSize: "+result);
//		return result;
//	}
//
}

