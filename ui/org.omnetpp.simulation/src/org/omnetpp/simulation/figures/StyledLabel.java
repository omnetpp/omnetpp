package org.omnetpp.simulation.figures;

import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.jface.viewers.StyledString;
import org.eclipse.swt.custom.StyleRange;
import org.eclipse.swt.graphics.TextLayout;
import org.eclipse.swt.widgets.Display;


/**
 * A label that displays a StyledString.
 *
 * StyledString example:
 *
 * <pre>
 * class ColorStyler extends Styler {
 *   Color color;
 *   public ColorStyler(Color color) { this.color = color; }
 *   public void applyStyles(TextStyle textStyle) { textStyle.foreground = color; }
 * };
 * Styler blueStyle = new ColorStyler(ColorFactory.BLUE4);
 * Styler redStyle = new ColorStyler(ColorFactory.RED);
 *
 * StyledString styledString = new StyledString();
 * styledString.append("Hello ", blueStyle);
 * styledString.append("world", redStyle);
 * </pre>
 *
 * @author Andras
 */
//TODO currently there's no gap around the label. If that's needed, use a Border or change the code
//TODO text alignment (currently always left), justification, etc.
public class StyledLabel extends Figure {
    private StyledString styledString;
    private TextLayout textLayout;

    public StyledLabel() {
    }

    public StyledLabel(StyledString styledString) {
        setStyledString(styledString);
    }

    public void setStyledString(StyledString text) {
        if (text.equals(styledString))
            return;
        this.styledString = text;

        // recompute TextLayout
        if (textLayout != null)
            textLayout.dispose();
        textLayout = new TextLayout(Display.getCurrent());
        textLayout.setText(styledString.getString());
        for (StyleRange styleRange : styledString.getStyleRanges())
            textLayout.setStyle(styleRange, styleRange.start, styleRange.start + styleRange.length);

        revalidate();
        repaint();
    }

    @Override
    public Dimension getPreferredSize(int wHint, int hHint) {
        // copy/pasta from Label
        if (prefSize == null) {
            prefSize = calculateTextSize();
            Insets insets = getInsets();
            prefSize.expand(insets.getWidth(), insets.getHeight());
            if (getLayoutManager() != null)
                prefSize.union(getLayoutManager().getPreferredSize(this, wHint, hHint));
        }
        if (wHint >= 0 && wHint < prefSize.width) {
            Dimension minSize = getMinimumSize(wHint, hHint);
            Dimension result = prefSize.getCopy();
            result.width = Math.min(result.width, wHint);
            result.width = Math.max(minSize.width, result.width);
            return result;
        }
        return prefSize;
    }

    private Dimension calculateTextSize() {
        org.eclipse.swt.graphics.Rectangle r = textLayout.getBounds();
        return new Dimension(r.width, r.height);
    }

    @Override
    protected void paintClientArea(Graphics graphics) {
        // like in Label
        if (isOpaque())
            super.paintClientArea(graphics);

        if (textLayout != null) {
            Rectangle area = getClientArea();
            graphics.drawTextLayout(textLayout, area.x, area.y);
        }
    }

    @Override
    protected void finalize() throws Throwable {
        super.finalize();
        if (textLayout != null)
            textLayout.dispose();
    }
}

