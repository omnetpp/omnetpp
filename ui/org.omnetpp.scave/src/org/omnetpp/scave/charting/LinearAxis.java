package org.omnetpp.scave.charting;

import java.math.BigDecimal;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.SWTGraphics;
import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Point;
import org.omnetpp.common.color.ColorFactory;


/**
 * Draws a (horizontal or vertical) chart axis, with the corresponding axis 
 * title, ticks and tick labels, and (vertical or horizontal) grid lines.
 * 
 * @author andras
 */
public class LinearAxis {
	public static final Font DEFAULT_AXIS_TITLE_FONT = new Font(null, "Arial", 10, SWT.NORMAL);
	public static final Font DEFAULT_TICK_FONT = new Font(null, "Arial", 8, SWT.NORMAL);
	public static final Color AXIS_COLOR = ColorFactory.asColor("black");
	public static final Color GRID_COLOR = ColorFactory.asColor("grey80");
	
	private Rectangle bounds;
	private Insets insets;  // plot area = bounds minus insets 
	private boolean vertical;
	private boolean gridVisible = true;
	private int gap = 0;  // space between axis line and plot area (usually 0)
	private int majorTickLength = 4;
	private int minorTickLength = 2;

	private ICoordsMapping mapping;
	
	private String title = "";
	private Font titleFont = DEFAULT_AXIS_TITLE_FONT;
	private Font tickFont = DEFAULT_TICK_FONT;

	public LinearAxis(ICoordsMapping mapping, boolean vertical) {
		this.mapping = mapping;
		this.vertical = vertical;
	}

	public void layout(GC gc, Rectangle bounds, Insets insets) {
		// note: the "bounds" and "insets" rectangle objects are shared among the
		// horizontal and vertical axis objects and possibly other objects;
		// this is how we keep them consistent.
		
		this.bounds = bounds.getCopy();
		this.insets = insets;  
		
		// grow insets according to our needs
		if (insets.top < 30) insets.top = 30;
		if (insets.bottom < 30) insets.bottom = 30;
		if (insets.left < 50) insets.left = 50;
		if (insets.right < 50) insets.right = 50;

//TODO modify insets along the following lines:
//		gc.setFont(titleFont);
//		Point titleSize = gc.textExtent(title);
//		int width = titleSize.y + 30 + 10;
//		this.rect = recnew Rectangle(bounds.x, bounds.y, width, bounds.height);
//		return new Rectangle(bounds.x + width, bounds.y, Math.max(bounds.width - width, 0), bounds.height);
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

	/**
	 * Draw grid only. 
	 */
	public void drawGrid(GC gc) {
		draw(gc, false, true);
	}
	
	protected void draw(GC gc, boolean doAxis, boolean doGrid) {
		Graphics graphics = new SWTGraphics(gc);
		graphics.pushState();

		Rectangle plotArea = bounds.getCopy().crop(insets);
		
		// draw axis line and title
		graphics.setLineWidth(1);
		graphics.setLineStyle(SWT.LINE_SOLID);
		graphics.setForegroundColor(AXIS_COLOR);
		graphics.setFont(titleFont);
		Point titleSize = gc.textExtent(title);
		if (doAxis) {
			if (vertical) {
				graphics.drawLine(plotArea.x - gap, plotArea.y, plotArea.x - gap, plotArea.bottom());
				graphics.drawLine(plotArea.right() + gap, plotArea.y, plotArea.right() + gap, plotArea.bottom());
				graphics.rotate(-90);
				graphics.drawText(title, 
						plotArea.x - gap - titleSize.y / 2, 
						plotArea.y + plotArea.height / 2 - titleSize.x / 2);
				graphics.rotate(90);
			}
			else {
				graphics.drawLine(plotArea.x, plotArea.y - gap, plotArea.right(), plotArea.y - gap);
				graphics.drawLine(plotArea.x, plotArea.bottom() + gap, plotArea.right(), plotArea.bottom() + gap);
				graphics.drawText(title, 
						plotArea.x + plotArea.width / 2 - titleSize.x / 2,
						plotArea.bottom() + gap);
			}
		}

		// draw ticks and labels
		Ticks ticks;
		if (vertical)
			ticks = createTicks(mapping.fromCanvasY(plotArea.bottom()), mapping.fromCanvasY(plotArea.y), plotArea.height);
		else
			ticks = createTicks(mapping.fromCanvasX(plotArea.x), mapping.fromCanvasX(plotArea.right()), plotArea.width);

		graphics.setFont(tickFont);
		for (BigDecimal tick : ticks) {
			String label = tick.toPlainString();
			Point size = gc.textExtent(label);
			int tickLen = ticks.isMajorTick(tick) ? majorTickLength : minorTickLength; 
			if (vertical) {
				int y = mapping.toCanvasY(tick.doubleValue()); 
				if (y >= plotArea.y && y <= plotArea.bottom()) {
					if (doAxis) {
						graphics.drawLine(plotArea.x - gap - tickLen, y, plotArea.x - gap, y);
						graphics.drawLine(plotArea.right() + gap + tickLen, y, plotArea.right() + gap, y);
						if (ticks.isMajorTick(tick)) {
							graphics.drawText(label, plotArea.x - gap - tickLen - size.x - 1, y - size.y / 2);
							graphics.drawText(label, plotArea.right() + gap + tickLen + 3, y - size.y / 2);
						}
					}
					if (doGrid && gridVisible) {
						graphics.setLineStyle(Graphics.LINE_DOT);
						graphics.setForegroundColor(GRID_COLOR);
						graphics.drawLine(plotArea.x, y, plotArea.right(), y);
						graphics.setLineStyle(Graphics.LINE_SOLID);
						graphics.setForegroundColor(AXIS_COLOR);
					}
				}
			}
			else {
				int x = mapping.toCanvasX(tick.doubleValue()); 
				if (x >= plotArea.x && x <= plotArea.right()) {
					if (doAxis) {
						graphics.drawLine(x, plotArea.y - gap - tickLen, x, plotArea.y - gap);
						graphics.drawLine(x, plotArea.bottom() + gap + tickLen, x, plotArea.bottom() + gap);
						if (ticks.isMajorTick(tick)) {
							graphics.drawText(label, x - size.x / 2 + 1, plotArea.y - gap - tickLen - size.y - 1);
							graphics.drawText(label, x - size.x / 2 + 1, plotArea.bottom() + gap + tickLen + 1);
						}
					}
					if (doGrid && gridVisible) {
						graphics.setLineStyle(Graphics.LINE_DOT);
						graphics.setForegroundColor(GRID_COLOR);
						graphics.drawLine(x, plotArea.y, x, plotArea.bottom());
						graphics.setLineStyle(Graphics.LINE_SOLID);
						graphics.setForegroundColor(AXIS_COLOR);
					}
				}
			}
		}

		graphics.popState();
		graphics.dispose();
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
