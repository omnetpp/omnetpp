/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import java.util.Collection;
import java.util.concurrent.Callable;

import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.edit.command.CreateChildCommand;
import org.eclipse.emf.edit.domain.EditingDomain;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.window.Window;
import org.eclipse.jface.wizard.WizardDialog;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.wizard.NewScaveObjectWizard;

/**
 * Adds a new child or sibling to the selected object.
 * It opens a wizard where the type of the new node and its
 * attributes can be entered.
 */
public class NewAction extends AbstractScaveAction {
    private EObject defaultParent;
    private boolean createChild;

    public NewAction() {
        this(null, true);
    }

    public NewAction(EObject defaultParent, boolean createChild) {
        this.defaultParent = defaultParent;
        this.createChild = createChild;
        if (createChild) {
            setText("New Child...");
            setToolTipText("Create new child");
        }
        else {
            setText("New Sibling...");
            setToolTipText("Create new sibling");
        }
    }

    @Override
    protected void doRun(final ScaveEditor editor, final IStructuredSelection selection) {
        final EObject parent = getParent(selection);
        final NewScaveObjectWizard wizard = new NewScaveObjectWizard(editor, parent);
        if (isApplicable(editor, selection)) {
            int returnCode = ResultFileManager.callWithReadLock(editor.getResultFileManager(), new Callable<Integer>() {
                public Integer call() throws Exception {
                    WizardDialog dialog = new WizardDialog(editor.getSite().getShell(), wizard);
                    return dialog.open();
                }
            });

            if (returnCode == Window.OK) {
                Command command = CreateChildCommand.create(
                        editor.getEditingDomain(),
                        parent,
                        wizard.getNewChildDescriptor(),
                        selection.toList());
                editor.executeCommand(command);
            }
        }
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
        EObject parent = getParent(selection);
        if (parent != null) {
            EditingDomain domain = editor.getEditingDomain();
            Collection<?> childDescriptors = domain.getNewChildDescriptors(parent, null);
            return childDescriptors.size() > 0;
        }
        return false;
    }

    private EObject getParent(IStructuredSelection selection) {
        if (selection!=null && selection.size()==1 && selection.getFirstElement() instanceof EObject) {
            EObject object = (EObject)selection.getFirstElement();
            return createChild ? object : object.eContainer();
        }
        else
            return defaultParent;
    }
}
