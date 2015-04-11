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
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.DatasetItem;
import org.omnetpp.scave.wizard.AbstractExportWizard;
import org.omnetpp.scave.wizard.CsvExportWizard;
import org.omnetpp.scave.wizard.MatlabExportWizard;
import org.omnetpp.scave.wizard.OctaveExportWizard;

/**
 * Exports the selected result items in various formats.
 *
 * @author tomi
 */
public class ExportDataAction extends AbstractScaveAction {

    /** output formats */
    public static final String
        CSV     = "csv",
        MATLAB  = "matlab",
        OCTAVE  = "octave";

    public static final String[] FORMATS = {CSV, OCTAVE };  // We do not provide MATLAB format

    String format;

    public ExportDataAction(String format) {
        this.format = format;
        if (CSV.equals(format)) {
            setText("CSV...");
            setToolTipText("Exports selected data in CSV format.");
        }
        else if (MATLAB.equals(format)) {
            setText("Matlab...");
            setToolTipText("Exports selected data as a Matlab script.");
        }
        else if (OCTAVE.equals(format)) {
            setText("Octave...");
            setToolTipText("Exports selected data in Octave text format.");
        }
    }

    @Override
    protected void doRun(final ScaveEditor scaveEditor, final IStructuredSelection selection) {
        if (selection != null) {
            final IWorkbenchWizard wizard = createWizard();
            if (wizard != null) {
                ResultFileManager.callWithReadLock(scaveEditor.getResultFileManager(), new Callable<Object>() {
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
    protected boolean isApplicable(ScaveEditor editor,
            IStructuredSelection selection) {
        return selection instanceof IDListSelection ||
                (selection instanceof IStructuredSelection &&
                 (selection.getFirstElement() instanceof Dataset ||
                  selection.getFirstElement() instanceof DatasetItem));
    }

    private AbstractExportWizard createWizard() {
        if (CSV.equals(format))
            return new CsvExportWizard();
        else if (MATLAB.equals(format))
            return new MatlabExportWizard();
        else if (OCTAVE.equals(format))
            return new OctaveExportWizard();
        else
            return null;
    }
}
