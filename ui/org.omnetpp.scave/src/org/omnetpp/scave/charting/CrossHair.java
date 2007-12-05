package org.omnetpp.scave.charting;

import java.util.ArrayList;
import java.util.List;

import org.apache.commons.lang.StringEscapeUtils;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.KeyAdapter;
import org.eclipse.swt.events.KeyEvent;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseListener;
import org.eclipse.swt.events.MouseMoveListener;
import org.eclipse.swt.events.MouseTrackAdapter;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Cursor;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Control;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.common.ui.IHoverTextProvider;
import org.omnetpp.common.ui.SizeConstraint;
import org.omnetpp.scave.charting.VectorChart.LineProperties;
import org.omnetpp.scave.charting.dataset.DatasetUtils;
import org.omnetpp.scave.charting.dataset.IAveragedXYDataset;
import org.omnetpp.scave.charting.dataset.IStringValueXYDataset;
import org.omnetpp.scave.charting.dataset.IXYDataset;
import org.omnetpp.scave.charting.plotter.IChartSymbol;

/**
 * Displays crosshair mouse cursor for VectorChart.
 */
class CrossHair {
	public static final Cursor CROSS_CURSOR = new Cursor(null, SWT.CURSOR_CROSS);
	public static final Font CROSSHAIR_NORMAL_FONT = new Font(null, "Arial", 8, SWT.NORMAL);
	public static final Font CROSSHAIR_BOLD_FONT = new Font(null, "Arial", 8, SWT.BOLD);
	public static final Color TOOLTIP_COLOR = new Color(null, 255, 255, 225);

	private static final int MAXCOUNT = 1000;
	private static final int HALO = 3;

	private final VectorChart chart;

	private boolean detailedTooltip = false; // turned on while hovering 

	//XXX experimental
//	private Canvas hoverCanvas; // displayed in its own shell, the tooltip window

	Rectangle plotArea;

	// to transfer the coordinates from mouseMoved() to paint()
	private int canvasX = Integer.MAX_VALUE;
	private int canvasY = Integer.MAX_VALUE;

	protected static class DataPoint {
		int series;
		int index;
		int multiplicity;

		public DataPoint(int series, int index) {
			this.series = series;
			this.index = index;
		}
	}

	public CrossHair(VectorChart chart) {
		this.chart = chart;

//		//XXX experimental		
//		Shell shell = new Shell(chart.getShell(), SWT.NO_FOCUS | SWT.ON_TOP | SWT.NO_TRIM);
//		shell.setLayout(new FillLayout());
//		shell.setSize(50,10);
//		hoverCanvas = new Canvas(shell, SWT.DOUBLE_BUFFERED);
//		shell.layout();
//		shell.setVisible(true);
//
//		chart.addMouseTrackListener(new MouseTrackListener() {
//		public void mouseEnter(MouseEvent e) {
//		CrossHair.this.chart.redraw();
//		}
//		public void mouseExit(MouseEvent e) {
//		CrossHair.this.chart.redraw();
//		}
//		public void mouseHover(MouseEvent e) {
//		if ((e.stateMask & SWT.BUTTON_MASK) == 0) {
//		hoverCanvas.getShell().setVisible(true);
//		hoverCanvas.getShell().setLocation(CrossHair.this.chart.toDisplay(e.x+5,e.y+5));
//		//XXX and paint
//		}
//		}
//		});
//		chart.addMouseMoveListener(new MouseMoveListener() {
//		public void mouseMove(MouseEvent e) {
//		hoverCanvas.getShell().setVisible(false);
//		}
//		});			

		final VectorChart finalChart = chart;
		// add key listener to restore the cross cursor, after other cursor is turned off
		// (by ZoomableCanvasMouseSupport for example)
		chart.addKeyListener(new KeyAdapter() {
			public void keyPressed(KeyEvent e) {
				if (finalChart.getCursor() == null)
					finalChart.setCursor(CROSS_CURSOR);
			}
			public void keyReleased(KeyEvent e) {
				if (finalChart.getCursor() == null)
					finalChart.setCursor(CROSS_CURSOR);
			}
		});
		chart.addMouseMoveListener(new MouseMoveListener() {
			public void mouseMove(MouseEvent e) {
				if ((e.stateMask & SWT.BUTTON_MASK) != 0) {
					invalidatePosition();
					finalChart.redraw();
					return;
				}

				canvasX = e.x;
				canvasY = e.y;
				detailedTooltip = false;

				finalChart.redraw();  //XXX this is killer if canvas is not cached. unfortunately, gc.setXORMode() cannot be used

				// set cursor
				if (plotArea != null && plotArea.contains(canvasX,canvasY)) {
					if (finalChart.getCursor() == null)
						finalChart.setCursor(CROSS_CURSOR);
				}
				else
					finalChart.setCursor(null);
			}
		});
		chart.addMouseTrackListener(new MouseTrackAdapter() {
			public void mouseEnter(MouseEvent e) {
				invalidatePosition();
				finalChart.redraw();
			}
			public void mouseExit(MouseEvent e) {
				invalidatePosition();
				finalChart.redraw();
			}
			public void mouseHover(MouseEvent e) {
				detailedTooltip = true;
				finalChart.redraw();
			}
		});
		chart.addMouseListener(new MouseListener() {
			public void mouseDoubleClick(MouseEvent e) {
			}

			public void mouseDown(MouseEvent e) {
				invalidatePosition();
				finalChart.redraw();
			}

			public void mouseUp(MouseEvent e) {
				if (finalChart.getCursor() == null)
					finalChart.setCursor(CROSS_CURSOR);

				canvasX = e.x;
				canvasY = e.y;
				detailedTooltip = false;
				finalChart.redraw();
			}
		});
		HoverSupport hoverSupport = new HoverSupport();
		hoverSupport.setHoverSizeConstaints(500, 400);
		hoverSupport.adapt(chart, new IHoverTextProvider() {
			public String getHoverTextFor(Control control, int x, int y, SizeConstraint outSizeConstraint) {
				return getHoverText(x, y, outSizeConstraint);
			}
		});
	}

