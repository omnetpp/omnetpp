package org.omnetpp.figures.layout;

import java.util.List;

import org.eclipse.draw2d.Connection;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.LayoutManager;
import org.eclipse.draw2d.XYLayout;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;

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
	    IFigure nodeParent = compoundFigure.getSubmoduleContainer(); 
	    IFigure edgeParent = compoundFigure.getConnectionContainer();
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
		//autoLayouter.setConfineToArea(width, height, 50);
		autoLayouter.setScaleToArea(width, height, 50);

		// iterate over the nodes and add them to the algorithm
		// all child figures on this layer are considered as node
		for (IFigure node : (List<IFigure>)nodeParent.getChildren()) {
			// get the associated constraint (coordinates) if any
            Rectangle constr = (Rectangle)getConstraint(node);

            if (constr == null || constr.x == Integer.MIN_VALUE && constr.y == Integer.MIN_VALUE) {
            	autoLayouter.addMovableNode(node, node.getBounds().x, node.getBounds().y, node.getPreferredSize().width, node.getPreferredSize().height);
            }
            else
            	// add as fixed node
            	autoLayouter.addFixedNode(node,
            			constr.x, constr.y,
            			constr.width, constr.height);
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
            Rectangle oldBounds = f.getBounds().getCopy();
            Rectangle constr = ((Rectangle)getConstraint(f)).getCopy();
            boolean isUnpinned = constr.x == Integer.MIN_VALUE && constr.y == Integer.MIN_VALUE;
            // first set the size of the figure from the provided constraint object
            f.setSize(constr.getSize());

            // calculate the new location
            Rectangle newBounds = f.getBounds().getCopy();
            // get the computed location from the auto-layout algorithm (if there is an algorithm at all)
            Point loc = alg != null ? alg.getNodePosition(f) : null;
            // set the location (if algorithm has calculated, get if from the algorithm
            // if the alg. was not and the module is pinned, simply use the coord from the constraint
            // if the module was unpinned, use the original bounds center (ie, keep the figuire's center
            // while changing its size
            newBounds.setLocation(loc != null ? loc : isUnpinned ? oldBounds.getCenter() : constr.getLocation());

            newBounds.translate(offset);
            // translate to the middle of the figure
            newBounds.translate(-newBounds.width / 2, -newBounds.height / 2);
            f.setBounds(newBounds);
        }
        if (debug)
            System.out.println("SpringEmbedderLayout: " + (System.currentTimeMillis()-startTime) + "ms");
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
	 * After calling this, the next layout process will call an auto layout process
	 */
	public void requestAutoLayout() {
	    requestAutoLayout = true;
	}
	
	@Override
	protected Dimension calculatePreferredSize(IFigure f, int hint, int hint2) {
	    if (f.getChildren().size() == 0)
	        return new Dimension(DEFAULT_SIZE);
	    Rectangle rect = new Rectangle();
	    for(IFigure child : (List<IFigure>)f.getChildren()) {
	        Rectangle r = (Rectangle)constraints.get(child);
	        if (r == null)
	            continue;
	        
	        // translate the x,y as we use the center of the figure in the constraint
	        r = r.getTranslated(-r.width/2, -r.height/2);

	        if (r.width == -1 || r.height == -1) {
	            Dimension preferredSize = child.getPreferredSize(r.width, r.height);
	            r = r.getCopy();
	            if (r.width == -1)
	                r.width = preferredSize.width;
	            if (r.height == -1)
	                r.height = preferredSize.height;
	        }
	        
	        if (child instanceof SubmoduleFigure) {
	            // add the label bounds to it
	            r.union(((SubmoduleFigure)child).getLabelBounds());
	        }
	        rect.union(r);
	    }
	    
	    return new Dimension(rect.width + f.getInsets().getWidth(), rect.height + f.getInsets().getHeight()).
	        union(getBorderPreferredSize(f));
	}
}
