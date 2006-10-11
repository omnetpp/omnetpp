package org.omnetpp.ned.editor.graph.edit;

import org.eclipse.draw2d.ConnectionAnchor;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.geometry.Point;
import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.figures.GateAnchor;
import org.omnetpp.figures.SubmoduleFigure;
import org.omnetpp.figures.layout.SubmoduleConstraint;
import org.omnetpp.ned2.model.SubmoduleNodeEx;
import org.omnetpp.resources.NEDResources;
import org.omnetpp.resources.NEDResourcesPlugin;


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
    	SubmoduleNodeEx submNode = (SubmoduleNodeEx)getModel();
    	
    	// set module name and vector size
    	String nameToDisplay = submNode.getName();
    	// add [size] if it's a module vector
    	if (submNode.getVectorSize() != null && !"".equals(submNode.getVectorSize()))
    		nameToDisplay += "["+submNode.getVectorSize()+"]";
    	getSubmoduleFigure().setName(nameToDisplay);

    	// parse a dispaly string, so it's easier to get values from it.
    	// for other visula properties
        DisplayString dps = submNode.getDisplayString();
        // get a fallback display string for the module type
        DisplayString fallbackDps = NEDResourcesPlugin.getNEDResources()
        								.getEffectiveDisplayString(submNode.getType());
        dps.setDefaults(fallbackDps);
        
        // get the scale factor for this submodule (coming from the containing compound module's displaystring)
        float scale = getScale();
        // set it in the figure, so size and range indicator can use it
        getSubmoduleFigure().setScale(scale);
        // set the layout constraint for the figure (should be set BEFORE figure.setDisplayString() )
        // along with the scaling factor coming from the compound module
        SubmoduleConstraint constr = new SubmoduleConstraint(dps, scale);
        constr.setVectorName(nameToDisplay);
        // FIXME put the correct values here from the model
        constr.setVectorSize(5);
        constr.setVectorIndex(3);
        getSubmoduleFigure().setConstraint(constr);

        // set the rest of the dispay properties
        getSubmoduleFigure().setDisplayString(dps);

        // TODO implement a separate PIN decoration decorator figure in submodule figure
        if (dps.getLocation(scale) != null)
        	getSubmoduleFigure().setImageDecoration(ImageFactory.getImage(ImageFactory.DEFAULT_PIN));
        
    }

    public float getScale() {
        // get the container compound module's scaling factor
        return ((CompoundModuleEditPart)getParent()).getScale();
    }
    
}
