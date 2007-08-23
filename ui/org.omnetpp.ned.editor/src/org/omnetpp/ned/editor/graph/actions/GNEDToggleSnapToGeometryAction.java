package org.omnetpp.ned.editor.graph.actions;

import org.eclipse.gef.GraphicalViewer;
import org.eclipse.gef.SnapToGeometry;
import org.eclipse.gef.internal.GEFMessages;
import org.eclipse.gef.ui.actions.GEFActionConstants;
import org.eclipse.jface.action.Action;

/**
 * Re-implementation of GEF ToggleSnapToGeometryAction to toggle the PROPERTY_SNAP_ENABLED property
 * on the graphical viewer. The GEF version had a bug as it never called setChecked on the action
 * resulting in missing notification. This caused the contributor items to get out of sync.
 * @author rhornig
 */
public class GNEDToggleSnapToGeometryAction
    extends Action
{
    private GraphicalViewer diagramViewer;

    /**
     * Constructor
     *
     * @param diagramViewer
     *            the GraphicalViewer whose snap to geometry property is to be
     *            toggled
     */
    public GNEDToggleSnapToGeometryAction(GraphicalViewer diagramViewer) {
        super(GEFMessages.ToggleSnapToGeometry_Label, AS_CHECK_BOX);
        this.diagramViewer = diagramViewer;
        setToolTipText(GEFMessages.ToggleSnapToGeometry_Tooltip);
        setId(GEFActionConstants.TOGGLE_SNAP_TO_GEOMETRY);
        setActionDefinitionId(GEFActionConstants.TOGGLE_SNAP_TO_GEOMETRY);
        setChecked(isSnapEnabled());
    }

    private boolean isSnapEnabled() {
        Boolean val = (Boolean)diagramViewer.getProperty(SnapToGeometry.PROPERTY_SNAP_ENABLED);
        if (val != null)
            return val.booleanValue();
        return false;
    }

    private void setSnapEnabled(boolean enabled) {
        diagramViewer.setProperty(SnapToGeometry.PROPERTY_SNAP_ENABLED, new Boolean(enabled));
    }

    @Override
    public void run() {
        setSnapEnabled(!isSnapEnabled());
        setChecked(isSnapEnabled());
    }

}
