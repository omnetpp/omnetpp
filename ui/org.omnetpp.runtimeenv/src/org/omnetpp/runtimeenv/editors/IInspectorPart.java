package org.omnetpp.runtimeenv.editors;

import org.eclipse.jface.viewers.ISelectionProvider;


/**
 * Represents an inspector.
 * 
 * The inspector is also a selection provider. When this inspector is selected 
 * (see isSelected()), the selection should include this inspector (or its 
 * inspected object) as well. 
 */
public interface IInspectorPart extends ISelectionProvider {

    /**
     * Returns the corresponding figure.
     */
    IInspectorFigure getFigure();
    
    /**
     * Returns true if this inspector is selected.
     */
    boolean isSelected();

    /**
     * Sets the selected state of this inspector.
     */
    void setSelected(boolean b);
}
