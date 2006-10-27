package org.omnetpp.scave2.editors.ui;

import org.eclipse.emf.common.notify.Notification;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model.ScaveModelPackage;
import org.omnetpp.scave2.charting.ChartCanvas;
import org.omnetpp.scave2.charting.ChartFactory;
import org.omnetpp.scave2.charting.ChartUpdater;
import org.omnetpp.scave2.charting.InteractiveChart;
import org.omnetpp.scave2.charting.ScalarChart;
import org.omnetpp.scave2.editors.ScaveEditor;

public class ChartPage extends ScaveEditorPage {

	private Chart chart; // the underlying model
	private Control chartView;
	private ChartUpdater updater;

	public ChartPage(Composite parent, ScaveEditor editor, Chart chart) {
		super(parent, SWT.V_SCROLL, editor);
		this.chart = chart;
		initialize();
		updater = new ChartUpdater(chart, chartView);
	}
	
	public void updatePage(Notification notification) {
		ScaveModelPackage pkg = ScaveModelPackage.eINSTANCE;
		if (pkg.getChart_Name().equals(notification.getFeature())) {
			setPageTitle("Chart: " + chart.getName());
			setFormTitle("Chart: " + chart.getName());
		}
		updater.updateChart(notification);
	}
	
	public Composite getChartComposite() {
		return getBody();
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
		chartView = ChartFactory.createChart(parent, this.chart, scaveEditor.getResultFileManager());
		configureChartView(chartView, chart);
	}
}
