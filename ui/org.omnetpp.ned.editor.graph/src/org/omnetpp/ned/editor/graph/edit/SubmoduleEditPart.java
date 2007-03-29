package org.omnetpp.ned.editor.graph.edit;

import java.util.List;

import org.eclipse.draw2d.ConnectionAnchor;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.geometry.Point;
import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.figures.SubmoduleFigure;
import org.omnetpp.figures.layout.SubmoduleConstraint;
import org.omnetpp.figures.misc.GateAnchor;
import org.omnetpp.ned.model.NEDElement;
import org.omnetpp.ned.model.ex.SubmoduleNodeEx;


public class SubmoduleEditPart extends ModuleEditPart {
    protected GateAnchor gateAnchor;

    @Override
    protected void createEditPolicies() {
        super.createEditPolicies();
    }
    /** 
     * Returns a newly created Figure of this.
     * 
     * @return A new Figure of this.
     */
    @Override
    protected IFigure createFigure() {
        IFigure fig = new SubmoduleFigure();
        gateAnchor = new GateAnchor(fig);
        return fig;
    }

    /**
     * Returns the Figure for this as an SubmoduleFigure.
     * 
     * @return Figure of this as a SubmoduleFigure
     */
    public SubmoduleFigure getSubmoduleFigure() {
        return (SubmoduleFigure)getFigure();
    }

    /**
     * @return Helper function to return the model object with correct type
     */
    public SubmoduleNodeEx getSubmoduleModel() {
        return (SubmoduleNodeEx)getModel();
    }
    
	/**
	 * Compute the source connection anchor to be assigned based on the current mouse 
	 * location and available gates. 
	 * @param p current mouse coordinates
	 * @return The selected connection anchor
	 */
	public ConnectionAnchor getConnectionAnchorAt(Point p) {
        return gateAnchor;
	}

	/**
	 * Returns a conn anchor registered for the given gate
	 * @param gate
	 * @return
	 */
	public GateAnchor getConnectionAnchor(String gate) {
        return gateAnchor;
	}

    /**
     * Returns a list of connections for which this is the srcModule.
     * 
     * @return List of connections.
     */
    @Override
    protected List getModelSourceConnections() {
        // get the connections from out controller parent's model
        return getCompoundModulePart().getCompoundModuleModel().getSrcConnectionsFor(getSubmoduleModel().getName());
    }

    /**
     * Returns a list of connections for which this is the destModule.
     * 
     * @return List of connections.
     */
    @Override
    protected List getModelTargetConnections() {
        // get the connections from out controller parent's model
        return getCompoundModulePart().getCompoundModuleModel().getDestConnectionsFor(getSubmoduleModel().getName());
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
        DisplayString dps = submNode.getEffectiveDisplayString();
        
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
    
    public boolean isEditable() {
        // editable only if the parent controllers model is the same as the model's parent
        // ie the submodule is defined in this compound module (not inherited)
        return super.isEditable() && 
                (getParent().getModel() == ((SubmoduleNodeEx)getModel()).getCompoundModule());
    }

    @Override
    public CompoundModuleEditPart getCompoundModulePart() {
        return (CompoundModuleEditPart)getParent();
    }

    /* (non-Javadoc)
     * @see org.omnetpp.ned.editor.graph.edit.BaseEditPart#getTypeNameForDblClickOpen()
     * open the type of component for double click
     */
    @Override
    protected NEDElement getNEDElementToOpen() {
        return getSubmoduleModel().getEffectiveTypeRef();
    }
}
