package org.omnetpp.ned.editor.graph.edit.policies;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.gef.EditPart;
import org.eclipse.gef.Request;
import org.eclipse.gef.requests.GroupRequest;
import org.omnetpp.ned.editor.graph.edit.IReadOnlySupport;

/**
 * @author rhornig
 * Static helper functions for policy classes
 */
public class PolicyUtil {

    /**
     * Filters out all read only parts from a GroupRequest
     * @param req 
     */
//    @SuppressWarnings("unchecked")
//    public static void filterOutReadOnlyParts(Request req) {
//        if (!(req instanceof GroupRequest))
//            return;
//        List filteredParts = new ArrayList();
//        for (Object part : ((GroupRequest)req).getEditParts()) {
//            if (part instanceof IReadOnlySupport && !((IReadOnlySupport)part).isEditable())
//                continue;
//            // otherwise add it to the list
//            filteredParts.add(part);
//        }
//        ((GroupRequest)req).setEditParts(filteredParts);
//    }
    
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
