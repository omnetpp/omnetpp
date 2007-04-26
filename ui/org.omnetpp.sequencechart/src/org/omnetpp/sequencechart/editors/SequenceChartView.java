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

public class SequenceChartView extends ViewWithMessagePart {
	protected SequenceChart sequenceChart;

	@Override
	public void createPartControl(Composite parent) {
		super.createPartControl(parent);

		// we want to provide selection for the sequence chart tool (an IEditPart)
		IViewSite viewSite = (IViewSite)getSite();
		viewSite.setSelectionProvider(sequenceChart);

		// contribue to toolbar
		SequenceChartContributor sequenceChartContributor = new SequenceChartContributor(sequenceChart);
		sequenceChart.setSequenceChartContributor(sequenceChartContributor);
		sequenceChartContributor.contributeToToolBar(viewSite.getActionBars().getToolBarManager());

		// follow selection
		viewSite.getPage().addSelectionListener(new ISelectionListener() {
			public void selectionChanged(IWorkbenchPart part, ISelection selection) {
				if (part != sequenceChart)
					sequenceChart.setSelection(selection);

				if (sequenceChart.getInput() == null)
					showMessage("No event log available");
				else
					hideMessage();
			}
		});
		
		// bootstrap with current selection
		sequenceChart.setSelection(getActiveEditorSelection());
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
