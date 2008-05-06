package org.omnetpp.scave.charting;

import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.TextLayout;
import org.omnetpp.scave.charting.properties.ChartDefaults;


/**
 * This class layouts and draws a title on a chart.
 *
 * @author tomi
 */
public class Title {

	private String text;
	private Font font;
	private Color color = ChartDefaults.DEFAULT_TITLE_COLOR;
	private Rectangle bounds;
	
	public Title() {
	}
	
	public Title(String text, Font font) {
		this.text = text;
		this.font = font;
	}
	
	public String getText() {
		return text;
	}
	
	public void setText(String text) {
		this.text = text;
	}
	
	public Font getFont() {
		return font;
	}
	
	public void setFont(Font font) {
		this.font = font;
	}
	
	public Rectangle layout(GC gc, Rectangle parent) {
		if (text == null || text.length() == 0)
			return parent;
		
		TextLayout textLayout = new TextLayout(gc.getDevice());
		textLayout.setFont(font);
		textLayout.setText(text);
		textLayout.setWidth(parent.width);
		textLayout.setAlignment(SWT.CENTER);
		org.eclipse.swt.graphics.Rectangle rect = textLayout.getBounds();
		textLayout.dispose();
		bounds = new Rectangle(parent.x + (parent.width - rect.width) / 2, parent.y, rect.width, rect.height);
		
		return new Rectangle(parent.x, Math.min(bounds.y + bounds.height, parent.y + parent.height),
				parent.width, Math.max(parent.height - bounds.height, 0));
	}
	
	public void draw(GC gc) {
		if (text == null || text.length() == 0 || bounds == null)
			return;
		
		TextLayout textLayout = new TextLayout(gc.getDevice());
		textLayout.setFont(font);
		textLayout.setText(text);
		textLayout.setWidth(bounds.width);
		textLayout.setAlignment(SWT.CENTER);
		gc.setForeground(color);
		textLayout.draw(gc, bounds.x, bounds.y);
		textLayout.dispose();
	}
}
