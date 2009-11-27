/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import java.util.concurrent.Callable;

import org.eclipse.emf.common.command.BasicCommandStack;
import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.common.command.CommandStack;
import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.MessageBox;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.scave.editors.IDListSelection;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.ResultType;
import org.omnetpp.scave.model.ScaveModelPackage;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * Creates a temporary dataset from the selection on the BrowseDataPage and
 * creates a temporary chart containing the items of the dataset.
 * It opens a chart page to display the chart.
 */
public class CreateTempChartAction extends AbstractScaveAction {
	static int counter = 0;

	public CreateTempChartAction() {
		setText("Plot");
		setToolTipText("Plot");
		setImageDescriptor(ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_PLOT));
	}

	@Override
	protected void doRun(ScaveEditor editor, final IStructuredSelection selection) {
		final String datasetName = "dataset";
		final ResultFileManager manager = ((IDListSelection)selection).getResultFileManager(); // activePanel.getTable().getResultFileManager();
		Dataset dataset = ResultFileManager.callWithReadLock(manager, new Callable<Dataset>() {
			public Dataset call() {
				return ScaveModelUtil.createTemporaryDataset(
						datasetName,
						((IDListSelection)selection).toIDList(), // activePanel.getTable().getSelectedIDs(),
						null,
						manager);
			}
		});

		String chartName = "temp" + ++counter; //FIXME generate proper name
		ResultType type = ((IDListSelection)selection).getResultType();//activePanel.getTable().getType();
		if (type == null) {
            MessageBox messageBox = new MessageBox(Display.getCurrent().getActiveShell(), SWT.OK | SWT.APPLICATION_MODAL | SWT.ICON_ERROR);
            messageBox.setText("Cannot plot data.");
            messageBox.setMessage("The selection contains no or more than one kind of scalar, vector and histogram data.");
            messageBox.open();
            return;
		}
		Chart chart = ScaveModelUtil.createChart(chartName, type);
		dataset.getItems().add(chart);

		Command command = AddCommand.create(
							editor.getEditingDomain(),
							editor.getTempAnalysis().getDatasets(),
							ScaveModelPackage.eINSTANCE.getDatasets_Datasets(),
							dataset);

		// Do not use the CommandStack of the editor, because it would make it dirty
		// and the Add command undoable from the UI.
		// It's safe to use a separate command stack, because the operations on the
		// temporary resource does not interfere with the operations on the persistent resource.
		CommandStack commandStack = new BasicCommandStack();
		commandStack.execute(command);
		commandStack.flush();

		editor.openChart(chart);
	}

	@Override
	protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
	    return selection instanceof IDListSelection && !selection.isEmpty();
	}
}
