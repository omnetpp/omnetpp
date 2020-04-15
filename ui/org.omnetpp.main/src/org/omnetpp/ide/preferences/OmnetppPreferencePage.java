/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ide.preferences;

import java.io.File;
import java.util.List;

import org.apache.commons.lang3.ArrayUtils;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.preference.ComboFieldEditor;
import org.eclipse.jface.preference.DirectoryFieldEditor;
import org.eclipse.jface.preference.FieldEditor;
import org.eclipse.jface.preference.FieldEditorPreferencePage;
import org.eclipse.jface.preference.FileFieldEditor;
import org.eclipse.jface.preference.IPreferenceStore;
import org.eclipse.jface.preference.StringFieldEditor;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPreferencePage;
import org.omnetpp.common.CommonPlugin;
import org.omnetpp.common.IConstants;
import org.omnetpp.common.ui.MultiLineTextFieldEditor;
import org.omnetpp.common.util.LicenseUtils;
import org.omnetpp.common.util.ProcessUtils;
import org.omnetpp.common.util.ReflectionUtils;
import org.omnetpp.common.util.StringUtils;

/**
 * The OMNeT++ preference page that is contributed to the Preferences dialog.
 *
 * XXX Problems with this page:
 *  1. search does not work (try typing "Doxygen") -- this is probably due to nested group controls
 *  2. Custom license text does not resize (at all!)
 *  3. basically, the whole layout management is a complete mess
 */
