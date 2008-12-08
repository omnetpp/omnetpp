package org.omnetpp.scave.charting;

import java.util.ArrayList;
import java.util.List;

import org.apache.commons.lang.StringEscapeUtils;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.jface.text.IInformationControl;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Control;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.common.ui.IHoverTextProvider;
import org.omnetpp.common.ui.SizeConstraint;
import org.omnetpp.scave.charting.plotter.IChartSymbol;

/**
 * Legend tooltip.
 */
class LegendTooltip implements ILegend
{
	Button button;
	List<Item> items = new ArrayList<Item>();
	
	static class Item
	{
		Color color;
		String label;
		String imageFile;

		public Item(Color color, String label, IChartSymbol symbol, boolean drawLine) {
			this.color = color;
			this.label = label;
			this.imageFile = SymbolImageFactory.getImageFile(color, symbol, drawLine);
		}
	}
	
	public LegendTooltip(ChartCanvas parent) {
		button = new Button(parent, SWT.FLAT | SWT.CENTER | SWT.NO_FOCUS);
		Image icon = ImageFactory.getImage(ImageFactory.TOOLBAR_IMAGE_LEGEND);
		button.setImage(icon);
		button.setSize(button.computeSize(SWT.DEFAULT, SWT.DEFAULT));
		
		final HoverSupport hoverSupport = new HoverSupport();
		hoverSupport.setHoverSizeConstaints(320,400);
		hoverSupport.adapt(button, new IHoverTextProvider() {
			public String getHoverTextFor(Control control, int x, int y, SizeConstraint preferredSize) {
				return getTooltipText(x, y, preferredSize);
			}
		});
		
		button.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
					IInformationControl infoControl = hoverSupport.getInformationControl();
					if (infoControl == null)
						hoverSupport.makeHoverSticky();
			}
		});
	}
	
	public void clearItems() {
		items.clear();
	}
	
	public void addItem(Color color, String label, IChartSymbol symbol, boolean drawLine) {
		items.add(new Item(color, label, symbol, drawLine));
	}
	
	public Rectangle layout(GC gc, Rectangle rect) {
		button.setLocation(rect.width - button.getSize().x - 2, rect.y + 2);
		return rect;
	}
	
	public void draw(GC gc) {
		// button is drawn as a child of the canvas
	}
	
	public String getTooltipText(int x, int y, SizeConstraint preferredSize) {
		if (items.size() > 0) {
			StringBuffer sb = new StringBuffer();
			int height = 20;
			sb.append("<b>Legend:</b>"); height += 17;
			sb.append("<table style='margin-left: 1em'>");
			for (Item item : items) {
				sb.append("<tr>");
				if (item.imageFile != null)
					sb.append("<td style='vertical-align: middle'>").
						append("<img src='file://").append(item.imageFile).append("'></td>"); // Note: URLEncoded filename does not work in IE
				sb.append("<td style='vertical-align: middle; color: ").append(htmlColor(item.color)).append("'>").
					append(htmlText(item.label)).append("</td>");
				sb.append("</tr>");
				height += 17;
			}
			sb.append("</table>");
			preferredSize.preferredHeight = Math.max(height, 80);
			return HoverSupport.addHTMLStyleSheet(sb.toString());
		}
		else
			return null;
	}
	
	public String htmlColor(Color color) {
		return String.format("#%02X%02X%02X", color.getRed(), color.getGreen(), color.getBlue());
	}
	
	public String htmlText(String text) {
		return StringEscapeUtils.escapeHtml(text);
	}
}
