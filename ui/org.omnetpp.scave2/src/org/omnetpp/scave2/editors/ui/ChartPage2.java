package org.omnetpp.scave2.editors.ui;

import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.layout.RowLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ScaveModelPackage;
import org.omnetpp.scave2.charting.ChartCanvas;
import org.omnetpp.scave2.charting.ChartFactory;
import org.omnetpp.scave2.charting.ChartUpdater;
import org.omnetpp.scave2.editors.ScaveEditor;

public class ChartPage2 extends ScaveEditorPage {

	private Chart chart; // the underlying model
	private ChartCanvas chartView;
	private ChartUpdater updater;

	public ChartPage2(Composite parent, ScaveEditor editor, Chart chart) {
		super(parent, SWT.V_SCROLL, editor);
		this.chart = chart;
		initialize();
		this.updater = new ChartUpdater(chart, chartView, scaveEditor.getResultFileManager());
	}
	
	public void updatePage(Notification notification) {
		if (!(notification.getNotifier() instanceof EObject) ||
			(notification.getNotifier() != chart &&
					((EObject)notification.getNotifier()).eContainer() != chart))
			return;
		
		ScaveModelPackage pkg = ScaveModelPackage.eINSTANCE;
		if (pkg.getChart_Name().equals(notification.getFeature())) {
			setPageTitle("Chart: " + getChartName(chart));
			setFormTitle("Chart: " + getChartName(chart));
		}
		updater.updateChart(notification);
	}

	private String getChartName(Chart chart) {
		return chart.getName() != null ? chart.getName() : "<unnamed>";
	}

	public void setChart(Control chart) {
		// set layout data
	}

	private void initialize() {
		// set up UI
		setPageTitle("Chart: " + getChartName(chart));
		setFormTitle("Chart: " + getChartName(chart));
		setExpandHorizontal(true);
		setExpandVertical(true);
		setBackground(ColorFactory.asColor("white"));
		getBody().setLayout(new GridLayout(2,false));

		// set up contents
		Composite parent = getBody();
		chartView = (ChartCanvas) ChartFactory.createChart(parent, this.chart, scaveEditor.getResultFileManager());
		chartView.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 2, 1));
		configureChartView(chartView, chart);
		
		// create control strip (XXX temp code)
		createControlStrip(parent, chartView);

		Button refresh = new Button(parent, SWT.NONE);
		refresh.setText("Refresh");
		refresh.setLayoutData(new GridData(SWT.RIGHT, SWT.CENTER, false, false));
		refresh.moveAbove(chartView);
		refresh.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				updater.updateDataset();
			}
		});
	}

	//XXX temporary code
	private Composite createControlStrip(Composite parent, final ChartCanvas chart) {
		Composite controlStrip = new Composite(parent, SWT.NONE);
		controlStrip.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
		controlStrip.setLayout(new RowLayout());
		controlStrip.moveAbove(chartView);

		
		Button canvasCaching = new Button(controlStrip, SWT.CHECK);
		canvasCaching.setText("Caching");

		Button antialias = new Button(controlStrip, SWT.CHECK);
		antialias.setText("Antialias");

		Button zoomInX = new Button(controlStrip, SWT.NONE);
		zoomInX.setText("Zoom <->");

		Button zoomOutX = new Button(controlStrip, SWT.NONE);
		zoomOutX.setText("Zoom -><-");

		Button zoomInY = new Button(controlStrip, SWT.NONE);
		zoomInY.setText("Zoom ^");

		Button zoomOutY = new Button(controlStrip, SWT.NONE);
		zoomOutY.setText("Zoom v");
		
		// add event handlers
		zoomInX.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				chart.zoomXBy(1.5);
			}});

		zoomOutX.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				chart.zoomXBy(1/1.5);
			}});
		zoomInY.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				chart.zoomYBy(1.5);
			}});

		zoomOutY.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				chart.zoomYBy(1/1.5);
			}});
		
		canvasCaching.setSelection(chart.getCaching());
		canvasCaching.addSelectionListener(new SelectionAdapter () {
			public void widgetSelected(SelectionEvent e) {
				chart.setCaching(((Button)e.getSource()).getSelection());
				chart.redraw();
			}
		});

		antialias.setSelection(chart.getAntialias()==SWT.ON);
		antialias.addSelectionListener(new SelectionAdapter () {
			public void widgetSelected(SelectionEvent e) {
				chart.setAntialias(((Button)e.getSource()).getSelection() ? SWT.ON : SWT.OFF);
			}
		});
		
		return controlStrip;
	}
}
