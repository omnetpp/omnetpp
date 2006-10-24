package org.omnetpp.scave2.editors.ui;

import java.util.Collection;

import org.eclipse.emf.common.notify.Notification;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ChartSheet;
import org.omnetpp.scave.model.ScaveModelPackage;
import org.omnetpp.scave2.charting.ChartCanvas;
import org.omnetpp.scave2.charting.ChartFactory;
import org.omnetpp.scave2.charting.ScalarChart;
import org.omnetpp.scave2.charting.VectorChart;
import org.omnetpp.scave2.editors.ScaveEditor;
import org.omnetpp.scave2.model.ChartProperties;

public class ChartSheetPage extends ScaveEditorPage {

	private ChartSheet chartsheet; // the underlying model

	private LiveTable chartsArea;
	
	public ChartSheetPage(Composite parent, ScaveEditor editor, ChartSheet chartsheet) {
		super(parent, SWT.V_SCROLL | SWT.H_SCROLL, editor);
		this.chartsheet = chartsheet;
		initialize();
	}
	
	public void updatePage(Notification notification) {
		if (ScaveModelPackage.eINSTANCE.getChartSheet_Name().equals(notification.getFeature())) {
			setPageTitle("Charts: " + getChartSheetName(chartsheet));
			setFormTitle("Charts: " + getChartSheetName(chartsheet));
		}
		
		// TODO: update charts
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
		setPageTitle("Charts: " + getChartSheetName(chartsheet));
		setFormTitle("Charts: " + getChartSheetName(chartsheet));
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
		
		chartsArea.addSelectionChangedListener(new ISelectionChangedListener() {
			public void selectionChanged(SelectionChangedEvent event) {
				scaveEditor.setSelection(event.getSelection());
			}
		});
		
		// set up contents
		Collection<Chart> charts = chartsheet.getCharts();
		Composite parent = getChartSheetComposite();
		for (final Chart chart : charts) {
			Control swtChart = ChartFactory.createChart(parent, chart, scaveEditor.getResultFileManager());
			addChart(swtChart);
			configureChartView(swtChart, chart);

		}
	}
	
	@Override
	public void configureChartView(Control view, final Chart chart) {
		
		if (view instanceof ScalarChart) {
			((ScalarChart)view).setDisplayLegend(false);
		}
		else if (view instanceof ChartCanvas) {
			((ChartCanvas)view).setProperty(ChartProperties.PROP_DISPLAY_LEGEND, "false");
		}
		
		view.addMouseListener(new MouseAdapter() { //FIXME this is a hack to get chart opened by double-click; to be done properly (SelectionListener, ask chart from widget)
			public void mouseDoubleClick(MouseEvent e) {
				scaveEditor.openChart(chart);
			}
		});

		super.configureChartView(view, chart);
	}

	private static String getChartSheetName(ChartSheet chartSheet) {
		return chartSheet.getName() != null ? chartSheet.getName() : "<unnamed>";
	}
}
