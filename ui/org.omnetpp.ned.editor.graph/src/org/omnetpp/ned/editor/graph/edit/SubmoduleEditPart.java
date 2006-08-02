package org.omnetpp.ned.editor.graph.edit;

import org.eclipse.draw2d.ConnectionAnchor;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.geometry.Point;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.figures.GateAnchor;
import org.omnetpp.figures.SubmoduleFigure;
import org.omnetpp.ned2.model.INamedGraphNode;


// TODO implement UnpinRequest 
public class SubmoduleEditPart extends ModuleEditPart {

    /** 
     * Returns a newly created Figure of this.
     * 
     * @return A new Figure of this.
     */
    @Override
    protected IFigure createFigure() {
        return new SubmoduleFigure();
    }

    /**
     * Returns the Figure for this as an SubmoduleFigure.
     * 
     * @return Figure of this as a SubmoduleFigure
     */
    protected SubmoduleFigure getSubmoduleFigure() {
        return (SubmoduleFigure) getFigure();
    }

	/**
	 * Compute the source connection anchor to be assigned based on the current mouse 
	 * location and available gates. 
	 * @param p current mouse coordinates
	 * @return The selected connection anchor
	 */
	public ConnectionAnchor getConnectionAnchorAt(Point p) {
		// TODO select the appropriate gate name automatically
		// or return NULL if no output gate is available
		return new GateAnchor(getFigure());
	}

	/**
	 * Returns a conn anchor registered for the given gate
	 * @param gate
	 * @return
	 */
	public GateAnchor getConnectionAnchor(String gate) {
		return new GateAnchor(getFigure());
	}

	/**
     * Updates the visual aspect of this.
     */
    @Override
    protected void refreshVisuals() {
        
        // define the properties that determine the visual appearence
    	INamedGraphNode model = (INamedGraphNode)getNEDModel();
    	
    	getSubmoduleFigure().setName(model.getName());
        // parse a dispaly string, so it's easier to get values from it.
        IDisplayString dps = model.getDisplayString();
        getSubmoduleFigure().setDisplayString(dps);
        // TODO implement a separate PIN decoration decorator figure in submodule figure
        if (dps.getLocation() != null)
        	getSubmoduleFigure().setImageDecoration(ImageFactory.getImage(ImageFactory.DEFAULT_PIN));
        
    }
    
    
}
