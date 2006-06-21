package org.omnetpp.scave2.editors.ui;

import java.util.Collection;

import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ChartSheet;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.DatasetType;
import org.omnetpp.scave2.charting.ChartFactory;
import org.omnetpp.scave2.editors.ScaveEditor;
import org.omnetpp.scave2.model.DatasetManager;
import org.omnetpp.scave2.model.ScaveModelUtil;

public class ChartSheetPage extends ScaveEditorPage {

	private ChartSheet chartsheet; // the underlying model

	private LiveTable chartsArea;
	
	public ChartSheetPage(Composite parent, ScaveEditor editor, ChartSheet chartsheet) {
		super(parent, SWT.V_SCROLL | SWT.H_SCROLL, editor);
		this.chartsheet = chartsheet;
		initialize();
	}
	
	public Composite getChartSheetComposite() {
		//return getBody();
		return chartsArea;
	}
	
	public void addChart(Control chart) {
		chart.setLayoutData(new GridData(320,200));
		chartsArea.configureChild(chart);
	}
	
	private void initialize() {
		// set up UI
		setPageTitle("Charts: " + chartsheet.getName());
		setFormTitle("Charts: " + chartsheet.getName());
		setBackground(ColorFactory.asColor("white"));
		setExpandHorizontal(true);
		setExpandVertical(true);
		GridLayout layout = new GridLayout();
		getBody().setLayout(layout);
		
		chartsArea = new LiveTable(getBody(), SWT.DOUBLE_BUFFERED);
		chartsArea.setLayoutData(new GridData(SWT.FILL,SWT.FILL, true, true));
		chartsArea.setBackground(ColorFactory.asColor("white"));

		GridLayout gridLayout = new GridLayout(2, true); //2 columns
		gridLayout.horizontalSpacing = 7;
		gridLayout.verticalSpacing = 7;
		chartsArea.setLayout(gridLayout);
		
		// set up contents
		Collection<Chart> charts = chartsheet.getCharts();
		Composite parent = getChartSheetComposite();
		for (Chart chart : charts) {
			Dataset dataset = ScaveModelUtil.findEnclosingObject(chart, Dataset.class);
			IDList idlist = DatasetManager.getIDListFromDataset(scaveEditor.getResultFileManager(), dataset, chart);
			DatasetType type = dataset.getType();
			addChart(ChartFactory.createChart(parent, type, idlist, scaveEditor.getResultFileManager()));
		}
	}
}
