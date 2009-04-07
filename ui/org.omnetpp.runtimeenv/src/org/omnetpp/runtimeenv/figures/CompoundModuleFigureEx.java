package org.omnetpp.runtimeenv.figures;

import org.eclipse.draw2d.LineBorder;
import org.eclipse.draw2d.MarginBorder;
import org.omnetpp.figures.CompoundModuleFigure;
import org.omnetpp.runtimeenv.editors.GraphicalModulePart;

//FIXME draw proper resize handles, and make the mouse listener recognize it
public class CompoundModuleFigureEx extends CompoundModuleFigure {
    protected GraphicalModulePart modulePart;

    public CompoundModuleFigureEx() {
        setBorder(new MarginBorder(5));
    }
    
    public void setDragHandlesShown(boolean b) {
        setBorder(b ? new LineBorder(5) : new MarginBorder(5)); //XXX for now
    }

    public boolean getDragHandlesShown() {
        return getBorder() instanceof LineBorder; //XXX for now
    }

    public GraphicalModulePart getModulePart() {
        return modulePart;
    }

    public void setModulePart(GraphicalModulePart modulePart) {
        this.modulePart = modulePart;
    }
}
