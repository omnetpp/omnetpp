package org.omnetpp.simulation.figures;

import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;

/**
 *
 * @author Andras
 */
public class FigureUtils extends org.omnetpp.figures.misc.FigureUtils {
    private static final int BORDER = 6;
    private static final int CORNER_SIZE = 20;

    /**
     * An implementation for IInspectorPart.getDragOperation():
     * 6-pixel wide resize border; 10-pixel corner resize handles; top edge will move, other edges will resize.
     */
    public static int getBorderResizeTopBorderMoveDragOperation(int x, int y, Rectangle bounds) {
        if (Rectangle.SINGLETON.setBounds(bounds).shrink(BORDER, BORDER).contains(x, y))
            return 0;
        int result = 0;
        if (Math.abs(bounds.y - y) < CORNER_SIZE)  result = SWT.LEFT|SWT.TOP|SWT.RIGHT|SWT.BOTTOM;
        if (Math.abs(bounds.x - x) < CORNER_SIZE) result |= SWT.LEFT;
        if (Math.abs(bounds.right() - x) < CORNER_SIZE) result |= SWT.RIGHT;
        if (Math.abs(bounds.bottom() - y) < CORNER_SIZE) result |= SWT.BOTTOM;
        return result;
    }

    /**
     * An implementation for IInspectorPart.getDragOperation():
     * 6-pixel wide resize border; 10-pixel corner resize handles.
     */
    public static int getBorderResizeDragOperation(int x, int y, Rectangle bounds) {
        if (Rectangle.SINGLETON.setBounds(bounds).shrink(BORDER, BORDER).contains(x, y))
            return 0;
        int result = 0;
        if (Math.abs(bounds.y - y) < CORNER_SIZE)  result = SWT.TOP;
        if (Math.abs(bounds.x - x) < CORNER_SIZE) result |= SWT.LEFT;
        if (Math.abs(bounds.right() - x) < CORNER_SIZE) result |= SWT.RIGHT;
        if (Math.abs(bounds.bottom() - y) < CORNER_SIZE) result |= SWT.BOTTOM;
        return result;
    }

    /**
     * An implementation for IInspectorPart.getDragOperation():
     * 6-pixel wide resize border; 10-pixel corner resize handles; inside of figure will move.
     */
    public static int getBorderResizeInsideMoveDragOperation(int x, int y, Rectangle bounds) {
        if (Rectangle.SINGLETON.setBounds(bounds).shrink(BORDER, BORDER).contains(x, y))
            return SWT.LEFT|SWT.TOP|SWT.RIGHT|SWT.BOTTOM;
        int result = 0;
        if (Math.abs(bounds.y - y) < CORNER_SIZE)  result = SWT.TOP;
        if (Math.abs(bounds.x - x) < CORNER_SIZE) result |= SWT.LEFT;
        if (Math.abs(bounds.right() - x) < CORNER_SIZE) result |= SWT.RIGHT;
        if (Math.abs(bounds.bottom() - y) < CORNER_SIZE) result |= SWT.BOTTOM;
        return result;
    }


    /**
     * An implementation for IInspectorPart.getDragOperation():
     * 6-pixel wide border which will move the figure.
     */
    public static int getBorderMoveOnlyDragOperation(int x, int y, Rectangle bounds) {
        if (Rectangle.SINGLETON.setBounds(bounds).shrink(BORDER, BORDER).contains(x, y))
            return 0;
        return SWT.LEFT|SWT.TOP|SWT.RIGHT|SWT.BOTTOM;
    }

    /**
     * An implementation for IInspectorPart.getDragOperation():
     * grab anywhere, move-only.
     */
    public static int getMoveOnlyDragOperation(int x, int y, Rectangle bounds) {
        return SWT.LEFT|SWT.TOP|SWT.RIGHT|SWT.BOTTOM;
    }
}
