package org.omnetpp.scave.charting;

import static org.omnetpp.scave.charting.ChartDefaults.*;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_AXIS_TITLE_FONT;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_TICK_FONT;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_GRID_COLOR;

import java.math.BigDecimal;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.SWTGraphics;
import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Transform;

/**
 * Draws a (horizontal or vertical) chart axis, with the corresponding axis 
 * title, ticks and tick labels, and (vertical or horizontal) grid lines.
 * 
 * @author andras
 */
public class LinearAxis {
	private boolean vertical; // horizontal or vertical axis
	private int gap = 0;  // space between axis line and plot area (usually 0)
	private int majorTickLength = 4;
	private int minorTickLength = 2;
	private boolean gridVisible = DEFAULT_SHOW_GRID;

	private Rectangle bounds;
	private Insets insets;  // plot area = bounds minus insets 

	private ICoordsMapping mapping;
	
	private String title; 
	private Font titleFont = DEFAULT_AXIS_TITLE_FONT;
	private Font tickFont = DEFAULT_TICK_FONT;

	public LinearAxis(ICoordsMapping mapping, boolean vertical) {
		this.mapping = mapping;
		this.vertical = vertical;
		this.title = vertical ? DEFAULT_Y_AXIS_TITLE : DEFAULT_X_AXIS_TITLE;
	}

	/**
	 * Modifies insets to accomodate room for axis title, ticks, tick labels etc.
	 * Also returns insets for convenience. 
	 */
	public Insets layoutHint(GC gc, Rectangle bounds, Insets insets) {
		// invalidate: they will have to be set via setLayout()
		this.bounds = null;
		this.insets = null;
		
		gc.setFont(tickFont);
		if (vertical) {
			// calculate tick label length
			Ticks ticks = createTicks(bounds);
			int labelWidth = 0;
			gc.setFont(tickFont);
			for (BigDecimal tick : ticks) {
				if (ticks.isMajorTick(tick)) {
					labelWidth = Math.max(labelWidth, gc.textExtent(tick.toPlainString()).x);
				}						
			}
			gc.setFont(titleFont);
			int titleWidth = title.equals("") ? 0 : gc.textExtent(title).y;  // "y" because we'll print it rotated 90 degrees
			insets.left = Math.max(insets.left, gap + majorTickLength + labelWidth + titleWidth + 4);
			insets.right = Math.max(insets.right, gap + majorTickLength + labelWidth + 4);
		}
		else {
			gc.setFont(tickFont);
			int labelHeight = gc.textExtent("999").y;
			gc.setFont(titleFont);
			int titleHeight = title.equals("") ? 0 : gc.textExtent(title).y;
			insets.top = Math.max(insets.top, gap + majorTickLength + labelHeight + 4);
			insets.bottom = Math.max(insets.bottom, gap + majorTickLength + labelHeight + titleHeight + 4);
		}
		return insets;
	}

	/**
	 * Sets geometry info used for drawing. Plot area = bounds minus insets.
	 */
	public void setLayout(Rectangle bounds, Insets insets) {
		this.bounds = bounds.getCopy();
		this.insets = new Insets(insets);  
	}
	
	/**
	 * Draw both axis and grid. 
	 */
	public void draw(GC gc) {
		draw(gc, true, true);
	}

	/**
	 * Draw axis only. 
	 */
	public void drawAxis(GC gc) {
		draw(gc, true, false);
	}

	public void drawGrid(GC gc) {
		draw(gc, false, true);
	}

