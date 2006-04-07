package org.omnetpp.scave.charting;

import java.awt.Point;

import org.eclipse.jface.preference.PreferenceDialog;
import org.eclipse.jface.preference.PreferenceManager;
import org.eclipse.jface.preference.PreferenceNode;
import org.eclipse.jface.preference.PreferencePage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.jfree.chart.plot.Plot;
import org.jfree.chart.plot.Zoomable;
import org.omnetpp.scave.dialogs.AxesPreferencePage;
import org.omnetpp.scave.dialogs.BarsPreferencePage;
import org.omnetpp.scave.dialogs.LegendPreferencePage;
import org.omnetpp.scave.dialogs.TitlePreferencePage;

public class InteractiveChart extends ChartSWTWrapper {

	public InteractiveChart(Composite parent, int style) {
		super(parent, style);

	    // enable rubber band selection on the chart
		enableRubberBand();

		// add context menu
		Menu menu = new Menu(this);
	    MenuItem item = new MenuItem(menu, SWT.PUSH);
	    item.setText("Zoom out");
	    item.addSelectionListener(new SelectionListener() {
			public void widgetSelected(SelectionEvent e) {
				zoomChart(0, 0, 5.0);
			}
			public void widgetDefaultSelected(SelectionEvent e) {}
			});
	    item = new MenuItem(menu, SWT.PUSH);
	    item.setText("Properties");
	    item.addSelectionListener(new SelectionListener() {
			public void widgetSelected(SelectionEvent e) {
				showPropertyDialog();
			}

			public void widgetDefaultSelected(SelectionEvent e) {
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
		System.out.println("CLICKED AT: " + x + "," + y);

		// zoom in the chart (5x)
		zoomChart(x, y, 0.2);
	}

	/**
	 * Zoom in when a rectangle got dragged out.
	 */
	public void rubberBandSelectionMade(Rectangle r) {
		System.out.println("SELECTED: " + r.toString());
		// TODO zoom
	}
	
	public void showPropertyDialog()
	{
		PreferenceDialog dialog = new PreferenceDialog(getShell(), createPreferenceManager());
		dialog.open();
		
	}

	private PreferenceManager createPreferenceManager() {
		PreferenceManager manager = new PreferenceManager();
		manager.addToRoot(new PreferenceNode("Title", new TitlePreferencePage("Title", this)));
		manager.addToRoot(new PreferenceNode("Axes", new AxesPreferencePage("Axes")));
		manager.addToRoot(new PreferenceNode("Bars", new BarsPreferencePage("Bars")));
		manager.addToRoot(new PreferenceNode("Legend", new LegendPreferencePage("Legend")));
		return manager;
	}
	
	static class EmptyPreferencePage extends PreferencePage
	{
		public EmptyPreferencePage(String title)
		{
			super(title);
		}
		
		@Override
		protected Control createContents(Composite parent) {
			return new Composite(parent, SWT.NULL);
		}
	}
}
