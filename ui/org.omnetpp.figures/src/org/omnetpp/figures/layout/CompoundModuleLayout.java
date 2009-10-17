/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.figures.layout;

import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.AssertionFailedException;
import org.eclipse.draw2d.AbstractLayout;
import org.eclipse.draw2d.Connection;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.LayoutManager;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.common.Debug;
import org.omnetpp.figures.CompoundModuleFigure;
import org.omnetpp.figures.SubmoduleFigure;
import org.omnetpp.figures.layout.ISubmoduleConstraint.VectorArrangement;


/**
 * This layouts submodules inside a compound module.
 *
 * @author rhornig, andras
 */
public class CompoundModuleLayout extends AbstractLayout {
    private static boolean debug = false;

    private static final Dimension DEFAULT_SIZE = new Dimension(300, 200);
	private static final int DEFAULT_MAX_WIDTH = 740;
	private static final int DEFAULT_MAX_HEIGHT = 500;
	protected long algSeed = 1971;
    private CompoundModuleFigure compoundFigure;

	/**
	 * Constructor.
	 */
	public CompoundModuleLayout(CompoundModuleFigure compoundFigure) {
		super();
		this.compoundFigure = compoundFigure;
 	}

	/**
	 * Creates the autoLayout algorithm using all currently specified constraints.
	 */
	@SuppressWarnings("unchecked")
	protected AbstractGraphLayoutAlgorithm createAutoLayouter() {
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
		IFigure nodeParent = compoundFigure.getSubmoduleLayer(); 
		for (IFigure node : (List<IFigure>)nodeParent.getChildren()) {
			ISubmoduleConstraint constr = (ISubmoduleConstraint)node;

			Rectangle shapeBounds = constr.getShapeBounds();
			Point centerLocation = constr.getCenterLocation();
			if (centerLocation != null) {
				// use cached (previously layouted) coordinates
				autoLayouter.addFixedNode(node, centerLocation.x, centerLocation.y, shapeBounds.width, shapeBounds.height);
			}
			else {
				// lay out this node, and store the coordinates with setCenterLocation()
				Point baseLoc = constr.getBaseLocation();
				if (baseLoc != null) {
					Point offset = getArrangementOffset(constr, 80); // handle vector layouts; note: we use fixed spacing NOT shape size, because items in a vector may be of different sizes which would result in strange arrangements
					autoLayouter.addFixedNode(node, baseLoc.x+offset.x, baseLoc.y+offset.y, shapeBounds.width, shapeBounds.height);
				} 
				else if (constr.getVectorIdentifier()==null || constr.getVectorArrangement()==VectorArrangement.none) {
					autoLayouter.addMovableNode(node, shapeBounds.width, shapeBounds.height);
				}
				else {
					Point offset = getArrangementOffset(constr, 80); // handle vector layouts
					autoLayouter.addAnchoredNode(node, constr.getVectorIdentifier(), offset.x, offset.y, shapeBounds.width, shapeBounds.height);
				}
			}
		}

		// iterate over the connections and add them to the algorithm
		// all child figures of type Connection on this layer are considered as edge
		IFigure edgeParent = compoundFigure.getConnectionLayer();
		for (IFigure edge : (List<IFigure>)edgeParent.getChildren()) {
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
		}
		return autoLayouter;
	}

