package org.omnetpp.ned.editor.graph.edit;

import java.util.List;

import org.eclipse.core.resources.IFile;
import org.eclipse.draw2d.ConnectionAnchor;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.gef.Request;
import org.eclipse.gef.RequestConstants;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.part.FileEditorInput;
import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.figures.GateAnchor;
import org.omnetpp.figures.SubmoduleFigure;
import org.omnetpp.figures.layout.SubmoduleConstraint;
import org.omnetpp.ned.editor.graph.GraphicalNedEditor;
import org.omnetpp.ned.editor.graph.misc.ISelectionSupport;
import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.ex.SubmoduleNodeEx;
import org.omnetpp.ned2.model.interfaces.INEDTypeInfo;


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
     * Returns a list of connections for which this is the srcModule.
     * 
     * @return List of connections.
     */
    @Override
    protected List getModelSourceConnections() {
        return getSubmoduleModel().getCompoundModule().getSrcConnectionsFor(getSubmoduleModel());
    }

    /**
     * Returns a list of connections for which this is the destModule.
     * 
     * @return List of connections.
     */
    @Override
    protected List getModelTargetConnections() {
        return getSubmoduleModel().getCompoundModule().getDestConnectionsFor(getSubmoduleModel());
    }

    @Override
    public void attributeChanged(NEDElement node, String attr) {
        super.attributeChanged(node, attr);
        // refresh only ourselves. Child changes do not change the apperaence
        if (node == getModel()) 
            refreshVisuals();

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
    
    public boolean isSelectable() {
        // TODO this is not a correct solution because we cannot connect these modules via connections
        // however this feature is required

        // selection should be allowed ONLY if the submodule is defined in the current compound module
        // if we inherited the submodule, we should not allow selection (and editing in this module)
        // we own the submodule if the submodule is declared in the current compound module
        // ie. our parent controllers model is the same object as our model's parent
//        return getParent().getModel() == ((SubmoduleNodeEx)getModel()).getCompoundModule();
        return true;
    }

    @Override
    public CompoundModuleEditPart getCompoundModulePart() {
        return (CompoundModuleEditPart)getParent();
    }

    @Override
    public void performRequest(Request req) {
        super.performRequest(req);
        // lets open or activate a new editor if somone has double clicked the submodule
        if (RequestConstants.REQ_OPEN.equals(req.getType())) {
            INEDTypeInfo typeInfo = getSubmoduleModel().getTypeNEDTypeInfo();
            if (typeInfo == null)
                return;
            IFile file = typeInfo.getNEDFile();
            IFileEditorInput fileEditorInput = new FileEditorInput(file);

            try {
                IEditorPart editor = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage()
                    .openEditor(fileEditorInput, GraphicalNedEditor.ID, true);
                
                // select the component so it will be visible in the opened editor
                if (editor instanceof ISelectionSupport)
                    ((ISelectionSupport)editor).selectComponent(typeInfo.getName());
                
            } catch (PartInitException e) {
                // TODO check if this can really happen?
                e.printStackTrace();
            }
        }
    }
}
