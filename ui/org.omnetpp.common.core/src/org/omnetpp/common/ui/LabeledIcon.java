package org.omnetpp.common.ui;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.PositionConstants;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.TextLayout;
import org.eclipse.swt.widgets.Display;

/**
 * A figure that displays an image and a text below it.
 */
public class LabeledIcon extends Figure implements PositionConstants {
    private Image icon;
    private String text = "";
    private Dimension iconSize; // cached
    private int iconTextGap = 3;
    private org.eclipse.swt.graphics.Rectangle textBounds = null;

    /**
     * Construct an empty Label.
     */
    public LabeledIcon() {
    }

    /**
     * Construct a Label with passed String as text and passed Image as its icon.
     */
    public LabeledIcon(String s, Image i) {
        setText(s);
        setIcon(i);
    }

    /**
     * Sets the label's icon to the passed image.
     */
    public void setIcon(Image image) {
        if (icon == image)
            return;
        icon = image;
        iconSize = new Dimension(icon.getBounds().width, icon.getBounds().height);
        repaint();
    }

    /**
     * Returns the Label's icon.
     */
    public Image getIcon() {
        return icon;
    }

    /**
     * Sets the label's text.
     */
    public void setText(String s) {
        if (s == null)
            s = "";
        if (text.equals(s))
            return;
        text = s;
        revalidate();
        repaint();
    }
    
    /**
     * Returns the text of the label.
     */
    public String getText() {
        return text;
    }

    public Rectangle getTextBounds() {
        Rectangle figureBounds = getBounds();
        return new Rectangle(figureBounds.x + textBounds.x, figureBounds.bottom() + textBounds.y - textBounds.height, textBounds.width, textBounds.height);
    }

    /**
     * @see IFigure#getPreferredSize(int, int)
     */
    public Dimension getPreferredSize(int wHint, int hHint) {
        Assert.isNotNull(icon);
        Dimension size = prefSize == null ? new Dimension() : prefSize.getCopy();
        if (size.width == -1)
            size.width = iconSize.width; // let the icon define the width
        if (size.height == -1) {
            TextLayout textLayout = new TextLayout(Display.getCurrent());
            textLayout.setText(getText());
            textLayout.setAlignment(SWT.CENTER);
            textLayout.setWidth(size.width);
            int textHeight = textLayout.getBounds().height;
            textLayout.dispose();
            size.height = iconSize.height + iconTextGap + textHeight;
        }
        return size;
    }

    /**
     * @see Figure#paintFigure(Graphics)
     */
    protected void paintFigure(Graphics graphics) {
        Assert.isNotNull(icon);
        if (isOpaque())
            super.paintFigure(graphics);

        Rectangle figureBounds = getBounds();
        graphics.translate(figureBounds.x, figureBounds.y);
        graphics.drawImage(icon, figureBounds.width/2 - iconSize.width/2, 0);

        TextLayout textLayout = new TextLayout(Display.getCurrent());
        textLayout.setText(getText());
        textLayout.setAlignment(SWT.CENTER);
        textLayout.setWidth(figureBounds.width);
        textBounds = textLayout.getBounds();
        graphics.drawTextLayout(textLayout, figureBounds.width/2 - textBounds.width/2, iconSize.height + iconTextGap);
        textLayout.dispose();

        graphics.translate(-figureBounds.x, -figureBounds.y);
    }

}
