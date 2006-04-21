/*******************************************************************************
 * Copyright (c) 2000, 2004 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials 
 * are made available under the terms of the Common Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/cpl-v10.html
 * 
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *******************************************************************************/
package org.omnetpp.ned.editor.graph.wizards;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.wizard.Wizard;
import org.eclipse.ui.INewWizard;
import org.eclipse.ui.IWorkbench;

public class ModuleCreationWizard extends Wizard implements INewWizard {
    private ModuleCreationWizardPage1 logicPage = null;

    private IStructuredSelection selection;

    private IWorkbench workbench;

    @Override
    public void addPages() {
        logicPage = new ModuleCreationWizardPage1(workbench, selection);
        addPage(logicPage);
    }

    public void init(IWorkbench aWorkbench, IStructuredSelection currentSelection) {
        workbench = aWorkbench;
        selection = currentSelection;
    }

    @Override
    public boolean performFinish() {
        return logicPage.finish();
    }

}