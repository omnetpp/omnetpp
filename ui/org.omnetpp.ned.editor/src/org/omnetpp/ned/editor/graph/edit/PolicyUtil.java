package org.omnetpp.ned.editor.graph.edit;


/**
 * Static helper functions for policy classes
 *
 * @author rhornig
 */
public class PolicyUtil {

    /**
     * Returns whether the given object is editable
     */
    public static boolean isEditable(Object part) {
        if (part instanceof IReadOnlySupport)
            return ((IReadOnlySupport)part).isEditable();
        // by default it is editable
        return true;
    }
}
