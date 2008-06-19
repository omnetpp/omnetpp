package org.omnetpp.sequencechart.editors;

import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IPartListener;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IViewSite;
import org.eclipse.ui.IWorkbenchPart;
import org.omnetpp.common.eventlog.EventLogView;
import org.omnetpp.common.eventlog.IEventLogSelection;
import org.omnetpp.eventlog.engine.IEventLog;
import org.omnetpp.sequencechart.widgets.SequenceChart;

/**
 * View for displaying causes and consequences of events.
 */
public class SequenceChartView extends EventLogView {
	private SequenceChart sequenceChart;

    private SequenceChartContributor sequenceChartContributor;

	private ISelectionListener selectionListener;

	private IPartListener partListener;

	@Override
	public void createPartControl(Composite parent) {
		super.createPartControl(parent);

		// we want to provide selection for the sequence chart tool (an IEditPart)
		IViewSite viewSite = (IViewSite)getSite();
		viewSite.setSelectionProvider(sequenceChart);

		// contribute to toolbar
		sequenceChartContributor = new SequenceChartContributor(sequenceChart);
		sequenceChart.setSequenceChartContributor(sequenceChartContributor);
		sequenceChartContributor.contributeToToolBar(viewSite.getActionBars().getToolBarManager());

		// follow selection
		selectionListener = new ISelectionListener() {
			public void selectionChanged(IWorkbenchPart part, ISelection selection) {
				if (part != SequenceChartView.this && selection instanceof IEventLogSelection)
					updateSelection(selection);
			}
		};
		viewSite.getPage().addSelectionListener(selectionListener);

		// follow active editor changes
		partListener = new IPartListener() {
			public void partActivated(IWorkbenchPart part) {
			}

			public void partBroughtToTop(IWorkbenchPart part) {
			    if (part instanceof IEditorPart && !sequenceChart.isDisposed())
			        updateSelectionFromActiveEditor();
			}

			public void partClosed(IWorkbenchPart part) {
                if (part instanceof IEditorPart && !sequenceChart.isDisposed())
                    updateSelectionFromActiveEditor();
			}

			public void partDeactivated(IWorkbenchPart part) {
			}

			public void partOpened(IWorkbenchPart part) {
                if (part instanceof IEditorPart && !sequenceChart.isDisposed())
                    updateSelectionFromActiveEditor();
			}
		};
		viewSite.getPage().addPartListener(partListener);

		// bootstrap with current selection
		selectionListener.selectionChanged(null, getActiveEditorSelection());
	}
	
	@Override
	public IEventLog getEventLog() {
	    return sequenceChart.getEventLog();
	}
	
	@Override
	public void dispose() {
		IViewSite viewSite = (IViewSite)getSite();
		
		if (selectionListener != null)
			viewSite.getPage().removeSelectionListener(selectionListener);
		
		if (partListener != null)
			viewSite.getPage().removePartListener(partListener);

		super.dispose();
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

    private void updateSelectionFromActiveEditor() {
        updateSelection(getActiveEditorSelection());
    }
        
    private void updateSelection(ISelection selection) {
		if (selection instanceof IEventLogSelection) {
			hideMessage();
			sequenceChart.setSelection(selection);
		}
		else {
			sequenceChart.setInput(null);
			showMessage("No event log available");
		}
	}
}
