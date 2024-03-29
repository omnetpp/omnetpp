/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor;

import org.eclipse.core.runtime.preferences.AbstractPreferenceInitializer;
import org.eclipse.gef.SnapToGeometry;
import org.eclipse.gef.SnapToGrid;
import org.eclipse.jface.preference.IPreferenceStore;

public class NedEditorPreferenceInitializer extends AbstractPreferenceInitializer {
    @Override
    public void initializeDefaultPreferences() {
        IPreferenceStore store = NedEditorPlugin.getDefault().getPreferenceStore();
        store.setDefault(SnapToGeometry.PROPERTY_SNAP_ENABLED, true);
        store.setDefault(SnapToGrid.PROPERTY_GRID_ENABLED, false);
        store.setDefault(SnapToGrid.PROPERTY_GRID_VISIBLE, false);
        store.setDefault(SnapToGrid.PROPERTY_GRID_SPACING, 10);
    }
}
