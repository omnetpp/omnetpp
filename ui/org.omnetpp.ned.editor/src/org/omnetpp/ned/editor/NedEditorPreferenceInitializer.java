/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor;

import org.eclipse.core.runtime.preferences.AbstractPreferenceInitializer;
import org.eclipse.jface.preference.IPreferenceStore;

public class NedEditorPreferenceInitializer extends AbstractPreferenceInitializer {
    public final static String SNAP_TO_GEOMETRY = "snapToGeometry";

    @Override
    public void initializeDefaultPreferences() {
        IPreferenceStore store = NedEditorPlugin.getDefault().getPreferenceStore();
        store.setDefault(SNAP_TO_GEOMETRY, true);
    }
}
