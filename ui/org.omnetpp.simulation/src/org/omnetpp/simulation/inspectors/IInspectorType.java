package org.omnetpp.simulation.inspectors;

import org.omnetpp.simulation.canvas.IInspectorContainer;
import org.omnetpp.simulation.model.cObject;

/**
 *
 * @author Andras
 */
public interface IInspectorType {   //XXX could be defined as: an IScorable plus a className in the extension point

    boolean supports(cObject object);

    int getScore(cObject object);

    IInspectorPart create(InspectorDescriptor descriptor, IInspectorContainer parent, cObject object);
}
