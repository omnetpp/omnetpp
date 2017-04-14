/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.core.runtime.Assert;
import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.util.EcoreUtil;
import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.window.Window;
import org.eclipse.jface.wizard.WizardDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.wizard.NewScaveObjectWizard;

/**
 * Opens an edit dialog for the selected dataset, chart, chart sheet, etc.
 */
public class NewChartObjectAction extends AbstractScaveAction {
    private EObject elementPrototype;

    public NewChartObjectAction(EObject elementPrototype) {
        this.elementPrototype = elementPrototype;
    }

    @Override
    protected void doRun(ScaveEditor editor, IStructuredSelection selection) {
        //TODO  ResultFileManager.callWithReadLock(editor.getResultFileManager(), new Callable<Object>() {

        EObject target = editor.getAnalysis().getCharts();
        final EObject element = EcoreUtil.copy(elementPrototype);
        Command command = AddCommand.create(editor.getEditingDomain(), target, null, element);
        Assert.isTrue(command.canExecute());
        NewScaveObjectWizard wizard = new NewScaveObjectWizard(editor, target, target.eContents().size(), element);
        WizardDialog dialog = new WizardDialog(editor.getSite().getShell(), wizard) {
            @Override
            protected Point getInitialSize() {
                return getShell().computeSize(800, SWT.DEFAULT, true);
            }
        };
        if (dialog.open() == Window.OK)
            editor.executeCommand(command);

        // if it got inserted (has parent now), select it in the viewer.
        // TODO check whether it needs to be in asyncExec
        if (element.eContainer() != null) {
            Display.getDefault().asyncExec(new Runnable() {
                @Override
                public void run() {
                    editor.setSelection(new StructuredSelection(element));
                }
            });
        }
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
        return true;
    }

}
