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

	/**
	 * 
	 * @param nodeParent The parent figure of the nodes
	 * @param connectionParent The parent figure of the connection figures
	 */
	public SpringEmbedderLayout(IFigure nodeParent, IFigure edgeParent) {
		super();
		this.edgeParent = edgeParent;
 	}
	
	/**
	 * Calls the autoLayout algorithm using all curreny specified constraints. 
	 * @param nodeParent The Parent figure of the nodes
	 * @param edgeParent The parent figure of the connections (usually the ConnectionLayer)
	 */
	@SuppressWarnings("unchecked")
	protected AbstractGraphLayoutAlgorithm createAutoLayouter(IFigure nodeParent, IFigure edgeParent) {
		AbstractGraphLayoutAlgorithm autoLayouter = new BasicSpringEmbedderLayoutAlgorithm();
		autoLayouter.setDefaultEdgeLength(40);
		autoLayouter.setScaleToArea(400, 400, 50);
		
		// iterate over the nodes and add them to the algorithm 
		// all child figures on this layer are considered as node
		for(IFigure node : (List<IFigure>)nodeParent.getChildren()) {
			// get the associated constraint (coordinates) if any
            Rectangle constr = (Rectangle)getConstraint(node);
            // if not found add it as freely movable node
            // FIXME onpinned nodes still have a constrains (should be removed in the controller)
            if (constr == null || constr.x == 0) 
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
					autoLayouter.addEdgeToBorder(targetFig, 20);
				} // else if this is an edge goung out from a submodule
				else if (targetFig instanceof CompoundModuleFigure) {
					autoLayouter.addEdgeToBorder(srcFig, 20);
				} else {  // both are submodules
					autoLayouter.addEdge(srcFig, targetFig, 20);
				}
				
			}
		
		
		return autoLayouter;
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
    	AbstractGraphLayoutAlgorithm alg = createAutoLayouter(parent, edgeParent);
    	alg.setDefaultEdgeLength(20);
    	// execute the algorithm 
    	alg.execute();
    	
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
