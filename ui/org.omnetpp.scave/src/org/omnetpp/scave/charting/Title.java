/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.swt.graphics.TextLayout;
import org.eclipse.swt.graphics.TextStyle;
import org.eclipse.swt.widgets.Display;


/**
 * This class layouts and draws a title on a chart.
 *
 * @author tomi
 */
public class Title {

    private boolean visible = true;
    private String text;
    private Font font;
    private Color color;
    private Rectangle bounds;

    public Title() {
    }

    public Title(String text, Font font) {
        this.text = text;
        this.font = font;
    }

    public void setVisible(boolean value) {
        visible = value;
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

    public void setColor(RGB color) {
        this.color = new Color(null, color);
    }

    public Rectangle layout(Graphics graphics, Rectangle parent) {
        if (!visible || text == null || text.length() == 0)
            return parent;

        TextLayout textLayout = new TextLayout(Display.getDefault());
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

    public void draw(Graphics graphics) {
        if (!visible || text == null || text.length() == 0 || bounds == null)
            return;

        TextLayout textLayout = new TextLayout(Display.getDefault());
        textLayout.setText(text);
        textLayout.setWidth(bounds.width);
        textLayout.setAlignment(SWT.CENTER);
        textLayout.setStyle(new TextStyle(font, color, null), 0, Integer.MAX_VALUE);
        graphics.setFont(font);
        graphics.setForegroundColor(color);
        int[] lineOffsets = textLayout.getLineOffsets();
        for (int i = 0; i < textLayout.getLineCount(); i++) {
            Rectangle lineBounds = new Rectangle(textLayout.getLineBounds(i));
            graphics.drawText(text.substring(lineOffsets[i], lineOffsets[i + 1]), lineBounds.x, lineBounds.y);
        }
        textLayout.dispose();
    }
}
