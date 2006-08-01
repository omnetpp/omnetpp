package org.omnetpp.figures.layout;

import java.util.Iterator;
import java.util.List;

import org.eclipse.draw2d.Connection;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.LayoutManager;
import org.eclipse.draw2d.XYLayout;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.figures.CompoundModuleFigure;


public class SpringEmbedderLayout extends XYLayout {

	protected IFigure edgeParent;
	protected IFigure nodeParent;
	protected AbstractGraphLayoutAlgorithm alg;
	/**
	 * 
	 * @param nodeParent The parent figure of the nodes
	 * @param connectionParent The parent figure of the connection figures
	 */
	public SpringEmbedderLayout(IFigure nodeParent, IFigure edgeParent) {
		super();
		this.nodeParent = nodeParent;
		this.edgeParent = edgeParent;
 	}
	
	/**
	 * Calls the autoLayout algorithm using all curreny specified constraints. 
	 * @param nodeParent The Parent figure of the nodes
	 * @param edgeParent The parent figure of the connections (usually the ConnectionLayer)
	 */
	@SuppressWarnings("unchecked")
	protected AbstractGraphLayoutAlgorithm createAutoLayouter(IFigure nodeParent, IFigure edgeParent) {
		BasicSpringEmbedderLayoutAlgorithm autoLayouter = new BasicSpringEmbedderLayoutAlgorithm();
		autoLayouter.setDefaultEdgeLength(100);
		autoLayouter.setRepulsiveForce(300.0);
		autoLayouter.setMaxIterations(500);
		autoLayouter.setSeed(12345);
		// FIXME set the correct area size
		autoLayouter.setConfineToArea(400,200, 50);
//		autoLayouter.setConfineToArea(nodeParent.getSize().width, 
//				                      nodeParent.getSize().height, 50);
		
		// iterate over the nodes and add them to the algorithm 
		// all child figures on this layer are considered as node
		for(IFigure node : (List<IFigure>)nodeParent.getChildren()) {
			// get the associated constraint (coordinates) if any
            Rectangle constr = (Rectangle)getConstraint(node);
            // skip the node if it dos not have a constraint
//            if(constr == null) {
//            	System.out.println(node);
//            	continue;
//            }
            
            if (constr.x == Integer.MIN_VALUE && constr.y == Integer.MIN_VALUE) 
            	autoLayouter.addMovableNode(node, node.getPreferredSize().width, node.getPreferredSize().height);
            else
            	// add as foxed node
            	autoLayouter.addFixedNode(node, 
            			constr.x, constr.y,
            			constr.width, constr.height);
		}
		
		// iterate over the connections and add them to the algorithm 
		// all child figures of type Connection on this layer are considered as edge
		for(IFigure edge : (List<IFigure>)edgeParent.getChildren())
			if (edge instanceof Connection) {
				Connection conn = (Connection)edge;
				IFigure srcFig = conn.getSourceAnchor().getOwner();
				IFigure targetFig = conn.getTargetAnchor().getOwner();
				// if this is an edge coming from outside to a submodule
				if (srcFig instanceof CompoundModuleFigure) {
					autoLayouter.addEdgeToBorder(targetFig, 0);
				} // else if this is an edge goung out from a submodule
				else if (targetFig instanceof CompoundModuleFigure) {
					autoLayouter.addEdgeToBorder(srcFig, 0);
				} else {  // both are submodules
					autoLayouter.addEdge(srcFig, targetFig, 0);
				}
				
			}
		
		
		return autoLayouter;
	}

	@Override
	public void invalidate() {
		super.invalidate();
		alg = null;
	}
	
	public void initLayout() {
    	alg = createAutoLayouter(nodeParent, edgeParent);
    	// execute the algorithm 
    	alg.execute();
    	System.out.println("Layouting figure :"+nodeParent);
	}
	
	/**
     * Implements the algorithm to layout the components of the given container figure.
     * Each component is laid out using its own layout constraint specifying its size
     * and position. Copied from XYLayout BUT places the middlepoint if the children to the
     * specified constraint location.
     * 
     * @see LayoutManager#layout(IFigure)
     */
    @Override
    public void layout(IFigure parent) {
    	
    	if (alg == null)
    		initLayout();
    	
    	// lay out the children according to the autolayouter
        Iterator children = parent.getChildren().iterator();
        Point offset = getOrigin(parent);
        IFigure f;
        while (children.hasNext()) {
            f = (IFigure)children.next();
            Rectangle bounds = null;
//            bounds = (Rectangle)getConstraint(f);
            // if there is no constraint
            if (bounds == null) {
            	bounds = new Rectangle();
            	Point loc = alg.getNodePosition(f);
            	if (loc != null) {
            		bounds.setLocation(loc);
            		bounds.setSize(f.getPreferredSize());
            	} else
            	// not found in the algorithm result so do not layout this child
            		assert(false);
            }

            if (bounds.width == -1 || bounds.height == -1) {
                Dimension preferredSize = f.getPreferredSize(bounds.width, bounds.height);
                bounds = bounds.getCopy();
                if (bounds.width == -1)
                    bounds.width = preferredSize.width;
                if (bounds.height == -1)
                    bounds.height = preferredSize.height;
            }
            bounds = bounds.getTranslated(offset);
            // translate to the middle of the figure
            bounds.translate(-bounds.width / 2, -bounds.height / 2);
            f.setBounds(bounds);
        }
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

}
