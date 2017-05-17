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
import org.eclipse.jface.window.Window;
import org.eclipse.jface.wizard.WizardDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Point;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model.AnalysisItem;
import org.omnetpp.scave.wizard.NewScaveObjectWizard;

/**
 * Creates a new chart, chart sheet, or other analysis item.
 */
public class NewAnalysisItemAction extends AbstractScaveAction {
    private AnalysisItem elementPrototype;
    private boolean withEditDialog;

    public NewAnalysisItemAction(AnalysisItem elementPrototype, boolean withEditDialog) {
        this.elementPrototype = elementPrototype;
        this.withEditDialog = withEditDialog;
    }

    @Override
    protected void doRun(ScaveEditor editor, IStructuredSelection selection) {
        EObject target = editor.getAnalysis().getCharts();
        AnalysisItem element = EcoreUtil.copy(elementPrototype);
        Command command = AddCommand.create(editor.getEditingDomain(), target, null, element);
        Assert.isTrue(command.canExecute());

        if (withEditDialog) {
            NewScaveObjectWizard wizard = new NewScaveObjectWizard(editor, target, target.eContents().size(), element);
            WizardDialog dialog = new WizardDialog(editor.getSite().getShell(), wizard) {
                @Override
                protected Point getInitialSize() {
                    return getShell().computeSize(800, SWT.DEFAULT, true);
                }
            };
            if (dialog.open() != Window.OK)
                return;
        }

        editor.executeCommand(command);
        editor.showAnalysisItem(element);
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
        return true;
    }

}
