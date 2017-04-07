/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.preferences;

import org.eclipse.core.runtime.preferences.AbstractPreferenceInitializer;
import org.eclipse.jface.preference.IPreferenceStore;
import org.omnetpp.scave.ScavePlugin;

/**
 * Preference initializer
 *
 * @author Andras
 */
public class ScavePreferenceInitializer extends AbstractPreferenceInitializer {
    @Override
    public void initializeDefaultPreferences() {
        IPreferenceStore store = ScavePlugin.getDefault().getPreferenceStore();
        store.setDefault(ScavePreferenceConstants.TOTAL_DRAW_TIME_LIMIT_MILLIS, 10000);
        store.setDefault(ScavePreferenceConstants.PER_LINE_DRAW_TIME_LIMIT_MILLIS, 2000);
    }
}
