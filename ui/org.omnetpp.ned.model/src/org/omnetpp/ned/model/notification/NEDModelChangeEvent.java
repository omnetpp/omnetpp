package org.omnetpp.ned.model.notification;

import org.omnetpp.ned.model.INEDElement;

/**
 * Common base for all events that actually change the ned element tree
 * @author rhornig
 */
public class NEDModelChangeEvent extends NEDModelEvent {

    public NEDModelChangeEvent(INEDElement source) {
        super(source);
    }

}
