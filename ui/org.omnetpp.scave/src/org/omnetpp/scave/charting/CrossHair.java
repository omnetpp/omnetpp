package org.omnetpp.scave.charting;

import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseMoveListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Cursor;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Point;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.scave.charting.VectorChart.DPoint;

/**
 * Displays crosshair mouse cursor for VectorChart.
 */
class CrossHair {
	public static final Cursor CROSS_CURSOR = new Cursor(null, SWT.CURSOR_CROSS);
	public static final Font CROSS_HAIR_NORMAL_FONT = new Font(null, "Arial", 8, SWT.NORMAL);
	public static final Font CROSS_HAIR_BOLD_FONT = new Font(null, "Arial", 8, SWT.BOLD);

	private final VectorChart chart;

	//XXX experimental
	//private Canvas hoverCanvas; // displayed in its own shell, the tooltip window
	
	Rectangle plotArea;
	
	// to transfer the coordinates from mouseMoved() to paint()
	private int x = Integer.MAX_VALUE;
	private int y = Integer.MAX_VALUE;
	DPoint dataPoint;
	
	public CrossHair(VectorChart chart) {
		this.chart = chart;

		//XXX experimental		
//		Shell shell = new Shell(chart.getShell(), SWT.NO_FOCUS | SWT.ON_TOP | SWT.NO_TRIM);
//		shell.setLayout(new FillLayout());
//		shell.setSize(50,10);
//		hoverCanvas = new Canvas(shell, SWT.DOUBLE_BUFFERED);
//		shell.layout();
//		shell.setVisible(true);
//
//		chart.addMouseTrackListener(new MouseTrackListener() {
//			public void mouseEnter(MouseEvent e) {
//				CrossHair.this.chart.redraw();
//			}
//			public void mouseExit(MouseEvent e) {
//				CrossHair.this.chart.redraw();
//			}
//			public void mouseHover(MouseEvent e) {
//				if ((e.stateMask & SWT.BUTTON_MASK) == 0) {
//					hoverCanvas.getShell().setVisible(true);
//					hoverCanvas.getShell().setLocation(CrossHair.this.chart.toDisplay(e.x+5,e.y+5));
//					//XXX and paint
//				}
//			}
//		});
//		chart.addMouseMoveListener(new MouseMoveListener() {
//			public void mouseMove(MouseEvent e) {
//				hoverCanvas.getShell().setVisible(false);
//			}
//		});			
		
		final VectorChart finalChart = chart;
		chart.addMouseMoveListener(new MouseMoveListener() {
			public void mouseMove(MouseEvent e) {
				if ((e.stateMask & SWT.BUTTON_MASK) != 0) {
					x = Integer.MAX_VALUE;
					y = Integer.MAX_VALUE;
					return; //XXX almost, but we have to update on mousedown, mouseup, mouseenter, mouseleave too
				}

				x = e.x;
				y = e.y;
			
				// snap to data point
				long startTime = System.currentTimeMillis();
				dataPoint = dataPointNear(x, y, 2);
				System.out.println("crosshair: "+(System.currentTimeMillis()-startTime)+" ms");
				
				if (dataPoint != null) {
					x = CrossHair.this.chart.toCanvasX(dataPoint.x);
					y = CrossHair.this.chart.toCanvasY(dataPoint.y);
				}
					
				finalChart.redraw();  //XXX this is killer if canvas is not cached. unfortunately, gc.setXORMode() cannot be used
				
				if (plotArea != null && plotArea.contains(x,y))
					finalChart.setCursor(CROSS_CURSOR);
				else
					finalChart.setCursor(null);
			}
		});
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
			
			gc.setLineDash(new int[] {3, 3});
			gc.setLineWidth(1);
			gc.setForeground(ColorFactory.asColor("red"));
			gc.drawLine(plotArea.x, y, plotArea.x + plotArea.width, y);
			gc.drawLine(x, plotArea.y, x, plotArea.y + plotArea.height);
			
			drawTooltip(gc);
			
			gc.setLineDash(saveLineDash);
			gc.setLineWidth(saveLineWidth);
			gc.setForeground(saveForeground);
		}
	}

	private void drawTooltip(GC gc) {
		Font font = dataPoint != null ? CROSS_HAIR_BOLD_FONT : CROSS_HAIR_NORMAL_FONT; 
		String coordinates =
			String.format("%g,%g", //FIXME precision: the x distance one pixel represents!
				dataPoint != null ? dataPoint.x : this.chart.fromCanvasX(x),
				dataPoint != null ? dataPoint.y : this.chart.fromCanvasY(y));
		gc.setFont(font);
		Point size = gc.textExtent(coordinates);
		int left = x + 3;
		int top = y - size.y - 4;
		if (left + size.x + 3 > plotArea.x + plotArea.width)
			left = x - size.x - 6;
		if (top < plotArea.y)
			top = y + 3;
		gc.setForeground(ColorFactory.asColor("black"));
		gc.setBackground(this.chart.getBackground());
		gc.setLineStyle(SWT.LINE_SOLID);
		gc.drawRectangle(left, top, size.x + 3, size.y + 1);
		gc.drawText(coordinates, left + 2, top + 1, false); // XXX set as tooltip, rather than draw it on the canvas!
	}
	
	private DPoint dataPointNear(int x, int y, int d) {
		if (this.chart.getDataset()==null)
			return null;
		
		// for each series, perform binary search on the x axis
		for (int series = 0; series < this.chart.getDataset().getSeriesCount(); ++series) {
			int start = 0;
			int end = this.chart.getDataset().getItemCount(series) - 1;
			
			while (start <= end) {
				int mid = (end + start) / 2;
				int midX = this.chart.toCanvasX(this.chart.getDataset().getXValue(series, mid));
				
				if (Math.abs(midX - x) <= d) {
					for (int i = mid; i >= start; --i) {
						double xx = this.chart.getDataset().getXValue(series, i);
						double yy = this.chart.getDataset().getYValue(series, i);
						int dx = this.chart.toCanvasX(xx) - x;
						int dy = this.chart.toCanvasY(yy) - y;
						if (dx * dx + dy * dy <= d * d)
							return new VectorChart.DPoint(xx, yy);
						if (Math.abs(dx) > d)
							break;
					}
					for (int i = mid + 1; i <= end; ++i) {
						double xx = this.chart.getDataset().getXValue(series, i);
						double yy = this.chart.getDataset().getYValue(series, i);
						int dx = this.chart.toCanvasX(xx) - x;
						int dy = this.chart.toCanvasY(yy) - y;
						if (dx * dx + dy * dy <= d * d)
							return new VectorChart.DPoint(xx, yy);
						if (Math.abs(dx) > d)
							break;
					}
					break;
				}
				else if (midX - x < 0) {
					start = mid + 1;
				}
				else if (midX - x > 0) {
					end = mid - 1;
				}
			}
		}
		return null;
	}
}