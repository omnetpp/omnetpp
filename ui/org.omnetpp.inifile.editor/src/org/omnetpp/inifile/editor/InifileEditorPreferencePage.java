package org.omnetpp.inifile.editor;

import org.apache.commons.lang3.StringUtils;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.jface.preference.FieldEditor;
import org.eclipse.jface.preference.FieldEditorPreferencePage;
import org.eclipse.jface.preference.ListEditor;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPreferencePage;
import org.omnetpp.common.ui.SWTFactory;
import org.omnetpp.inifile.editor.model.ConfigRegistry;

/**
 * If the cursor is on a line with an unrecognized config option, add the option
 * to the list of custom options to be ignored.
 */
public class InifileEditorPreferencePage extends FieldEditorPreferencePage implements IWorkbenchPreferencePage {

    protected static class ConfigOptionsListEditor extends ListEditor {
        public ConfigOptionsListEditor(String name, String labelText, Composite parent) {
            super(name, labelText, parent);
        }

        @Override
        protected String[] parseString(String stringList) {
            return stringList.isBlank() ? new String[0] : stringList.split("\\s+");
        }

        @Override
        protected String getNewInputObject() {
            InputDialog inputDialog = new InputDialog(getShell(), "New Config Option", "Enter name of configuration option", "",
                    (s) -> s.isBlank() ? "Cannot be empty" : !s.matches("[a-zA-Z0-9-_]+") ? "Contains illegal character" : null);
            if (inputDialog.open() == Dialog.OK)
                return inputDialog.getValue();
            return null;
        }

        @Override
        protected String createList(String[] items) {
            return StringUtils.join(items, " ");
        }
    }

    public InifileEditorPreferencePage() {
        super(GRID);
        setPreferenceStore(InifileEditorPlugin.getDefault().getPreferenceStore());
        setDescription("Configuration for the OMNeT++ Ini File Editor.");
    }

    /**
     * Creates the field editors.
     */
    public void createFieldEditors() {
        SWTFactory.createComposite(getFieldEditorParent(), 1, 3, 0); // spacer
        FieldEditor editor = new ConfigOptionsListEditor(ConfigRegistry.PREF_IGNORED_OPTIONS, "User-defined configuration options:", getFieldEditorParent());
        SWTFactory.createWrapLabel(getFieldEditorParent(), "Note: Listing them here will prevent the editor from flagging them them as errors.", 3);
        addField(editor);
    }

    /* (non-Javadoc)
     * @see org.eclipse.ui.IWorkbenchPreferencePage#init(org.eclipse.ui.IWorkbench)
     */
    public void init(IWorkbench workbench) {
    }

}