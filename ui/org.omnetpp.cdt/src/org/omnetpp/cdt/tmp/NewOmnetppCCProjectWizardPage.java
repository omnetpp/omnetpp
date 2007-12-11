package org.omnetpp.cdt.tmp;

import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.dialogs.WizardNewProjectCreationPage;

public class NewOmnetppCCProjectWizardPage extends WizardNewProjectCreationPage {

	public NewOmnetppCCProjectWizardPage() {
		super("OMNeT++ Project");
		setTitle("OMNeT++ project");
		setDescription("Creates a new OMNeT++ project.");
        setImageDescriptor(ImageDescriptor.createFromFile(getClass(),"/icons/newoprj_wiz.png"));
	}

}
