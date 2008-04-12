package org.omnetpp.scave.charting;

import static org.omnetpp.scave.charting.properties.ChartDefaults.DEFAULT_Y_AXIS_LOGARITHMIC;
import static org.omnetpp.scave.charting.properties.ChartProperties.PROP_HIST_BAR;
import static org.omnetpp.scave.charting.properties.ChartProperties.PROP_HIST_COLOR;
import static org.omnetpp.scave.charting.properties.ChartProperties.PROP_Y_AXIS_LOGARITHMIC;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.swt.widgets.Composite;
import org.omnetpp.common.canvas.ICoordsMapping;
import org.omnetpp.common.canvas.RectangularArea;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.ui.SizeConstraint;
import org.omnetpp.common.util.Converter;
import org.omnetpp.scave.charting.dataset.IDataset;
import org.omnetpp.scave.charting.dataset.IHistogramDataset;
import org.omnetpp.scave.charting.properties.ChartDefaults;
import org.omnetpp.scave.charting.properties.ChartProperties.HistogramBar;

public class HistogramChartCanvas extends ChartCanvas {
	
	private static final boolean debug = false;
	
	
	private IHistogramDataset dataset = IHistogramDataset.EMPTY;
	private LinearAxis xAxis = new LinearAxis(false, false, false);
	private LinearAxis yAxis = new LinearAxis(true, DEFAULT_Y_AXIS_LOGARITHMIC, false);
	private HistogramPlot plot;
	
	private PropertyMap<HistogramProperties> properties = new PropertyMap<HistogramProperties>(HistogramProperties.class);
	static class HistogramProperties {
		RGB color;
	}

	public HistogramChartCanvas(Composite parent, int style) {
		super(parent, style);
		plot = new HistogramPlot(this);
		new Tooltip(this);
	}
	
	public void setProperty(String name, String value) {
		Assert.isLegal(name != null);
		if (debug) System.out.println("HistogramChartCanvas.setProperty: "+name+"='"+value+"'");

		if (PROP_HIST_BAR.equals(name))
			setBarType(Converter.stringToEnum(value, HistogramBar.class));
		else if (name.startsWith(PROP_HIST_COLOR))
			setHistogramColor(getElementId(name), ColorFactory.asRGB(value));
		else if (PROP_Y_AXIS_LOGARITHMIC.equals(name))
			; // TODO
		else 
			super.setProperty(name, value);
	}
	
	public void setBarType(HistogramBar barType) {
		if (barType == null)
			barType = ChartDefaults.DEFAULT_HIST_BAR;
		plot.setBarType(barType);
		chartChanged();
	}
	
	public RGB getHistogramColor(String key) {
		HistogramProperties histProps = properties.getProperties(key);
		if (histProps == null || histProps.color == null)
			histProps = properties.getDefaultProperties();
		return histProps != null ? histProps.color : null;
	}
	
	public void setHistogramColor(String key, RGB color) {
		HistogramProperties histProps = properties.getOrCreateProperties(key);
		histProps.color = color;
		updateLegends();
		chartChanged();
	}

	@Override
	void doSetDataset(IDataset dataset) {
		Assert.isLegal(dataset instanceof IHistogramDataset, "must be an IScalarDataset");
		this.dataset = (IHistogramDataset)dataset;
		updateLegends();
		chartArea = calculatePlotArea();
		updateArea();
		chartChanged();
	}
	
	IHistogramDataset getDataset() {
		return dataset;
	}
	
	private void updateLegends() {
		plot.updateLegend(legendTooltip);
		plot.updateLegend(legend);
	}
	
	@Override
	String getHoverHtmlText(int x, int y, SizeConstraint outSizeConstraint) {
		if (plot.getArea().contains(x, y))
			return plot.getTooltipText(x, y, outSizeConstraint);
		else
			return null;
	}

	@Override
	protected RectangularArea calculatePlotArea() {
		return plot.calculatePlotArea();
	}

	@Override
	protected void doLayoutChart(GC gc) {
		Rectangle area = new Rectangle(getClientArea());

		// preserve zoomed-out state while resizing
		boolean shouldZoomOutX = getZoomX()==0 || isZoomedOutX();
		boolean shouldZoomOutY = getZoomY()==0 || isZoomedOutY();

		// Calculate space occupied by title and legend and set insets accordingly
		ICoordsMapping coordsMapping = getOptimizedCoordinateMapper();
		Rectangle remaining = legendTooltip.layout(gc, area);
		remaining = title.layout(gc, area);
		remaining = legend.layout(gc, remaining);

		Rectangle mainArea = remaining.getCopy();
		Insets insetsToMainArea = new Insets();
		xAxis.layoutHint(gc, mainArea, insetsToMainArea, coordsMapping);
		// postpone yAxis.layoutHint() as it wants to use coordinate mapping which is not yet set up (to calculate ticks)
		insetsToMainArea.left = 50; insetsToMainArea.right = 30; // initial estimate for y axis

		// tentative plotArea calculation (y axis ticks width missing from the picture yet)
		Rectangle plotArea = mainArea.getCopy().crop(insetsToMainArea);
		setViewportRectangle(new org.eclipse.swt.graphics.Rectangle(plotArea.x, plotArea.y, plotArea.width, plotArea.height));

		if (shouldZoomOutX)
			zoomToFitX();
		if (shouldZoomOutY)
			zoomToFitY();
		validateZoom(); //Note: scrollbar.setVisible() triggers Resize too

		// now the coordinate mapping is set up, so the y axis knows what tick labels
		// will appear, and can calculate the occupied space from the longest tick label.
		coordsMapping = getOptimizedCoordinateMapper();
		yAxis.layoutHint(gc, mainArea, insetsToMainArea, coordsMapping);

		// now we have the final insets, set it everywhere again 
		xAxis.setLayout(mainArea, insetsToMainArea);
		yAxis.setLayout(mainArea, insetsToMainArea);
		plotArea = plot.layout(gc, mainArea.getCopy().crop(insetsToMainArea));
		//crosshair.layout(gc, plotArea);
		legend.layoutSecondPass(plotArea);
		//FIXME how to handle it when plotArea.height/width comes out negative??
		setViewportRectangle(new org.eclipse.swt.graphics.Rectangle(plotArea.x, plotArea.y, plotArea.width, plotArea.height));

		if (shouldZoomOutX)
			zoomToFitX();
		if (shouldZoomOutY)
			zoomToFitY();
		validateZoom(); //Note: scrollbar.setVisible() triggers Resize too
	}

	@Override
	protected void doPaintCachableLayer(GC gc, ICoordsMapping coordsMapping) {
		gc.fillRectangle(gc.getClipping());
		yAxis.drawGrid(gc, coordsMapping);
		plot.draw(gc, coordsMapping);
	}

	@Override
	protected void doPaintNoncachableLayer(GC gc, ICoordsMapping coordsMapping) {
		paintInsets(gc);
		title.draw(gc);
		legend.draw(gc);
		xAxis.drawAxis(gc, coordsMapping);
		yAxis.drawAxis(gc, coordsMapping);
		legendTooltip.draw(gc);
		drawStatusText(gc);
		drawRubberband(gc);
	}
}
