package org.omnetpp.scave2.editors.ui;

import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.DatasetType;
import org.omnetpp.scave2.charting.ChartFactory;
import org.omnetpp.scave2.editors.ScaveEditor;
import org.omnetpp.scave2.model.DatasetManager;
import org.omnetpp.scave2.model.ScaveModelUtil;

public class ChartPage extends ScaveEditorPage {

	private Chart chart; // the underlying model

	public ChartPage(Composite parent, ScaveEditor editor, Chart chart) {
		super(parent, SWT.V_SCROLL, editor);
		this.chart = chart;
		initialize();
	}
	
	public Composite getChartComposite() {
		return getBody();
	}
	
	public void setChart(Control chart) {
		// set layout data
	}
	
	private void initialize() {
		// set up UI
		setPageTitle("Chart: " + chart.getName());
		setFormTitle("Chart: " + chart.getName());
		setExpandHorizontal(true);
		setExpandVertical(true);
		setBackground(ColorFactory.asColor("white"));
		getBody().setLayout(new FillLayout());
		
		// set up contents
		Composite parent = getChartComposite();
		Dataset dataset = ScaveModelUtil.findEnclosingObject(chart, Dataset.class);
		IDList idlist = DatasetManager.getIDListFromDataset(scaveEditor.getResultFileManager(), dataset, chart);
		DatasetType type = dataset.getType();
		setChart(ChartFactory.createChart(parent, type, idlist, scaveEditor.getResultFileManager()));
	}
}
