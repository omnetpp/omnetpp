package org.omnetpp.scave.charting;

import java.awt.Point;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.jfree.chart.plot.Plot;
import org.jfree.chart.plot.Zoomable;

public class InteractiveChart extends ChartSWTWrapper {

	public InteractiveChart(Composite parent, int style) {
		super(parent, style);

	    // enable rubber band selection on the chart
		enableRubberBand();

		// add context menu
		Menu menu = new Menu(this);
	    MenuItem item = new MenuItem(menu, SWT.PUSH);
	    item.setText("Zoom out");
	    item.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				zoomChart(0, 0, 5.0);
			}
		});
		setMenu(menu);
	}

	public void zoomChart(int x, int y, double ratio) {
		Point point = new Point(x,y);
		Plot plot = chart.getPlot();
		if (plot instanceof Zoomable) {
			Zoomable catPlot = (Zoomable)plot;
			catPlot.zoomDomainAxes(ratio, renderingInfo.getPlotInfo(), point);
			//refresh();
		}
	}
	
	/**
	 * Zoom in when the chart gets clicked.
	 */
	public void clicked(int x, int y) {
		Debug.println("CLICKED AT: " + x + "," + y);

		// zoom in the chart (5x)
		zoomChart(x, y, 0.2);
	}

	/**
	 * Zoom in when a rectangle got dragged out.
	 */
	public void rubberBandSelectionMade(Rectangle r) {
		Debug.println("SELECTED: " + r.toString());
		// TODO zoom
	}
}