	private void invalidatePosition() {
		canvasX = Integer.MAX_VALUE;
		canvasY = Integer.MAX_VALUE;
		detailedTooltip = false;
	}

	public Rectangle layout(GC gc, Rectangle rect) {
		this.plotArea = rect;
		return rect;
	}

	public void draw(GC gc) {
		if (plotArea != null && plotArea.contains(canvasX, canvasY)) {
			int[] saveLineDash = gc.getLineDash();
			int saveLineWidth = gc.getLineWidth();
			Color saveForeground = gc.getForeground();

			// collect points close to cursor (x,y)
			ArrayList<DataPoint> dataPoints = new ArrayList<DataPoint>();
			int totalFound = dataPointsNear(canvasX, canvasY, HALO, dataPoints, 1); 

			// snap to data point
			DataPoint dataPoint = dataPoints.isEmpty() ? null : dataPoints.get(0);
			if (dataPoint != null) {
				IXYDataset dataset = chart.getDataset();
				double x = chart.transformX(dataset.getX(dataPoint.series, dataPoint.index));
				double y = chart.transformY(dataset.getY(dataPoint.series, dataPoint.index));
				canvasX = chart.toCanvasX(x);
				canvasY = chart.toCanvasY(y);
			}

			// draw crosshair
			gc.setLineDash(new int[] {3, 3});
			gc.setLineWidth(1);
			gc.setForeground(ColorFactory.RED);
			gc.drawLine(plotArea.x, canvasY, plotArea.x + plotArea.width, canvasY);
			gc.drawLine(canvasX, plotArea.y, canvasX, plotArea.y + plotArea.height);

//			hoverCanvas.getShell().setVisible(true);
//			hoverCanvas.getShell().setLocation(chart.toDisplay(x+5, y+5));
//			hoverCanvas.getShell().setSize(x%50+10, y%20+10);
//			GC tempGC = new GC(hoverCanvas);
//			tempGC.drawRoundRectangle(0, 0, 50, 20, 5, 5);

			// draw tooltip
			drawTooltip(gc, dataPoints, totalFound);

			gc.setLineDash(saveLineDash);
			gc.setLineWidth(saveLineWidth);
			gc.setForeground(saveForeground);
		}
	}

	private void drawTooltip(GC gc, ArrayList<DataPoint> dataPoints, int totalFound) {
		gc.setForeground(ColorFactory.BLACK);
		gc.setBackground(TOOLTIP_COLOR);
		gc.setLineStyle(SWT.LINE_SOLID);
		gc.setFont(CROSSHAIR_NORMAL_FONT);

		if (dataPoints.isEmpty() || !detailedTooltip) {
			double x = chart.inverseTransformX(chart.fromCanvasX(canvasX));
			double y = chart.inverseTransformY(chart.fromCanvasY(canvasY));
			String coordinates = String.format("%g, %g", x, y); //FIXME precision: the x distance one pixel represents!
			Point size = gc.textExtent(coordinates);
			int left = canvasX + 3;
			int top = canvasY - size.y - 4;
			if (left + size.x + 3 > plotArea.x + plotArea.width)
				left = canvasX - size.x - 6;
			if (top < plotArea.y)
				top = canvasY + 3;
			gc.fillRectangle(left, top, size.x + 2, size.y + 2);
			gc.drawRectangle(left, top, size.x + 2, size.y + 2);
			gc.drawText(coordinates, left + 2, top + 1, false); // XXX set as tooltip, rather than draw it on the canvas!
		}
	}
	
