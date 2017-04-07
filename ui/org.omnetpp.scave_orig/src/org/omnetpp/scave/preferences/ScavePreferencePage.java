package org.omnetpp.scave.preferences;

import org.eclipse.jface.preference.FieldEditorPreferencePage;
import org.eclipse.jface.preference.IntegerFieldEditor;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPreferencePage;
import org.omnetpp.scave.ScavePlugin;

/**
 * Preference page for Scave.
 */
public class ScavePreferencePage extends FieldEditorPreferencePage implements IWorkbenchPreferencePage {

    public ScavePreferencePage() {
        super(GRID);
        setPreferenceStore(ScavePlugin.getDefault().getPreferenceStore());
        setDescription("Configure the OMNeT++ Result Analysis editor.");
    }

    /**
     * Creates the field editors.
     */
    public void createFieldEditors() {
        IntegerFieldEditor totalLimitEditor = new IntegerFieldEditor(ScavePreferenceConstants.TOTAL_DRAW_TIME_LIMIT_MILLIS, "Line chart drawing time limit (ms):", getFieldEditorParent());
        totalLimitEditor.setValidRange(1, 99999);
        addField(totalLimitEditor);

        IntegerFieldEditor perLineLimitEditor = new IntegerFieldEditor(ScavePreferenceConstants.PER_LINE_DRAW_TIME_LIMIT_MILLIS, "Per-line drawing time limit (ms):", getFieldEditorParent());
        perLineLimitEditor.setValidRange(1, 99999);
        addField(perLineLimitEditor);
    }

    /* (non-Javadoc)
     * @see org.eclipse.ui.IWorkbenchPreferencePage#init(org.eclipse.ui.IWorkbench)
     */
    public void init(IWorkbench workbench) {
    }

}