	protected Point getArrangementOffset(ISubmoduleConstraint constr, int spacing) {
		int x, y;
		switch (constr.getVectorArrangement()) {
		case none: {
			x = 0;
			y = 0;
			break;
		}
		case exact: {
			x = fallback(constr.getVectorArrangementPar1(), 0); 
			y = fallback(constr.getVectorArrangementPar2(), 0); 
			break;
		}
		case row: { 
			int dx = fallback(constr.getVectorArrangementPar1(), spacing); 
			x = constr.getVectorIndex() * dx;
			y = 0;
			break;
		}
		case column: { 
			int dy = fallback(constr.getVectorArrangementPar1(), spacing); 
			x = 0;
			y = constr.getVectorIndex() * dy;
			break;
		}
		case matrix: { 
			int numCols = fallback(constr.getVectorArrangementPar1(), 5);
			int dx = fallback(constr.getVectorArrangementPar2(), spacing); 
			int dy = fallback(constr.getVectorArrangementPar3(), spacing); 
			int index = constr.getVectorIndex();
			x = (index % numCols)*dx;
			y = (index / numCols)*dy;
			break;
		}
		case ring: {
	        int rx = fallback(constr.getVectorArrangementPar1(), (int)(spacing*constr.getVectorSize()/(2*Math.PI)));
	        int ry = fallback(constr.getVectorArrangementPar2(), rx);
			double alpha = constr.getVectorIndex() * 2 * Math.PI / constr.getVectorSize();
	        x = (int)(rx - rx*Math.sin(alpha));
	        y = (int)(ry - ry*Math.cos(alpha));
			break;
		}
		default: { 
			throw new AssertionFailedException("unhandled vector arrangement " + constr.getVectorArrangement());
		}
		}
		return new Point(x, y);
	}

	private static int fallback(int vectorArrangementPar, int fallbackValue) {
		return vectorArrangementPar == Integer.MIN_VALUE ? fallbackValue : vectorArrangementPar;
	}

	/**
     * Implements the algorithm to layout the components of the given container figure.
     * 
     * @see LayoutManager#layout(IFigure)
     */
    @SuppressWarnings("unchecked")
    public void layout(IFigure parent) {
	    long startTime = System.currentTimeMillis();
	    
	    // create and run the layouter
	    AbstractGraphLayoutAlgorithm alg = createAutoLayouter();
	    alg.setSeed((int)algSeed);
	    alg.execute();

	    // store back the new seed - otherwise unpinned modules created one-by-one would pop up at the exact same place
	    algSeed = alg.getSeed();

    	// lay out the children according to the auto-layouter
        for (IFigure f : (List<IFigure>)parent.getChildren()) {
        	// get the computed location from the auto-layout algorithm (if there is an algorithm at all)
        	ISubmoduleConstraint constr = (ISubmoduleConstraint)f;
        	Point locationFromAlg = alg.getNodePosition(f);
        	Assert.isNotNull(locationFromAlg);
        
        	// we write back the calculated locations
        	constr.setCenterLocation(locationFromAlg);
        }
        if (debug)
            Debug.println("CompoundModuleLayout: " + (System.currentTimeMillis()-startTime) + "ms");
        System.out.println("layout()");
    }

	/**
	 * Set a new seed for the layouting algorithm. The next layout will use this seed.
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
	 * Pinned nodes will stay, unpinned nodes will move. 
	 * Nodes with unspecified location will be layouted by generating a random 
	 * starting position. 
	 */
	@SuppressWarnings("unchecked")
	public void requestFullLayout() {
		// forget all cached coordinates 
	    IFigure nodeParent = compoundFigure.getSubmoduleLayer(); 
		for (IFigure node : (List<IFigure>)nodeParent.getChildren()) {
			ISubmoduleConstraint constr = (ISubmoduleConstraint)node;
			constr.setCenterLocation(null);
		}
	}

	// XXX this algorithm is using ONLY the figure bounds. It might be much better to move
	// it to submoduleLayer.getPreferredSize

	@SuppressWarnings("unchecked")
	@Override
	protected Dimension calculatePreferredSize(IFigure f, int hHint, int wHint) {
		// if we have a size hint, we always use that for the size
		if (hHint >= 0 && wHint >= 0)
			return new Dimension(hHint,wHint);
	
		layout(f);  //XXX maybe not always needed ??
	    Rectangle rect = null;
	    for (IFigure child : (List<IFigure>)f.getChildren()) {
	        Rectangle r = child.getBounds();
	        
	        if (rect == null)
	        	rect = r.getCopy();
	        else
	        	rect.union(r);
	        
	        if (child instanceof SubmoduleFigure) {
	            // add the label bounds to it
	            rect.union(((SubmoduleFigure)child).getNameBounds());
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
}