	protected void draw(GC gc, boolean doAxis, boolean doGrid) {
		Rectangle plotArea = bounds.getCopy().crop(insets);

		Transform rotate90 = new Transform(gc.getDevice());
		rotate90.rotate(-90);
		
		// draw axis line and title
		gc.setLineWidth(1);
		gc.setLineStyle(SWT.LINE_SOLID);
		gc.setForeground(DEFAULT_AXIS_COLOR);
		gc.setFont(titleFont); 

		Point titleSize = gc.textExtent(title);
		if (doAxis) {
			if (vertical) {
				if (mapping.fromCanvasY(plotArea.bottom()) < 0 && mapping.fromCanvasY(plotArea.y) > 0)
					gc.drawLine(plotArea.x, mapping.toCanvasY(0), plotArea.right(), mapping.toCanvasY(0)); // x axis
				gc.drawLine(plotArea.x - gap, plotArea.y, plotArea.x - gap, plotArea.bottom());
				gc.drawLine(plotArea.right() + gap, plotArea.y, plotArea.right() + gap, plotArea.bottom());
				gc.setTransform(rotate90);
				gc.drawText(title, 
						-(plotArea.y + plotArea.height / 2 + titleSize.x / 2),
						plotArea.x - gap - titleSize.y - majorTickLength - 10); //FIXME "10": labelLength
				gc.setTransform(null);
			}
			else {
				if (mapping.fromCanvasX(plotArea.x) < 0 && mapping.fromCanvasX(plotArea.right()) > 0)
					gc.drawLine(mapping.toCanvasX(0), plotArea.y, mapping.toCanvasX(0), plotArea.bottom()); // y axis
				gc.drawLine(plotArea.x, plotArea.y - gap, plotArea.right(), plotArea.y - gap);
				gc.drawLine(plotArea.x, plotArea.bottom() + gap, plotArea.right(), plotArea.bottom() + gap);
				gc.drawText(title, 
						plotArea.x + plotArea.width / 2 - titleSize.x / 2,
						plotArea.bottom() + gap);
			}
		}
		
		rotate90.dispose();

		// draw ticks and labels
		Ticks ticks = createTicks(plotArea);
		gc.setFont(tickFont);
		for (BigDecimal tick : ticks) {
			String label = tick.toPlainString();
			Point size = gc.textExtent(label);
			int tickLen = ticks.isMajorTick(tick) ? majorTickLength : minorTickLength; 
			if (vertical) {
				int y = mapping.toCanvasY(tick.doubleValue()); 
				if (y >= plotArea.y && y <= plotArea.bottom()) {
					if (doAxis) {
						gc.drawLine(plotArea.x - gap - tickLen, y, plotArea.x - gap, y);
						gc.drawLine(plotArea.right() + gap + tickLen, y, plotArea.right() + gap, y);
						if (ticks.isMajorTick(tick)) {
							gc.drawText(label, plotArea.x - gap - tickLen - size.x - 1, y - size.y / 2);
							gc.drawText(label, plotArea.right() + gap + tickLen + 3, y - size.y / 2);
						}
					}
					if (doGrid && gridVisible) {
						gc.setLineStyle(Graphics.LINE_DOT);
						gc.setForeground(DEFAULT_GRID_COLOR);
						gc.drawLine(plotArea.x, y, plotArea.right(), y);
						gc.setLineStyle(Graphics.LINE_SOLID);
						gc.setForeground(DEFAULT_AXIS_COLOR);
					}
				}
			}
			else {
				int x = mapping.toCanvasX(tick.doubleValue()); 
				if (x >= plotArea.x && x <= plotArea.right()) {
					if (doAxis) {
						gc.drawLine(x, plotArea.y - gap - tickLen, x, plotArea.y - gap);
						gc.drawLine(x, plotArea.bottom() + gap + tickLen, x, plotArea.bottom() + gap);
						if (ticks.isMajorTick(tick)) {
							gc.drawText(label, x - size.x / 2 + 1, plotArea.y - gap - tickLen - size.y - 1);
							gc.drawText(label, x - size.x / 2 + 1, plotArea.bottom() + gap + tickLen + 1);
						}
					}
					if (doGrid && gridVisible) {
						gc.setLineStyle(Graphics.LINE_DOT);
						gc.setForeground(DEFAULT_GRID_COLOR);
						gc.drawLine(x, plotArea.y, x, plotArea.bottom());
						gc.setLineStyle(Graphics.LINE_SOLID);
						gc.setForeground(DEFAULT_AXIS_COLOR);
					}
				}
			}
		}
	}

	protected Ticks createTicks(Rectangle plotArea) {
		if (vertical)
			return createTicks(mapping.fromCanvasY(plotArea.bottom()), mapping.fromCanvasY(plotArea.y), plotArea.height);
		else
			return createTicks(mapping.fromCanvasX(plotArea.x), mapping.fromCanvasX(plotArea.right()), plotArea.width);
	}

	protected static Ticks createTicks(double start, double end, int pixels) {
		return new Ticks(start, end, 50 * (end-start) / pixels );
	}

	public Rectangle getBounds() {
		return bounds;
	}

//	public void setBounds(Rectangle bounds) {
//		this.bounds = bounds;
//	}

	public int getGap() {
		return gap;
	}

	public void setGap(int gap) {
		this.gap = gap;
	}

	public Insets getInsets() {
		return insets;
	}

	public void setInsets(Insets insets) {
		this.insets = insets;
	}

	public Font getTickFont() {
		return tickFont;
	}

	public void setTickFont(Font tickFont) {
		this.tickFont = tickFont;
	}

	public String getTitle() {
		return title;
	}

	public void setTitle(String title) {
		this.title = title;
	}

	public Font getTitleFont() {
		return titleFont;
	}

	public void setTitleFont(Font titleFont) {
		this.titleFont = titleFont;
	}

	public boolean isGridVisible() {
		return gridVisible;
	}

	public void setGridVisible(boolean gridEnabled) {
		this.gridVisible = gridEnabled;
	}
}
