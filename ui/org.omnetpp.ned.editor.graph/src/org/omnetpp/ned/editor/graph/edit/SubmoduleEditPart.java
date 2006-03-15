package org.omnetpp.ned.editor.graph.edit;

import org.eclipse.draw2d.IFigure;
import org.omnetpp.ned.editor.graph.figures.SimpleModuleFigure;

public class SubmoduleEditPart extends NedNodeEditPart {

    /**
     * Returns a newly created Figure of this.
     * 
     * @return A new Figure of this.
     */
    protected IFigure createFigure() {
        
        return new SimpleModuleFigure();
    }

    /**
     * Returns the Figure for this as an SimpleModuleFigure.
     * 
     * @return Figure of this as a SimpleModuleFigure
     */
    protected SimpleModuleFigure getSimpleModuleFigure() {
        return (SimpleModuleFigure) getFigure();
    }

}
