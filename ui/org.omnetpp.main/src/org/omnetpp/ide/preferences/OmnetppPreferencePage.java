package org.omnetpp.ide.preferences;

import java.io.File;

import org.apache.commons.lang.StringUtils;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.preference.DirectoryFieldEditor;
import org.eclipse.jface.preference.FieldEditor;
import org.eclipse.jface.preference.FieldEditorPreferencePage;
import org.eclipse.jface.preference.FileFieldEditor;
import org.eclipse.jface.preference.IPreferenceStore;
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

	public OmnetppPreferencePage() {
		super(GRID);
		setPreferenceStore(CommonPlugin.getConfigurationPreferenceStore());
	}

	public void createFieldEditors() {
		Composite parent = getFieldEditorParent();
        final Group group = createGroup(parent, "OMNeT++", 3, 3, GridData.FILL_HORIZONTAL);
        Composite spacer = createComposite(group, 3, 3, GridData.FILL_HORIZONTAL);
        createLabel(spacer, "Install location is the directory where the Makefile.inc or configuser.vc is located.", 3);
        addAndFillIntoGrid(new DirectoryFieldEditor(IConstants.PREF_OMNETPP_ROOT, "Install location:", spacer), spacer, 3);
        addAndFillIntoGrid(new DirectoryListFieldEditor(IConstants.PREF_OMNETPP_IMAGE_PATH, "Image path:", spacer), spacer, 3);
        createLabel(spacer, "Note: Image path changes take effect on next restart.", 3);
        
		// supported only in the commercial build
		if (IConstants.IS_COMMERCIAL) {
		    Group group2 = createGroup(parent, "Tools for generating documentation", 3, 3, GridData.FILL_HORIZONTAL);
	        Composite spacer2 = createComposite(group2, 3, 3, GridData.FILL_HORIZONTAL);
	        addAndFillIntoGrid(new LookupExecutableFileFieldEditor(IConstants.PREF_DOXYGEN_EXECUTABLE, "Doxygen executable path:", spacer2), spacer2, 3);
	        addAndFillIntoGrid(new LookupExecutableFileFieldEditor(IConstants.PREF_GRAPHVIZ_DOT_EXECUTABLE, "GraphViz Dot executable path:", spacer2), spacer2, 3);
		}
	}

	protected void addAndFillIntoGrid(FieldEditor editor, Composite parent, int numColumns) {
	    addField(editor);
	    editor.fillIntoGrid(parent, numColumns);
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
    	if (!IConstants.IS_COMMERCIAL)
    		return false;
        IPreferenceStore store = OmnetppMainPlugin.getDefault().getPreferenceStore();
        String graphvizDotExecutablePath = store.getString(IConstants.PREF_GRAPHVIZ_DOT_EXECUTABLE);
        return graphvizDotExecutablePath != null && new File(ProcessUtils.lookupExecutable(graphvizDotExecutablePath)).exists();
    }

    public static boolean isDoxygenAvailable() {
    	if (!IConstants.IS_COMMERCIAL)
    		return false;
        IPreferenceStore store = OmnetppMainPlugin.getDefault().getPreferenceStore();
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
            
            if (!state)
                showErrorMessage("Executable file not found: " + getStringValue());
            else
                clearErrorMessage();

            return state;
        }
    }

    protected static class DirectoryListFieldEditor extends DirectoryFieldEditor {
        public DirectoryListFieldEditor(String name, String labelText, Composite parent) {
            super(name, labelText, parent);
            setErrorMessage("Value contains nonexistent directory");
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
            for (String dirName : dirNames.split(";"))
                if (!StringUtils.isEmpty(dirName) && !new File(dirName.trim()).isDirectory())
                    return false;
            return true;
        }
    }
   
}

