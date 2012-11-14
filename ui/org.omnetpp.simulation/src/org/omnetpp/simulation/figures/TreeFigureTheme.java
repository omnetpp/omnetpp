package org.omnetpp.simulation.figures;

import org.eclipse.core.runtime.Platform;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.widgets.Display;

/**
 * Abstracts platform differences in tree item rendering.
 *
 * @author Andras
 */
public class TreeFigureTheme {
    // note: this is just a 1st approximation, e.g. Windows uses different colors from this in trees
    private static final Color SELECTION_BACKGROUND_COLOR = Display.getDefault().getSystemColor(SWT.COLOR_LIST_SELECTION);
    private static final Color SELECTION_FOREGROUND_COLOR = Display.getDefault().getSystemColor(SWT.COLOR_LIST_SELECTION_TEXT);

    public void paintBackground(Graphics graphics, Rectangle r, int imageIndent, boolean selected, boolean mouseOver, boolean active) {
        if (selected || mouseOver) {
            graphics.setBackgroundColor(SELECTION_BACKGROUND_COLOR);
            boolean shouldIndent = Platform.getOS().equals(Platform.OS_WIN32);
            if (shouldIndent)
                r = new Rectangle(r.x + imageIndent, r.y, r.width - imageIndent, r.height);
            graphics.fillRectangle(r);
        }
    }

    public void paintToggle(Graphics graphics, Point centerLoc, boolean expanded, boolean selected, boolean mouseOver, boolean active) {
        //TODO ToggleFigure should delegate here
    }

    public Color getSelectionForeground(boolean mouseOver, boolean active) {
        //TODO StyledText should use this for selected text, instead of colors in the styles
        return SELECTION_FOREGROUND_COLOR;
    }

}