package org.omnetpp.scave.charting;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Point;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.SWTGraphics;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.scave.model2.ChartProperties.LegendAnchor;
import org.omnetpp.scave.model2.ChartProperties.LegendPosition;

/**
 * This class layouts and draws the legend on a vector chart.
 *
 * @author tomi
 */
public class Legend {
	
	class Item {
		String text;
		Color color;
		int x, y;	// location relative to the legend top-left
		int width, height;
		
		public Item(Color color, String text) {
			this.color = color;
			this.text = text;
		}
		
		public void calculateSize(GC gc) {
			Point size = gc.textExtent(text);
			width = size.x + 9; // place for mark
			height = size.y;
		}
		
		public void draw(GC gc, int x, int y) {
			// draw oval
			if (font != null) gc.setFont(font);
			gc.setForeground(color);
			gc.setBackground(color);
			gc.fillOval(x + 2 , y + (height - 5) / 2, 5, 5);
			// draw text
			gc.setForeground(ColorFactory.asColor("black"));
			gc.drawText(text, x + 9, y, true);
		}
	}
	
	private boolean visible;
	private boolean drawBorder;
	private Font font;
	private LegendPosition position;
	private LegendAnchor anchor;
	private int horizontalSpacing = 5;
	private int verticalSpacing = 5;
	private int horizontalMargin = 5;
	private int verticalMargin = 5;
	private List<Item> items = new ArrayList<Item>();
	private Rectangle bounds; // rectangle of the legend in canvas coordinates
	
	public Legend() {
	}
	
	public Legend(boolean visible, boolean drawBorder, Font font, LegendPosition position, LegendAnchor anchor) {
		this.visible = visible;
		this.drawBorder = drawBorder;
		this.font = font;
		this.position = position;
		this.anchor = anchor;
	}
	
	public void clearLegendItems() {
		items.clear();
	}
	
	public void addLegendItem(Color color, String text) {
		items.add(new Item(color, text));
	}
	
	public boolean isVisible() {
		return visible;
	}
	
	public void setVisible(boolean flag) {
		visible = flag;
	}
	
	public boolean getDrawBorder() {
		return drawBorder;
	}
	
	public void setDrawBorder(boolean flag) {
		drawBorder = flag;
	}
	
	public Font getFont() {
		return font;
	}
	
	public void setFont(Font font) {
		this.font = font;
	}
	
	public LegendPosition getPosition() {
		return position;
	}
	
	public void setPosition(LegendPosition position) {
		this.position = position;
	}
	
	public LegendAnchor getAnchor() {
		return anchor;
	}
	
	public void setAnchor(LegendAnchor anchor) {
		this.anchor = anchor;
	}
	
	public Rectangle getBounds() {
		return bounds;
	}
	
	/**
	 * Calculates the position and size of the legend and its items.
	 * Returns the remaining space.
	 */
	public Rectangle layout(GC gc, Rectangle parent) {
		if (!visible)
			return parent;
		
		gc.setFont(font);
		
		// measure items
		for (int i = 0; i < items.size(); ++i)
			items.get(i).calculateSize(gc);
		
		// position items and calculate size
		bounds = new Rectangle(0, 0, 0, 0);
		int x = 0;
		int y = 0;
		boolean vertical = position == LegendPosition.Left || position == LegendPosition.Right;
		for (int i = 0; i < items.size(); ++i) {
			Item item = items.get(i);
			if (i > 0) {
				if (vertical)
					y += verticalSpacing;
				else
					x += horizontalSpacing;
			}
			item.x = x;
			item.y = y;
			
			if (vertical) {
				y += item.height;
				bounds.width = Math.max(bounds.width, item.width);
				bounds.height = y;
			}
			else {
				x += item.width;
				bounds.width  = x;
				bounds.height = Math.max(bounds.height, item.height);
			}
		}
		
		// calculate legend position
		int dx, dy;
		switch (anchor) {
		case North:		dx = 0; dy = -1; break;
		case NorthEast:	dx = 1; dy = -1; break; 
		case East:		dx = 1; dy = 0; break;
		case SouthEast:	dx = 1; dy = 1; break;
		case South:		dx = 0; dy = 1; break;
		case SouthWest:	dx = -1; dy = 1; break;
		case West:		dx = -1; dy = 0; break;
		case NorthWest:	dx = -1; dy = -1; break;
		default: throw new IllegalStateException();
		}
		
		int top = parent.y;
		int left = parent.x;
		int bottom = parent.y + parent.height;
		int right = parent.x + parent.width;
		
		switch (position) {
		case Above:
			bounds.x = left + (parent.width - bounds.width) * (dx + 1) / 2;
			bounds.y = top + verticalMargin;
			top = Math.min(top + bounds.height + 2 * verticalMargin, bottom);
			break;
		case Below:
			bounds.x = left + (parent.width - bounds.width) * (dx + 1) / 2;
			bounds.y = bottom - verticalMargin - bounds.height;
			bottom = Math.max(bottom - bounds.height - 2 * verticalMargin, top);
			break;
		case Left:
			bounds.x = left + horizontalMargin;
			bounds.y = top + (parent.height - bounds.height) * (dy + 1) / 2;
			left = Math.min(left + bounds.width + 2 * horizontalMargin, right);
			break;
		case Right:
			bounds.x = right - bounds.width - horizontalMargin;
			bounds.y = top + (parent.height - bounds.height) * (dy + 1) / 2;
			right = Math.max(right - bounds.width - 2 * horizontalMargin, left);
			break;
		case Inside:
			bounds.x = left + (parent.width - bounds.width) * (dx + 1) / 2;
			bounds.y = top + (parent.height - bounds.height) * (dy + 1) / 2;
			break;
		default:
			throw new IllegalStateException();
		}
		
		return new Rectangle(left, top, right - left, bottom - top);
	}
	
	/**
	 * Draws the legend to the canvas.
	 */
	public void draw(GC gc) {
		if (!visible)
			return;
		Graphics graphics = new SWTGraphics(gc);
		graphics.pushState();
		
		// draw background and border
		graphics.setBackgroundColor(ColorFactory.asColor("white"));
		graphics.fillRectangle(bounds);
		if (drawBorder) {
			graphics.setForegroundColor(ColorFactory.asColor("black"));
			graphics.drawRectangle(bounds);
		}
		// draw items
		int left = bounds.x;
		int top = bounds.y;
		for (int i = 0; i < items.size(); i++) {
			Item item = items.get(i);
			item.draw(gc, left+item.x, top + item.y);
		}

		graphics.popState();
		graphics.dispose();
	}
}
