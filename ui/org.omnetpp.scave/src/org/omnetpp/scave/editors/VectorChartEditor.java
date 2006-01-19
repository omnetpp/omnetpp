package org.omnetpp.scave.editors;


import java.awt.Color;

import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;
import org.jfree.chart.ChartFactory;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.axis.NumberAxis;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.chart.plot.XYPlot;
import org.jfree.chart.renderer.xy.XYLineAndShapeRenderer;
import org.jfree.data.xy.XYDataset;
import org.jfree.data.xy.XYSeriesCollection;
import org.jfree.ui.RectangleInsets;

import org.omnetpp.scave.charting.ChartHelper;
import org.omnetpp.scave.charting.InteractiveChart;
import org.omnetpp.scave.model.VectorDatasetStrategy;


/**
 * Editor for scalar datasets.
 */
public class VectorChartEditor extends DatasetEditor {

	public VectorChartEditor() {
		super(new VectorDatasetStrategy());
	}

	/**
	 * Creates the Chart page of the multi-page editor.
	 */
	void createChartPage() {
		try {
			Composite panel = new Composite(getContainer(), SWT.NONE);
			panel.setLayout(new GridLayout(2,false));
			int index = addPage(panel);
			setPageText(index, "Chart");

			chartWrapper = new InteractiveChart(panel, SWT.NONE);
			chartWrapper.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
			Button refreshButton = new Button(panel, SWT.NONE);
			refreshButton.setLayoutData(new GridData(SWT.RIGHT, SWT.TOP, false, false));
			refreshButton.setText("Refresh");
			refreshButton.addListener(SWT.Selection, new Listener() {
				public void handleEvent(Event event) {
					chartWrapper.refresh();
				}
			});

			JFreeChart chart = createEmptyChart();
			chartWrapper.setChart(chart);

		} catch (Exception e) {
			ErrorDialog.openError(getSite().getShell(), "Error creating sample chart", null, null);
		}
	}

	/**
	 * Create empty line chart.
	 */
	private JFreeChart createEmptyChart() {
		// create dataset
		XYSeriesCollection xyseriescollection = new XYSeriesCollection();

		// create chart
		JFreeChart jfreechart = ChartFactory.createXYLineChart(
				"Line Chart", "X", "Y",
				xyseriescollection, PlotOrientation.VERTICAL,
				true, true, false);
		jfreechart.setAntiAlias(false);
		jfreechart.setBackgroundPaint(Color.white);
		XYPlot xyplot = (XYPlot)jfreechart.getPlot();
		xyplot.setBackgroundPaint(Color.lightGray);
		xyplot.setAxisOffset(new RectangleInsets(5D, 5D, 5D, 5D));
		xyplot.setDomainGridlinePaint(Color.white);
		xyplot.setRangeGridlinePaint(Color.white);
		XYLineAndShapeRenderer xylineandshaperenderer = (XYLineAndShapeRenderer)xyplot.getRenderer();
		xylineandshaperenderer.setShapesVisible(false);
		xylineandshaperenderer.setShapesFilled(false);
		NumberAxis numberaxis = (NumberAxis)xyplot.getRangeAxis();
		numberaxis.setStandardTickUnits(NumberAxis.createIntegerTickUnits());
		return jfreechart;
	}

	/**
	 * Creates the pages of the multi-page editor.
	 */
	protected void createPages() {
		createDatasetPage();
		createChartPage();
		setActivePage(1);
		updateChart();

		setupDragSource(filterPanel.getPanel().getTable());
		setupDropTarget(filterPanel.getPanel());
		setupDropTarget(chartWrapper);
	}

	public void doUpdateChart() {
		if (chartWrapper!=null && chartWrapper.getChart()!=null) {
			JFreeChart chart = chartWrapper.getChart();
			System.out.println("assembling and running data-flow network...");
			XYDataset ds = ChartHelper.createXYDataSet(getDataset().get());
			System.out.println("done, adding dataset to chart and redrawing it...");
			chart.getXYPlot().setDataset(ds);
			chartWrapper.refresh();
			System.out.println("done");
		}
	}
}
