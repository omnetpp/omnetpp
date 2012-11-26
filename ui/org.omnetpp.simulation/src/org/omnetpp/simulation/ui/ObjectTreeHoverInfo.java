package org.omnetpp.simulation.ui;

import org.omnetpp.common.ui.HoverInfo;
import org.omnetpp.simulation.canvas.IInspectorContainer;

/**
 *
 * @author Andras
 */
public class ObjectTreeHoverInfo  extends HoverInfo {
    public ObjectTreeHoverInfo(Object input, IInspectorContainer canvas) {
        super(ObjectTreeInformationControl.getCreatorFor(canvas), input);
    }
}

