package org.omnetpp.ned.editor.graph.edit;

import org.eclipse.draw2d.IFigure;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.figures.SubmoduleFigure;
import org.omnetpp.ned2.model.INamedGraphNode;

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
        
    }
    
}
