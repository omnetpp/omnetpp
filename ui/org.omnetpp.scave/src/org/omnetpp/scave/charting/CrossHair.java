package org.omnetpp.scave.charting;

import java.util.ArrayList;

import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseListener;
import org.eclipse.swt.events.MouseMoveListener;
import org.eclipse.swt.events.MouseTrackAdapter;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Cursor;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Point;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.scave.charting.plotter.IChartSymbol;

/**
 * Displays crosshair mouse cursor for VectorChart.
 */
class CrossHair {
	public static final Cursor CROSS_CURSOR = new Cursor(null, SWT.CURSOR_CROSS);
	public static final Font CROSSHAIR_NORMAL_FONT = new Font(null, "Arial", 8, SWT.NORMAL);
	public static final Font CROSSHAIR_BOLD_FONT = new Font(null, "Arial", 8, SWT.BOLD);
	public static final Color TOOLTIP_COLOR = new Color(null, 255, 255, 225);

	private static final int MAXCOUNT = 10;
	private static final int HALO = 3;

	private final VectorChart chart;

	private boolean detailedTooltip = false; // turned on while hovering 

	//XXX experimental
	//private Canvas hoverCanvas; // displayed in its own shell, the tooltip window

	Rectangle plotArea;

	// to transfer the coordinates from mouseMoved() to paint()
	private int x = Integer.MAX_VALUE;
	private int y = Integer.MAX_VALUE;

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

