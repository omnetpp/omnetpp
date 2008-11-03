package org.omnetpp.ned.editor;

import org.eclipse.core.runtime.preferences.AbstractPreferenceInitializer;
import org.eclipse.jface.preference.IPreferenceStore;

public class NedEditorPreferenceInitializer extends AbstractPreferenceInitializer {
    @Override
    public void initializeDefaultPreferences() {
        IPreferenceStore store = NedEditorPlugin.getDefault().getPreferenceStore();
        store.setDefault("snapToGeometry", true);
    }
}
