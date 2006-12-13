package org.omnetpp.scave.editors.ui;

import org.eclipse.emf.common.notify.Notification;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.scave.charting.ChartFactory;
import org.omnetpp.scave.charting.ChartUpdater;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ScaveModelPackage;

public class ChartPage extends ScaveEditorPage {

	private Chart chart; // the underlying model
	private Control chartView;
	private ChartUpdater updater;

	public ChartPage(Composite parent, ScaveEditor editor, Chart chart) {
		super(parent, SWT.V_SCROLL, editor);
		this.chart = chart;
		initialize();
		updater = new ChartUpdater(chart, chartView, editor.getResultFileManager());
	}
	
	public void updatePage(Notification notification) {
		ScaveModelPackage pkg = ScaveModelPackage.eINSTANCE;
		if (pkg.getChart_Name().equals(notification.getFeature())) {
			setPageTitle("Chart: " + chart.getName());
			setFormTitle("Chart: " + chart.getName());
		}
		updater.updateChart(notification);
	}
	
	private void initialize() {
		// set up UI
		setPageTitle("Chart: " + chart.getName());
		setFormTitle("Chart: " + chart.getName());
		setExpandHorizontal(true);
		setExpandVertical(true);
		setBackground(ColorFactory.asColor("white"));
		getBody().setLayout(new GridLayout());
		
		// set up contents
		Button refreshButton = new Button(getBody(), SWT.NONE);
		refreshButton.setLayoutData(new GridData(SWT.RIGHT, SWT.CENTER, false, false));
		refreshButton.setText("Refresh");
		refreshButton.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				updater.updateDataset();
			}
		});
		chartView = ChartFactory.createChart(getBody(), this.chart, scaveEditor.getResultFileManager());
		chartView.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		configureChartView(chartView, chart);
	}
}