	private String getHoverText(int x, int y, SizeConstraint preferredSize) {
		ArrayList<DataPoint> dataPoints = new ArrayList<DataPoint>();
		int totalFound = dataPointsNear(x, y, HALO, dataPoints, MAXCOUNT); 
		
		if (!dataPoints.isEmpty()) {
			StringBuilder sb = new StringBuilder();
			int maxTextLength = 0;
			sb.append("<table>");
			for (DataPoint dp : dataPoints) {
				LineProperties props = chart.getLineProperties(dp.series);
				Color color = props.getColor();
				String text = getText(dp);
				IChartSymbol symbol = props.getSymbol();
				String imageFile = SymbolImageFactory.getImageFile(color, symbol, true);
				sb.append("<tr>");
				sb.append("<td>");
				if (imageFile != null)
					sb.append(String.format("<img src='%s'>", imageFile));
				sb.append("</td>");
				sb.append("<td>");
				sb.append(StringEscapeUtils.escapeHtml(text));
				sb.append("</td>");
				sb.append("</tr>");
				maxTextLength = Math.max(maxTextLength, text.length());
			}
			if (totalFound > dataPoints.size())
				sb.append(String.format("<tr><td></td><td>... and %d more</td></tr>", totalFound - dataPoints.size()));
			sb.append("</table>");
			preferredSize.preferredWidth = 20 + maxTextLength * 7;
			preferredSize.preferredHeight = 25 + dataPoints.size() * 12;
			return HoverSupport.addHTMLStyleSheet(sb.toString());
		}
		else
			return null;
	}

	private String getText(DataPoint dataPoint) {
		IXYDataset dataset = chart.getDataset();
		//String coordinates = String.format("%g, %g", dataset.getXValue(dataPoint.series, dataPoint.index), dataset.getYValue(dataPoint.series, dataPoint.index));
		int series = dataPoint.series;
		int index = dataPoint.index;
		double xConf = Double.NaN, yConf = Double.NaN;
		String xStr = null, yStr = null;
		if (dataset instanceof IStringValueXYDataset) {
			xStr = ((IStringValueXYDataset)dataset).getXAsString(series, index);
			yStr = ((IStringValueXYDataset)dataset).getYAsString(series, index);
		}
		if (xStr == null) {
			BigDecimal xp = dataset.getPreciseX(series, index);
			double x = dataset.getX(series, index);
			if (dataset instanceof IAveragedXYDataset)
				xConf = ((IAveragedXYDataset)dataset).getXConfidenceInterval(series, index, ChartCanvas.CONFIDENCE_LEVEL);
			xStr = (xp != null ? xp.toString() : chart.formatValue(x, xConf));
		}
		if (yStr == null) {
			BigDecimal yp = dataset.getPreciseY(series, index);
			double y = dataset.getY(series, index);
			if (dataset instanceof IAveragedXYDataset)
				yConf = ((IAveragedXYDataset)dataset).getYConfidenceInterval(series, index, ChartCanvas.CONFIDENCE_LEVEL);
			yStr = (yp != null ? yp.toString() : chart.formatValue(y, yConf));
		}
		String seriesStr = dataset.getSeriesKey(series);
		//seriesStr = StringUtils.abbreviate(series, series.length(), 25);
		return String.format("%s, %s - %s", xStr, yStr, seriesStr);
	}

	// XXX move this method into a VectorPlot class
	protected int dataPointsNear(int x, int y, int d, List<DataPoint> result, int maxCount) {
		IXYDataset dataset = chart.getDataset();
		if (dataset==null)
			return 0;

		// for each series, collect data points close to (x,y), at most maxCount of them
		int totalFound = 0;
		for (int series = 0; series < dataset.getSeriesCount(); ++series) {
			LineProperties props = chart.getLineProperties(series);
			if (!props.getDisplayLine())
				continue;
			// find data point nearest to cursor x, using binary search
			int mid = DatasetUtils.findXLowerLimit(dataset, series, chart.inverseTransformX(chart.fromCanvasX(x)));

			// then search downwards and upwards for data points close to (x,y)
			for (int i = mid; i >= 0; --i) {
				double xx = chart.transformX(dataset.getX(series, i));
				double yy = chart.transformY(dataset.getY(series, i));
				int dx = chart.toCanvasX(xx) - x;
				int dy = chart.toCanvasY(yy) - y;
				if (dx * dx + dy * dy <= d * d) {
					totalFound++;
					if (result.size() < maxCount)  //XXX add at least one point for each series
						result.add(new DataPoint(series, i));
				}
				if (Math.abs(dx) > d)
					break;
			}
			for (int i = mid + 1; i < dataset.getItemCount(series); ++i) {
				double xx = chart.transformX(dataset.getX(series, i));
				double yy = chart.transformY(dataset.getY(series, i));
				int dx = chart.toCanvasX(xx) - x;
				int dy = chart.toCanvasY(yy) - y;
				if (dx * dx + dy * dy <= d * d) {
					totalFound++;
					if (result.size() < maxCount)
						result.add(new DataPoint(series, i));
				}
				if (Math.abs(dx) > d)
					break;
			}
		}
		return totalFound;
	}
}