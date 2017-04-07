/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.apache.commons.lang3.StringEscapeUtils;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Control;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.common.ui.HtmlHoverInfo;
import org.omnetpp.common.ui.IHoverInfoProvider;
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
        Image image;

        public Item(Color color, String label, IChartSymbol symbol, boolean drawLine) {
            this.color = color;
            this.label = label;
            this.image = SymbolImageFactory.createSymbolImage(symbol, color, drawLine);
        }
    }

    public LegendTooltip(ChartCanvas parent) {
        button = new Button(parent, SWT.FLAT | SWT.CENTER | SWT.NO_FOCUS);
        Image icon = ImageFactory.global().getImage(ImageFactory.TOOLBAR_IMAGE_LEGEND);
        button.setImage(icon);
        button.setSize(button.computeSize(SWT.DEFAULT, SWT.DEFAULT));

        final HoverSupport hoverSupport = new HoverSupport();
        hoverSupport.setHoverSizeConstaints(320,400);
        hoverSupport.adapt(button, new IHoverInfoProvider() {
            @Override
            public HtmlHoverInfo getHoverFor(Control control, int x, int y) {
                return getTooltip(x, y);
            }
        });

        button.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                hoverSupport.makeHoverSticky(button);
            }
        });
    }

    public boolean isVisible() {
        return button.isVisible();
    }

    public void setVisible(boolean visible) {
        button.setVisible(visible);
    }

    public void clearItems() {
        items.clear();
    }

    public void addItem(Color color, String label, IChartSymbol symbol, boolean drawLine) {
        items.add(new Item(color, label, symbol, drawLine));
    }

    public Rectangle layout(Graphics graphics, Rectangle rect) {
        button.setLocation(rect.getRight().x - button.getSize().x - 2, rect.y + 2);
        return rect;
    }

    public void draw(Graphics graphics) {
        // button is drawn as a child of the canvas
    }

    public HtmlHoverInfo getTooltip(int x, int y) {
        if (items.size() > 0) {
            Map<String, Image> imageMap = new HashMap<String, Image>();
            StringBuffer sb = new StringBuffer();
            sb.append("<b>Legend:</b>");
            for (int i = 0; i < items.size(); i++) {
                Item item = items.get(i);
                sb.append("<div style='vertical-align: middle; color: ").append(htmlColor(item.color)).append("'>");
                if (item.image != null) {
                    imageMap.put(String.valueOf(i), item.image);
                    sb.append("<img style='vertical-align: middle' src='" + i + "'/>&nbsp;");
                }
                sb.append(htmlText(item.label)).append("</div>");
            }
            return new HtmlHoverInfo(HoverSupport.addHTMLStyleSheet(sb.toString()), imageMap, null);
        }
        else
            return null;
    }

    public String htmlColor(Color color) {
        return String.format("#%02X%02X%02X", color.getRed(), color.getGreen(), color.getBlue());
    }

    public String htmlText(String text) {
        return StringEscapeUtils.escapeHtml4(text);
    }
}
