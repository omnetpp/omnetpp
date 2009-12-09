/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.wizards;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.wizard.Wizard;
import org.eclipse.ui.INewWizard;
import org.eclipse.ui.IWorkbench;

/**
 * Wizard for ini files. Its role is to create a new file
 * resource in the provided container. If the container resource
 * (a folder or a project) is selected in the workspace
 * when the wizard is opened, it will accept it as the target
 * container. The wizard creates one file with the extension
 * "ini".
 */
public class NewInifileWizard extends Wizard implements INewWizard {
    private NewInifileWizardPage1 page1 = null;
    private IStructuredSelection selection;
    private IWorkbench workbench;

    @Override
    public void addPages() {
        page1 = new NewInifileWizardPage1(workbench, selection);
        addPage(page1);
    }

    public void init(IWorkbench aWorkbench, IStructuredSelection currentSelection) {
        workbench = aWorkbench;
        selection = currentSelection;
        setWindowTitle("New Ini File");
    }

    @Override
    public boolean performFinish() {
        return page1.finish();
    }
}