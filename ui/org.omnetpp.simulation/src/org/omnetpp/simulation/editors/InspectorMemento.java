package org.omnetpp.simulation.editors;

import org.eclipse.draw2d.geometry.Rectangle;

/**
 * Information to restore an inspector in another simulation session.
 *
 * @author Andras
 */
//TODO the inspectors themselves should be able to contribute to the memento; potentially IMemento/XMLMemento can be useful here
public class InspectorMemento {
    String objectFullPath;
    String objectClassName;
    String inspectorId;   // inspector type id
    Rectangle bounds;

    public InspectorMemento(String objectFullPath, String objectClassName, String inspectorId, Rectangle bounds) {
        this.objectFullPath = objectFullPath;
        this.objectClassName = objectClassName;
        this.inspectorId = inspectorId;
        this.bounds = bounds;
    }

    @Override
    public String toString() {
        return inspectorId + "@(" + objectClassName + ")" + objectFullPath;
    }
}