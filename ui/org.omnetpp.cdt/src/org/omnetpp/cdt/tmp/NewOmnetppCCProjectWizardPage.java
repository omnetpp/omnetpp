/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

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
