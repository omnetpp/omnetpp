package org.omnetpp.sequencechart.editors;

import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IViewSite;
import org.eclipse.ui.IWorkbenchPart;
import org.omnetpp.common.ui.ViewWithMessagePart;
import org.omnetpp.sequencechart.widgets.SequenceChart;

//FIXME if view gets displayed while no file is open --> NPE
public class SequenceChartView extends ViewWithMessagePart {
	protected SequenceChart sequenceChart;

	protected ISelectionListener listener;

	@Override
	public void createPartControl(Composite parent) {
		super.createPartControl(parent);

		// we want to provide selection for the sequence chart tool (an IEditPart)
		IViewSite viewSite = (IViewSite)getSite();
		viewSite.setSelectionProvider(sequenceChart);

		// contribute to toolbar
		SequenceChartContributor sequenceChartContributor = new SequenceChartContributor(sequenceChart);
		sequenceChart.setSequenceChartContributor(sequenceChartContributor);
		sequenceChartContributor.contributeToToolBar(viewSite.getActionBars().getToolBarManager());

		// follow selection
		listener = new ISelectionListener() {
			public void selectionChanged(IWorkbenchPart part, ISelection selection) {
				if (part != SequenceChartView.this)
					sequenceChart.setSelection(selection);

				if (sequenceChart.getInput() == null)
					showMessage("No event log available");
				else
					hideMessage();
			}
		};
		viewSite.getPage().addSelectionListener(listener);

		// bootstrap with current selection
		listener.selectionChanged(null, getActiveEditorSelection());
	}
	
	@Override
	public void dispose() {
		super.dispose();
		getViewSite().getPage().removeSelectionListener(listener);
	}

	@Override
	public void setFocus() {
		sequenceChart.setFocus();
	}

	@Override
	protected Control createViewControl(Composite parent) {
		sequenceChart = new SequenceChart(parent, SWT.DOUBLE_BUFFERED);

		return sequenceChart;
	}
}
