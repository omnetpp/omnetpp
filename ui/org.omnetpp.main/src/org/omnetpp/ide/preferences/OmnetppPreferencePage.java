package org.omnetpp.ide.preferences;

import java.io.File;

import org.eclipse.jface.preference.DirectoryFieldEditor;
import org.eclipse.jface.preference.FieldEditorPreferencePage;
import org.eclipse.jface.preference.FileFieldEditor;
import org.eclipse.jface.preference.IPreferenceStore;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPreferencePage;
import org.omnetpp.common.util.ProcessUtils;
import org.omnetpp.ide.OmnetppMainPlugin;

/**
 * This class represents a preference page that
 * is contributed to the Preferences dialog. By 
 * subclassing <samp>FieldEditorPreferencePage</samp>, we
 * can use the field support built into JFace that allows
 * us to create a page that is small and knows how to 
 * save, restore and apply itself.
 * <p>
 * This page is used to modify preferences only. They
 * are stored in the preference store that belongs to
 * the main plug-in class. That way, preferences can
 * be accessed directly via the preference store.
 */

public class OmnetppPreferencePage
	extends FieldEditorPreferencePage
	implements IWorkbenchPreferencePage {

	public static final String OMNETPP_ROOT = "omnetppRoot";
	public static final String DOXYGEN_EXECUTABLE = "doxygenExecutable";
    public static final String GRAPHVIZ_DOT_EXECUTABLE = "graphvizDotExecutable";

    public OmnetppPreferencePage() {
		super(GRID);
		setPreferenceStore(OmnetppMainPlugin.getDefault().getPreferenceStore());
		setDescription("Specify the directory where the Makefile.inc or configuser.vc are located. " +
				"This is usually the root folder of the OMNEST/OMNeT++ installation.");
	}
	
	public void createFieldEditors() {
	    // FIXME naming is not correct - not install location) 
		addField(new DirectoryFieldEditor(OmnetppPreferencePage.OMNETPP_ROOT, "OMNEST/OMNeT++ install location:", getFieldEditorParent()));
		addField(new LookupFileFieldEditor(DOXYGEN_EXECUTABLE, "Doxygen executable path:", getFieldEditorParent()));
        addField(new LookupFileFieldEditor(GRAPHVIZ_DOT_EXECUTABLE, "GraphViz Dot executable path:", getFieldEditorParent()));
	}

	/* (non-Javadoc)
	 * @see org.eclipse.ui.IWorkbenchPreferencePage#init(org.eclipse.ui.IWorkbench)
	 */
	public void init(IWorkbench workbench) {
	}

    public static boolean isGraphvizDotAvailable() {
        IPreferenceStore store = OmnetppMainPlugin.getDefault().getPreferenceStore();
        String graphvizDotExecutablePath = store.getString(OmnetppPreferencePage.GRAPHVIZ_DOT_EXECUTABLE);
        return graphvizDotExecutablePath != null && new File(ProcessUtils.lookupExecutable(graphvizDotExecutablePath)).exists();
    }

    public static boolean isDoxygenAvailable() {
        IPreferenceStore store = OmnetppMainPlugin.getDefault().getPreferenceStore();
        String doxyExecutablePath = store.getString(OmnetppPreferencePage.DOXYGEN_EXECUTABLE);
        return doxyExecutablePath != null && new File(ProcessUtils.lookupExecutable(doxyExecutablePath)).exists();
    }

    private static class LookupFileFieldEditor extends FileFieldEditor {
        public LookupFileFieldEditor(String name, String string, Composite fieldEditorParent) {
            super(name, string, fieldEditorParent);
        }

        @Override
        protected boolean checkState() {
            return ProcessUtils.lookupExecutable(getStringValue()) != null || super.checkState();
        }
    }
}