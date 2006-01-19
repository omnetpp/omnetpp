package org.omnetpp.scave.editors;


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
import org.jfree.chart.plot.CategoryPlot;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.chart.renderer.category.BarRenderer;
import org.jfree.chart.renderer.category.CategoryItemRenderer;
import org.jfree.data.category.CategoryDataset;
import org.jfree.data.category.DefaultCategoryDataset;

import org.omnetpp.scave.charting.ChartHelper;
import org.omnetpp.scave.charting.InteractiveChart;
import org.omnetpp.scave.model.ScalarDatasetStrategy;


/**
 * Editor for scalar datasets.
 */
public class ScalarChartEditor extends DatasetEditor {

	public ScalarChartEditor() {
		super(new ScalarDatasetStrategy());
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

	/*
	 * Create empty bar chart.
	 */
	private JFreeChart createEmptyChart() {
		// create dataset
		DefaultCategoryDataset categorydataset = new DefaultCategoryDataset();

		// create chart
		JFreeChart jfreechart = ChartFactory.createBarChart3D(
				"Scalars Chart", "Category", "Value",
				categorydataset, PlotOrientation.VERTICAL,
				true, true, false);
		CategoryPlot categoryplot = jfreechart.getCategoryPlot();
		//CategoryAxis categoryaxis = categoryplot.getDomainAxis();
		//categoryaxis.setCategoryLabelPositions(CategoryLabelPositions.createUpRotationLabelPositions(0.39269908169872414D));
		//categoryaxis.setCategoryLabelPositions(CategoryLabelPositions.createUpRotationLabelPositions(0));
		CategoryItemRenderer categoryitemrenderer = categoryplot.getRenderer();
		categoryitemrenderer.setItemLabelsVisible(true);
		BarRenderer barrenderer = (BarRenderer)categoryitemrenderer;
		barrenderer.setMaxBarWidth(0.05D);
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
			CategoryDataset ds = ChartHelper.createChartWithRunsOnXAxis(getDataset().get());
			chart.getCategoryPlot().setDataset(ds);
			chartWrapper.refresh();
		}
	}
}