public class OmnetppPreferencePage
    extends FieldEditorPreferencePage
    implements IWorkbenchPreferencePage {

    public OmnetppPreferencePage() {
        super(GRID);
        setPreferenceStore(CommonPlugin.getConfigurationPreferenceStore());
    }

    @SuppressWarnings("rawtypes")
    @Override
    public String getErrorMessage() {
        for (Object object : (List)ReflectionUtils.getFieldValue(this, "fields")) {
            if (object instanceof StringFieldEditor) {
                StringFieldEditor fieldEditor = (StringFieldEditor)object;
                if (!fieldEditor.isValid() && fieldEditor.getErrorMessage() != null)
                    return fieldEditor.getErrorMessage();
            }
        }

        return null;
    }

    @Override
    public void createFieldEditors() {
        Composite parent = getFieldEditorParent();
        final Group group = createGroup(parent, "OMNeT++", 3, 3, GridData.FILL_HORIZONTAL);
        Composite spacer = createComposite(group, 3, 3, GridData.FILL_HORIZONTAL);
        createLabel(spacer, "Install location is the directory where the Makefile.inc is located.", 3);
        addAndFillIntoGrid(new DirectoryFieldEditor(IConstants.PREF_OMNETPP_ROOT, "Install location:", spacer), spacer, 3);
        addAndFillIntoGrid(new DirectoryListFieldEditor(IConstants.PREF_OMNETPP_IMAGE_PATH, "Image path:", spacer), spacer, 3);
        createLabel(spacer, "Note: Image path changes take effect on next restart.", 3);

        Group group2 = createGroup(parent, "Tools for generating documentation", 3, 3, GridData.FILL_HORIZONTAL);
        Composite spacer2 = createComposite(group2, 3, 3, GridData.FILL_HORIZONTAL);
        addAndFillIntoGrid(new LookupExecutableFileFieldEditor(IConstants.PREF_DOXYGEN_EXECUTABLE, "Doxygen executable path:", spacer2), spacer2, 3);
        addAndFillIntoGrid(new LookupExecutableFileFieldEditor(IConstants.PREF_GRAPHVIZ_DOT_EXECUTABLE, "GraphViz Dot executable path:", spacer2), spacer2, 3);

        String[] licenses = LicenseUtils.getLicenses();
        String[][] licenseChoices = new String[licenses.length][];
        for (int i=0; i<licenses.length; i++)
            licenseChoices[i] = new String[] {licenses[i], licenses[i]};

        Group group3 = createGroup(parent, "Project and file creation wizards", 3, 3, GridData.FILL_HORIZONTAL);
        Composite spacer3 = createComposite(group3, 3, 3, GridData.FILL_HORIZONTAL);
        addAndFillIntoGrid(new StringFieldEditor(IConstants.PREF_COPYRIGHT_LINE, "Copyright line:", 50, spacer3), spacer3, 3);
        addAndFillIntoGrid(new ComboFieldEditor(IConstants.PREF_DEFAULT_LICENSE, "License:", licenseChoices, spacer3), spacer3, 3);
        if (ArrayUtils.contains(licenses, LicenseUtils.CUSTOM))
           addAndFillIntoGrid(new MultiLineTextFieldEditor(IConstants.PREF_CUSTOM_LICENSE_HEADER, "Custom license source code header:", spacer3), spacer3, 3);
        ((GridLayout)spacer3.getLayout()).numColumns = 3; // idiotic field editors change it in their fillIntoGrid methods!!!
        // Note: at this point, horizSpan of the Combo and the Text is 1, but after an asyncExec it becomes 2!!! probably the dialog plays games

        //TODO disable "custom license header" when not "custom" is selected!!!!


//        UIUtils.dumpWidgetHierarchy(parent);
//        final Composite finalParent = parent;
//        Display.getDefault().asyncExec(new Runnable() {
//            public void run() {
//                System.out.println("ASYNC dumpWidgetHierarchy:");
//                UIUtils.dumpWidgetHierarchy(finalParent);
//            }
//        });
    }

    protected void addAndFillIntoGrid(FieldEditor editor, Composite parent, int numColumns) {
        addField(editor);
    }

    // from SWTFactory
    protected static Group createGroup(Composite parent, String text, int columns, int hspan, int fill) {
        Group g = new Group(parent, SWT.NONE);
        g.setLayout(new GridLayout(columns, false));
        g.setText(text);
        g.setFont(parent.getFont());
        GridData gd = new GridData(fill);
        gd.horizontalSpan = hspan;
        g.setLayoutData(gd);
        return g;
    }

    // from SWTFactory
    protected static Composite createComposite(Composite parent, int columns, int hspan, int fill) {
        Composite g = new Composite(parent, SWT.NONE);
        g.setLayout(new GridLayout(columns, false));
        g.setFont(parent.getFont());
        GridData gd = new GridData(fill);
        gd.horizontalSpan = hspan;
        g.setLayoutData(gd);
        return g;
    }

    // from SWTFactory
    protected static Label createLabel(Composite parent, String text, int hspan) {
        Label l = new Label(parent, SWT.WRAP);
        l.setFont(parent.getFont());
        l.setText(text);
        GridData gd = new GridData(GridData.FILL_HORIZONTAL);
        gd.horizontalSpan = hspan;
        gd.grabExcessHorizontalSpace = false;
        //gd.horizontalIndent = 10;
        l.setLayoutData(gd);
        return l;
    }

    /* (non-Javadoc)
     * @see org.eclipse.ui.IWorkbenchPreferencePage#init(org.eclipse.ui.IWorkbench)
     */
    public void init(IWorkbench workbench) {
    }

    public static boolean isGraphvizDotAvailable() {
        IPreferenceStore store = CommonPlugin.getConfigurationPreferenceStore();
        String graphvizDotExecutablePath = store.getString(IConstants.PREF_GRAPHVIZ_DOT_EXECUTABLE);
        return graphvizDotExecutablePath != null && new File(ProcessUtils.lookupExecutable(graphvizDotExecutablePath)).exists();
    }

    public static boolean isDoxygenAvailable() {
        IPreferenceStore store = CommonPlugin.getConfigurationPreferenceStore();
        String doxyExecutablePath = store.getString(IConstants.PREF_DOXYGEN_EXECUTABLE);
        return doxyExecutablePath != null && new File(ProcessUtils.lookupExecutable(doxyExecutablePath)).exists();
    }

    protected static class LookupExecutableFileFieldEditor extends FileFieldEditor {
        public LookupExecutableFileFieldEditor(String name, String string, Composite fieldEditorParent) {
            super(name, string, fieldEditorParent);
        }

        @Override
        protected boolean checkState() {
            String fileName = ProcessUtils.lookupExecutable(getStringValue());
            boolean state = StringUtils.isEmpty(fileName) || new File(fileName).exists();
            setErrorMessage(state ? null : "Executable file not found: " + getStringValue());
            showErrorMessage(getErrorMessage());
            return state;
        }
    }

    protected static class DirectoryListFieldEditor extends DirectoryFieldEditor {
        public DirectoryListFieldEditor(String name, String labelText, Composite parent) {
            super(name, labelText, parent);
        }

        @Override
        protected String changePressed() {
            String original = getTextControl().getText();
            String newDir = super.changePressed(); // directory selection dialog
            if (newDir == null)
                return null; // cancel
            if (StringUtils.isEmpty(original))
                return newDir;
            if (!original.contains(";") && !new File(original.trim()).isDirectory())
                return newDir; // replace if it only contained a nonexistent directory
            boolean append = MessageDialog.openQuestion(getShell(), "Append?", "Append to existing contents as another directory? (Choosing No will replace it).");
            return append ? (original + ";" + newDir) : newDir;
        }

        @Override
        protected boolean doCheckState() {
            String dirNames = getTextControl().getText();
            for (String dirName : dirNames.split(";")) {
                if (!StringUtils.isEmpty(dirName) && !new File(dirName.trim()).isDirectory()) {
                    setErrorMessage("Nonexistent directory: " + dirName);
                    return false;
                }
            }
            setErrorMessage(null);
            return true;
        }
    }

}

