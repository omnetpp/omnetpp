/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.actions;

import org.eclipse.gef.GraphicalViewer;
import org.eclipse.gef.SnapToGrid;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.resource.ImageDescriptor;
import org.omnetpp.ned.editor.NedEditorPlugin;
import org.omnetpp.ned.editor.graph.parts.EditPartUtil;

/**
 * Toggle Snap to Grid action.
 *
 * @author andras
 */
public class ToggleSnapToGridAction extends Action {

    public static final String ID = "ToggleSnapToGrid";
    public static final String MENUNAME = "Snap to Grid";
    public static final String TOOLTIP = "Snap to Grid";
    public static final ImageDescriptor IMAGE = NedEditorPlugin.getImageDescriptor("icons/full/etool16/snaptogrid.png");

    private GraphicalViewer diagramViewer;


    public ToggleSnapToGridAction(GraphicalViewer diagramViewer) {
        super("Snap to Grid", AS_CHECK_BOX);
        this.diagramViewer = diagramViewer;
        setId(ID);
        setImageDescriptor(IMAGE);
        setChecked(isSnapEnabled());
    }

    private boolean isSnapEnabled() {
        return EditPartUtil.isSnapToGridEnabled(diagramViewer);
    }

    private void setSnapEnabled(boolean enabled) {
        NedEditorPlugin.getDefault().getPreferenceStore().setValue(SnapToGrid.PROPERTY_GRID_ENABLED, enabled);
        NedEditorPlugin.getDefault().getPreferenceStore().setValue(SnapToGrid.PROPERTY_GRID_VISIBLE, enabled);
        diagramViewer.setProperty(SnapToGrid.PROPERTY_GRID_ENABLED, Boolean.valueOf(enabled));
        diagramViewer.setProperty(SnapToGrid.PROPERTY_GRID_VISIBLE, Boolean.valueOf(enabled));
    }

    @Override
    public void run() {
        setSnapEnabled(!isSnapEnabled());
        setChecked(isSnapEnabled());
    }

}
