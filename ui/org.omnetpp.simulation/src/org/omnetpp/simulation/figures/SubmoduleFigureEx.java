package org.omnetpp.simulation.figures;

import org.omnetpp.figures.SubmoduleFigure;

public class SubmoduleFigureEx extends SubmoduleFigure {
    protected int moduleID;

    public int getModuleID() {
        return moduleID;
    }

    public void setModuleID(int moduleID) {
        this.moduleID = moduleID;
    }

    public void setSelectionBorderShown(boolean b) {
        setBorder(b ? new SelectionBorder() : null); //XXX for now
    }

}
