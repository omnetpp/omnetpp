/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import java.util.concurrent.Callable;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.wizard.WizardDialog;
import org.eclipse.ui.IWorkbenchWizard;
import org.omnetpp.scave.editors.IDListSelection;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.engine.ExporterFactory;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.wizard.ScaveExportWizard;

/**
 * Exports the selected result items in various formats.
 *
 * @author tomi, andras
 */
public class ExportDataAction extends AbstractScaveAction {

    public static final String[] FORMATS = ExporterFactory.getSupportedFormats().toArray();

    private String format;

    public ExportDataAction(String format) {
        this.format = format;
        setText(ExporterFactory.getByFormat(format).getDisplayName() + "...");
    }

    @Override
    protected void doRun(final ScaveEditor scaveEditor, final IStructuredSelection selection) {
        if (selection != null) {
            final IWorkbenchWizard wizard = new ScaveExportWizard(format);
            if (wizard != null) {
                ResultFileManager.callWithReadLock(scaveEditor.getResultFileManager(), new Callable<Object>() {
                    @Override
                    public Object call() throws Exception {
                        wizard.init(scaveEditor.getSite().getWorkbenchWindow().getWorkbench(), selection);
                        return null;
                    }
                });
                WizardDialog dialog = new WizardDialog(scaveEditor.getSite().getShell(), wizard);
                dialog.open();
            }
        }
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
        return selection instanceof IDListSelection ||
                (selection instanceof IStructuredSelection &&
                        selection.getFirstElement() instanceof Chart);
    }
}
