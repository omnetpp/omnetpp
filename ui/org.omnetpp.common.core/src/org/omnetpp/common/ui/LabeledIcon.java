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
    private boolean horizontal = false;
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
     * Choose between horizontal layout (text on the right of the icon) and
     * vertical layout (text below the icon, both center-aligned/justified).
     */
    public void setHorizontalLayout(boolean horizontal) {
        this.horizontal = horizontal;
        repaint();
    }

    public boolean isHorizontalLayout() {
        return horizontal;
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
        return new Rectangle(figureBounds.x + textBounds.x, figureBounds.y + textBounds.y, textBounds.width, textBounds.height);
    }

    private TextLayout makeTextLayout(int width) {
        TextLayout textLayout = new TextLayout(Display.getCurrent());
        textLayout.setText(getText());
        textLayout.setAlignment(horizontal ? SWT.LEFT : SWT.CENTER);
        textLayout.setWidth(width);
        return textLayout;
    }

    private org.eclipse.swt.graphics.Rectangle measureText(int width) {
        TextLayout textLayout = makeTextLayout(width);
        org.eclipse.swt.graphics.Rectangle result = textLayout.getBounds();
        textLayout.dispose();
        return result;
    }

    private static void setRectanglePos(org.eclipse.swt.graphics.Rectangle rect, int x, int y) {
        rect.x = x;
        rect.y = y;
    }

    /**
     * @see IFigure#getPreferredSize(int, int)
     */
    public Dimension getPreferredSize(int wHint, int hHint) {
        Assert.isNotNull(icon);
        Dimension size = prefSize == null ? new Dimension() : prefSize.getCopy();
        if (horizontal) {
            org.eclipse.swt.graphics.Rectangle textSize = measureText(size.width == -1 ? -1 : Math.max(10, size.width - iconSize.width - iconTextGap));
            if (size.width == -1)
                size.width = iconSize.width + iconTextGap + textSize.width;
            if (size.height == -1)
                size.height = Math.max(iconSize.height, textSize.height);
        }
        else {
            if (size.width == -1)
                size.width = iconSize.width; // let the icon define the width
            if (size.height == -1) {
                int textHeight = measureText(size.width).height;
                size.height = iconSize.height + iconTextGap + textHeight;
            }
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

        if (horizontal) {
            graphics.drawImage(icon, 0, figureBounds.height/2 - iconSize.height/2);

            TextLayout textLayout = makeTextLayout(figureBounds.width - (iconSize.width + iconTextGap));
            textBounds = textLayout.getBounds();
            setRectanglePos(textBounds, iconSize.width + iconTextGap, figureBounds.height/2 - textBounds.height/2);
            graphics.drawTextLayout(textLayout, textBounds.x, textBounds.y);
            textLayout.dispose();
        }
        else {
            graphics.drawImage(icon, figureBounds.width/2 - iconSize.width/2, 0);

            TextLayout textLayout = makeTextLayout(figureBounds.width);
            textBounds = textLayout.getBounds();
            setRectanglePos(textBounds, figureBounds.width/2 - textBounds.width/2, iconSize.height + iconTextGap);
            graphics.drawTextLayout(textLayout, textBounds.x, textBounds.y);
            textLayout.dispose();
        }

        graphics.translate(-figureBounds.x, -figureBounds.y);
    }

    @Override
    public String toString() {
        return getClass().getSimpleName() + "('" + getText() + "')";
    }
}
