/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.parts;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import org.eclipse.gef.ConnectionEditPart;
import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPartViewer;
import org.eclipse.gef.SnapToGrid;

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

    protected static boolean getPropertyAsBool(EditPartViewer viewer, String propertyName) {
        Object prop = viewer.getProperty(propertyName);
        return prop instanceof Boolean && ((Boolean)prop).booleanValue();
    }

    protected static float getPropertyAsFloat(EditPartViewer viewer, String propertyName, float defaultValue) {
        Object prop = viewer.getProperty(propertyName);
        return prop instanceof Number ? ((Number)prop).floatValue() : defaultValue;
    }

    public static boolean isSnapToGridEnabled(EditPartViewer viewer) {
        return getPropertyAsBool(viewer, SnapToGrid.PROPERTY_GRID_ENABLED);
    }

    public static boolean isSnapToGridVisible(EditPartViewer viewer) {
        return getPropertyAsBool(viewer, SnapToGrid.PROPERTY_GRID_VISIBLE);
    }

    public static float getSnapToGridSpacing(EditPartViewer viewer) {
        return getPropertyAsFloat(viewer, SnapToGrid.PROPERTY_GRID_SPACING, 10.0f);
    }
}
