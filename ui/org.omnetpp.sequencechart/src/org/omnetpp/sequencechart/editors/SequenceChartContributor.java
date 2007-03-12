package org.omnetpp.sequencechart.editors;

import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.part.EditorActionBarContributor;
import org.omnetpp.eventlog.engine.IEventLog;
import org.omnetpp.sequencechart.widgets.SequenceChart;


public class SequenceChartContributor extends EditorActionBarContributor {
	protected SequenceChart sequenceChart;
	
	public SequenceChartContributor() {
	}
	
	public SequenceChartContributor(SequenceChart sequenceChart) {
		this.sequenceChart = sequenceChart;
	}

	public void contributeToPopupMenu(IMenuManager menuManager) {
	}
	@Override
	public void contributeToToolBar(IToolBarManager toolBarManager) {
	}
	
	@Override
	public void setActiveEditor(IEditorPart targetEditor) {
		sequenceChart = ((SequenceChartEditor)targetEditor).getSequenceChart();
	}
	
	private IEventLog getEventLog() {
		return sequenceChart.getEventLog();
	}
}
