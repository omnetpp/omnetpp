package org.omnetpp.simulation.figures;

import org.omnetpp.figures.SubmoduleFigure;

public class SubmoduleFigureEx extends SubmoduleFigure {

    public void setSelectionBorderShown(boolean b) {
        setBorder(b ? new SelectionBorder() : null); //XXX for now
    }

}