		//XXX experimental		
//		Shell shell = new Shell(chart.getShell(), SWT.NO_FOCUS | SWT.ON_TOP | SWT.NO_TRIM);
//		shell.setLayout(new FillLayout());
//		shell.setSize(50,10);
//		hoverCanvas = new Canvas(shell, SWT.DOUBLE_BUFFERED);
//		shell.layout();
//		shell.setVisible(true);

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
		chart.addMouseMoveListener(new MouseMoveListener() {
			public void mouseMove(MouseEvent e) {
				if ((e.stateMask & SWT.BUTTON_MASK) != 0) {
					invalidatePosition();
					finalChart.redraw();
					return;
				}

				x = e.x;
				y = e.y;
				detailedTooltip = false;

				finalChart.redraw();  //XXX this is killer if canvas is not cached. unfortunately, gc.setXORMode() cannot be used

				// set cursor
				if (plotArea != null && plotArea.contains(x,y))
					finalChart.setCursor(CROSS_CURSOR);
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
				x = e.x;
				y = e.y;
				detailedTooltip = false;
				finalChart.redraw();
			}
		});
	}

	private void invalidatePosition() {
		x = Integer.MAX_VALUE;
		y = Integer.MAX_VALUE;
		detailedTooltip = false;
	}

	public Rectangle layout(GC gc, Rectangle rect) {
		this.plotArea = rect;
		return rect;
	}

	public void draw(GC gc) {
		if (plotArea != null && plotArea.contains(x, y)) {
			int[] saveLineDash = gc.getLineDash();
			int saveLineWidth = gc.getLineWidth();
			Color saveForeground = gc.getForeground();

			// collect points close to cursor (x,y)
			ArrayList<DataPoint> dataPoints = new ArrayList<DataPoint>();
			int totalFound = dataPointsNear(x, y, HALO, dataPoints, MAXCOUNT); 

			// snap to data point
			DataPoint dataPoint = dataPoints.isEmpty() ? null : dataPoints.get(0);
			if (dataPoint != null) {
				OutputVectorDataset dataset = chart.getDataset();
				double xx = dataset.getXValue(dataPoint.series, dataPoint.index);
				double yy = dataset.getYValue(dataPoint.series, dataPoint.index);
				x = chart.toCanvasX(xx);
				y = chart.toCanvasY(yy);
			}

			// draw crosshair
			gc.setLineDash(new int[] {3, 3});
			gc.setLineWidth(1);
			gc.setForeground(ColorFactory.asColor("red"));
			gc.drawLine(plotArea.x, y, plotArea.x + plotArea.width, y);
			gc.drawLine(x, plotArea.y, x, plotArea.y + plotArea.height);

			// draw tooltip
			drawTooltip(gc, dataPoints, totalFound);

			gc.setLineDash(saveLineDash);
			gc.setLineWidth(saveLineWidth);
			gc.setForeground(saveForeground);
		}
	}

	private void drawTooltip(GC gc, ArrayList<DataPoint> dataPoints, int totalFound) {
		gc.setForeground(ColorFactory.asColor("black"));
		gc.setBackground(TOOLTIP_COLOR);
		gc.setLineStyle(SWT.LINE_SOLID);
		gc.setFont(CROSSHAIR_NORMAL_FONT);

		if (dataPoints.isEmpty() || !detailedTooltip) {
			String coordinates = String.format("%g, %g", chart.fromCanvasX(x), chart.fromCanvasY(y)); //FIXME precision: the x distance one pixel represents!
			Point size = gc.textExtent(coordinates);
			int left = x + 3;
			int top = y - size.y - 4;
			if (left + size.x + 3 > plotArea.x + plotArea.width)
				left = x - size.x - 6;
			if (top < plotArea.y)
				top = y + 3;
			gc.fillRectangle(left, top, size.x + 2, size.y + 2);
			gc.drawRectangle(left, top, size.x + 2, size.y + 2);
			gc.drawText(coordinates, left + 2, top + 1, false); // XXX set as tooltip, rather than draw it on the canvas!
		}
		else {
			// calculate bounding rect
			int width = 0;
			int lineHeight = gc.textExtent("999").y; 
			for (DataPoint dataPoint : dataPoints) {
				String coordinates = getText(dataPoint);
				Point size = gc.textExtent(coordinates);
				width = Math.max(width, size.x);
			}
			int height = dataPoints.size() * lineHeight;
			if (totalFound > dataPoints.size())
				height += lineHeight; 
			int symbolWidth = 10;

			// draw
			int left = x + symbolWidth + 6;
			int top = y - height - 4;
			if (left + width + 3 > plotArea.x + plotArea.width)
				left = x - width - symbolWidth - 8;
			if (top < plotArea.y)
				top = y + 3;
			gc.fillRectangle(left, top, width + symbolWidth + 4, height + 2);
			gc.drawRectangle(left, top, width + symbolWidth + 4, height + 2);
			int currentY = top + 1;
			OutputVectorDataset dataset = chart.getDataset();
			for (DataPoint dataPoint : dataPoints) {
				gc.setForeground(chart.getLineColor(dataPoint.series));
				String text = getText(dataPoint);
				IChartSymbol symbol = chart.getSymbol((String)dataset.getSeriesKey(dataPoint.series));
				symbol.setSizeHint(6);
				symbol.drawSymbol(gc, left + symbolWidth/2 + 2, currentY + lineHeight/2);
				gc.drawLine(left + symbolWidth/2 - 3, currentY + lineHeight/2, left + symbolWidth/2 + 7, currentY + lineHeight/2);
				gc.drawText(text, left + symbolWidth + 6, currentY, true);
				currentY += lineHeight; 
			}
			gc.setForeground(ColorFactory.asColor("black"));
			if (totalFound > dataPoints.size())
				gc.drawText("... and "+(totalFound-dataPoints.size())+" more", left + 2, currentY, true);
		}
	}

	private String getText(DataPoint dataPoint) {
		OutputVectorDataset dataset = chart.getDataset();
		//String coordinates = String.format("%g, %g", dataset.getXValue(dataPoint.series, dataPoint.index), dataset.getYValue(dataPoint.series, dataPoint.index));
		String coordinates = dataset.getXValue(dataPoint.series, dataPoint.index)+", "+dataset.getYValue(dataPoint.series, dataPoint.index);
		coordinates += " - " + dataset.getSeriesKey(dataPoint.series);
		return coordinates;
	}

	private int dataPointsNear(int x, int y, int d, ArrayList<DataPoint> result, int maxCount) {
		OutputVectorDataset dataset = chart.getDataset();
		if (dataset==null)
			return 0;

		// for each series, collect data points close to (x,y), at most maxCount of them
		int totalFound = 0;
		for (int series = 0; series < dataset.getSeriesCount(); ++series) {
			// find data point nearest to cursor x, using binary search
			int mid = DatasetUtils.findXLowerLimit(dataset, series, chart.fromCanvasX(x));

			// then search downwards and upwards for data points close to (x,y)
			for (int i = mid; i >= 0; --i) {
				double xx = dataset.getXValue(series, i);
				double yy = dataset.getYValue(series, i);
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
				double xx = dataset.getXValue(series, i);
				double yy = dataset.getYValue(series, i);
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