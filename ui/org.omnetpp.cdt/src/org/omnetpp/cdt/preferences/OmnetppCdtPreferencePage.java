package org.omnetpp.cdt.preferences;

import org.eclipse.jface.preference.*;
import org.eclipse.ui.IWorkbenchPreferencePage;
import org.eclipse.ui.IWorkbench;
import org.omnetpp.cdt.Activator;

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

public class OmnetppCdtPreferencePage
	extends FieldEditorPreferencePage
	implements IWorkbenchPreferencePage {

	public static final String OMNETPP_ROOT = "omnetppRoot";

    public OmnetppCdtPreferencePage() {
		super(GRID);
		setPreferenceStore(Activator.getDefault().getPreferenceStore());
		setDescription("Specify the directory where the Makefile.inc or configuser.vc are located. " +
				"This is usually the root folder of the OMNEST/OMNeT++ installation.");
	}
	
	public void createFieldEditors() {
	    // FIXME naming is not correct - not install location) 
		addField(new DirectoryFieldEditor(OmnetppCdtPreferencePage.OMNETPP_ROOT, 
				"OMNEST/OMNeT++ install location:", getFieldEditorParent()));
	}

	/* (non-Javadoc)
	 * @see org.eclipse.ui.IWorkbenchPreferencePage#init(org.eclipse.ui.IWorkbench)
	 */
	public void init(IWorkbench workbench) {
	}
	
}