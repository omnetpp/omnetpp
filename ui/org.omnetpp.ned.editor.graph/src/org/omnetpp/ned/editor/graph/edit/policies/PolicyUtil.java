package org.omnetpp.ned.editor.graph.edit.policies;

import org.eclipse.gef.EditPart;
import org.omnetpp.ned.editor.graph.edit.IReadOnlySupport;

/**
 * @author rhornig
 * Static helper functions for policy classes
 */
public class PolicyUtil {

    /**
     * @param part
     * @return whtether the given edit part is editable or not
     */
    public static boolean isEditable(EditPart part) {
        if (part instanceof IReadOnlySupport) 
            return ((IReadOnlySupport)part).isEditable();
        // by default it is editable
        return true;
    }
}
