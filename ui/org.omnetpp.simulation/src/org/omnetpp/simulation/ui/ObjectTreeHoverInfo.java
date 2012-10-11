package org.omnetpp.simulation.ui;

import org.omnetpp.common.ui.HoverInfo;

/**
 *
 * @author Andras
 */
public class ObjectTreeHoverInfo  extends HoverInfo {
    public ObjectTreeHoverInfo(Object input) {
        super(ObjectTreeInformationControl.getCreator(), input);
    }
}

