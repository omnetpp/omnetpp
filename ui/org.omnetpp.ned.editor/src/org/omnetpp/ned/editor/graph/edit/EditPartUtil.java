package org.omnetpp.ned.editor.graph.edit;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import org.eclipse.gef.ConnectionEditPart;
import org.eclipse.gef.EditPart;

/**
 * Utility functions for editparts.
 * 
 * @author rhornig, andras
 */
public class EditPartUtil {
    /**
     * Returns whether the given object is editable
     */
    public static boolean isEditable(Object part) {
        if (part instanceof IReadOnlySupport)
            return ((IReadOnlySupport)part).isEditable();
        // by default it is editable
        return true;
    }


	/**
	 * Returns a collection of all collection editparts that
	 * whose both ends are among the selected editparts.
	 */
	@SuppressWarnings("unchecked")
	public static List<ConnectionEditPart> getAttachedConnections(List<EditPart> editParts) {
		// collect selected objects. 
		// Note: getSelectedObjects() seems to return the editparts not the model objects
		List<ConnectionEditPart> result = new ArrayList<ConnectionEditPart>();

		// extend selection with connections among the selected submodules (and their compound modules).
		// first, collect modules and submodules:
		Set<ModuleEditPart> moduleEditParts = new HashSet<ModuleEditPart>();
		for (EditPart editPart : editParts)
			if (editPart instanceof ModuleEditPart)
				moduleEditParts.add((ModuleEditPart)editPart);

		// then collect connections
		for (ModuleEditPart srcModuleEditPart : moduleEditParts)
			for (ConnectionEditPart conn : (List<ConnectionEditPart>)srcModuleEditPart.getSourceConnections())
				if (moduleEditParts.contains(conn.getTarget()))
					result.add(conn);
		return result;
	}

}
