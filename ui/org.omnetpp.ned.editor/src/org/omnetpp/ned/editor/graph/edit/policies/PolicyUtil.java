package org.omnetpp.ned.editor.graph.edit.policies;

import org.eclipse.gef.EditPart;
import org.omnetpp.ned.editor.graph.edit.IReadOnlySupport;

/**
 * Static helper functions for policy classes
 * @author rhornig
 */
public class PolicyUtil {

    /**
     * Returns whether the given edit part is editable
     */
    public static boolean isEditable(EditPart part) {
        if (part instanceof IReadOnlySupport)
            return ((IReadOnlySupport)part).isEditable();
        // by default it is editable
        return true;
    }
}
