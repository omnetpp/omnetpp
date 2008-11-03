package org.omnetpp.ned.editor.graph.actions;

import org.eclipse.gef.GraphicalViewer;
import org.eclipse.gef.SnapToGeometry;
import org.eclipse.gef.internal.GEFMessages;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.resource.ImageDescriptor;
import org.omnetpp.ned.editor.NedEditorPlugin;
import org.omnetpp.ned.editor.NedEditorPreferenceInitializer;

/**
 * Re-implementation of GEF ToggleSnapToGeometryAction to toggle the PROPERTY_SNAP_ENABLED property
 * on the graphical viewer. The GEF version had a bug as it never called setChecked on the action
 * resulting in missing notification. This caused the contributor items to get out of sync.
 * 
 * See https://bugs.eclipse.org/bugs/show_bug.cgi?id=130728
 * 
 * @author rhornig
 */
public class GNEDToggleSnapToGeometryAction extends Action {

    public static final String ID = "ToggleSnap";
    public static final String MENUNAME = GEFMessages.ToggleSnapToGeometry_Label;
    public static final String TOOLTIP = GEFMessages.ToggleSnapToGeometry_Tooltip;
    public static final ImageDescriptor IMAGE = NedEditorPlugin.getImageDescriptor("icons/full/etool16/snaptogeometry.png");

    private GraphicalViewer diagramViewer;
    

    /**
     * Constructor
     *
     * @param diagramViewer
     *            the GraphicalViewer whose snap to geometry property is to be
     *            toggled
     */
    public GNEDToggleSnapToGeometryAction(GraphicalViewer diagramViewer) {
        super(MENUNAME, AS_CHECK_BOX);
        this.diagramViewer = diagramViewer;
        setToolTipText(TOOLTIP);
        setId(ID);
		setImageDescriptor(IMAGE);
        setChecked(isSnapEnabled());
    }

    private boolean isSnapEnabled() {
        return (Boolean)diagramViewer.getProperty(SnapToGeometry.PROPERTY_SNAP_ENABLED);
    }

    private void setSnapEnabled(boolean enabled) {
        NedEditorPlugin.getDefault().getPreferenceStore().setValue(NedEditorPreferenceInitializer.SNAP_TO_GEOMETRY, enabled);
        diagramViewer.setProperty(SnapToGeometry.PROPERTY_SNAP_ENABLED, new Boolean(enabled));
    }

    @Override
    public void run() {
        setSnapEnabled(!isSnapEnabled());
        setChecked(isSnapEnabled());
    }

}
