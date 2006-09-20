package org.omnetpp.scave2.charting;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.scave2.model.ChartProperties.LegendAnchor;
import org.omnetpp.scave2.model.ChartProperties.LegendPosition;

/**
 * This class layouts and draws the legend on a vector chart.
 *
 * @author tomi
 */
public class Legend {
	
	static class Item {
		String text;
		Color color;
		Rectangle rect; // rectangle of the item relative to the legend
		
		public Item(Color color, String text) {
			this.color = color;
			this.text = text;
		}
	}
	
	private boolean visible;
	private boolean drawBorder;
	private Font font;
	private LegendPosition position;
	private LegendAnchor anchor;
	private int hGap = 5;
	private int vGap = 5;
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
		
		Font saveFont = gc.getFont();
		if (font != null)
			gc.setFont(font);
		
		// position items and calculate size
		bounds = new Rectangle(0, 0, 0, 0);
		int x = 0;
		int y = 0;
		boolean vertical = position == LegendPosition.Left || position == LegendPosition.Right;
		for (int i = 0; i < items.size(); ++i) {
			Item item = items.get(i);
			
			Point size = gc.stringExtent(item.text);
			size.x += 10; // place for mark
			
			if (i > 0) {
				if (vertical)
					y += vGap;
				else
					x += hGap;
			}
			
			item.rect = new Rectangle(x, y, size.x, size.y);
			
			if (vertical) {
				y += size.y;
				bounds.width = Math.max(bounds.width, size.x);
				bounds.height = y;
			}
			else {
				x += size.x;
				bounds.width  = x;
				bounds.height = Math.max(bounds.height, size.y);
			}
		}
		
		gc.setFont(saveFont);
		
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
			bounds.y = top;
			top = Math.min(top + bounds.height, bottom);
			break;
		case Below:
			bounds.x = left + (parent.width - bounds.width) * (dx + 1) / 2;
			bounds.y = bottom - bounds.height;
			bottom = Math.max(bottom - bounds.height, top);
			break;
		case Left:
			bounds.x = left;
			bounds.y = top + (parent.height - bounds.y) * (dy + 1) / 2;
			left = Math.min(left + bounds.width, right);
			break;
		case Right:
			bounds.x = right - bounds.width;
			bounds.y = top + (parent.height - bounds.height) * (dy + 1) / 2;
			right = Math.max(right - bounds.width, left);
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
		Font saveFont = gc.getFont();
		if (font != null)
			gc.setFont(font);
		
		// draw background and border
		gc.setBackground(ColorFactory.asColor("white"));
		gc.fillRectangle(bounds);
		if (drawBorder) {
			gc.setForeground(ColorFactory.asColor("black"));
			gc.drawRectangle(bounds);
		}
		// draw items
		int left = bounds.x;
		int top = bounds.y;
		for (int i = 0; i < items.size(); i++) {
			Item item = items.get(i);
			// draw oval
			gc.setForeground(item.color);
			gc.setBackground(item.color);
			gc.fillOval(left+item.rect.x + 2 , top + item.rect.y + (item.rect.height - 5) / 2, 5, 5);
			// draw text
			gc.setForeground(ColorFactory.asColor("black"));
			gc.drawString(item.text, left + 9 + item.rect.x, top + item.rect.y, true);
		}
		gc.setFont(saveFont);
	}
}
