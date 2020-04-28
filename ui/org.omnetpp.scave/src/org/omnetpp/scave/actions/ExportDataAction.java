/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.wizard.WizardDialog;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.scave.editors.IDListSelection;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.engine.ExporterFactory;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.wizard.ScaveExportWizard;

/**
 * Exports the selected result items in various formats.
 *
 * @author andras
 */
public class ExportDataAction extends AbstractScaveAction {

    public static final String[] FORMATS = ExporterFactory.getSupportedFormats().toArray();

    private String format;

    public ExportDataAction(String format) {
        this.format = format;
        setText(ExporterFactory.getByFormat(format).getDisplayName() + "...");
    }

    @Override
    protected void doRun(ScaveEditor scaveEditor, ISelection selection) throws CoreException {
        if (selection != null) {
            IDList selectedIDs = null;
            final ResultFileManager resultFileManager = scaveEditor.getResultFileManager();
            if (selection instanceof IDListSelection) {
                IDListSelection idlistSelection = (IDListSelection)selection;
                selectedIDs = idlistSelection.getIDList();
            }
            else if (selection instanceof IStructuredSelection && ((IStructuredSelection)selection).size() == 1 && ((IStructuredSelection)selection).getFirstElement() instanceof Chart) {
                // get IDList from selected Chart's filter
                Object selected = ((IStructuredSelection)selection).getFirstElement();
                if (scaveEditor != null && selected instanceof Chart) {
                    Chart chart = (Chart)selected;
                    Property filterProperty = chart.getProperty("filter");
                    if (filterProperty == null) {
                        MessageDialog.openInformation(Display.getCurrent().getActiveShell(), "Error",
                                "Selected chart does not have a 'filter' property which would define "
                                + "which simulation results the chart script uses as input.");
                        return;
                    }
                    String filterExpression = filterProperty.getValue();

                    selectedIDs = ResultFileManager.callWithReadLock(resultFileManager, () -> {
                        IDList idList = resultFileManager.getAllItems(false);
                        return resultFileManager.filterIDList(idList, filterExpression);
                    });

                }
            }
            else {
                return; // no data to export
            }


            ScaveExportWizard wizard = new ScaveExportWizard(format, selectedIDs, resultFileManager);
            WizardDialog dialog = new WizardDialog(scaveEditor.getSite().getShell(), wizard);
            dialog.open();
        }
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        if (selection.isEmpty())
            return false;
        if (selection instanceof IDListSelection) {
            IDList selectedIDs = ((IDListSelection)selection).getIDList();
            int selectionItemTypes = selectedIDs.getItemTypes();
            int supportedTypes = ExporterFactory.getByFormat(format).getSupportedResultTypes();
            int unsupportedTypes = selectionItemTypes & ~supportedTypes;
            return unsupportedTypes == 0;
        }
        if (selection instanceof IStructuredSelection) {
            Object firstElement = ((IStructuredSelection) selection).getFirstElement();
            return firstElement instanceof Chart;
        }
        return false;
    }
}
