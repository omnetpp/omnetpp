package org.omnetpp.sequencechart.editors;

import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IViewSite;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.part.ViewPart;
import org.omnetpp.sequencechart.widgets.SequenceChart;

public class SequenceChartView extends ViewPart {
	protected SequenceChart sequenceChart;

	@Override
	public void createPartControl(Composite parent) {
		IViewSite viewSite = (IViewSite)getSite();
		sequenceChart = new SequenceChart(parent, SWT.NONE);

		// we want to provide selection for the sequence chart tool (an IEditPart)
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
			}
		});
		
		// bootstrap with current selection
		sequenceChart.setSelection(getSite().getSelectionProvider().getSelection());
	}

	@Override
	public void setFocus() {
		sequenceChart.setFocus();
	}
}
