package org.omnetpp.runtimeenv.figures;

import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;


public class FigureUtils extends org.omnetpp.figures.misc.FigureUtils {

	/**
	 * A possible implementation for IInspectorFigure.getDragOperation()
	 */
    public static int getBorderMoveResizeDragOperation(int x, int y, Rectangle bounds) {
    	// 6-pixel wide resize border; 10-pixel resize handles; top edge will move, other edges will resize
    	if (Rectangle.SINGLETON.setBounds(bounds).shrink(6, 6).contains(x, y))
    		return 0;
        int result = 0;
        if (Math.abs(bounds.y - y) < 10)  result = SWT.LEFT|SWT.TOP|SWT.RIGHT|SWT.BOTTOM;
        if (Math.abs(bounds.x - x) < 10) result |= SWT.LEFT;
        if (Math.abs(bounds.right() - x) < 10) result |= SWT.RIGHT;
        if (Math.abs(bounds.bottom() - y) < 10) result |= SWT.BOTTOM;
        return result;
    }

	/**
	 * A possible implementation for IInspectorFigure.getDragOperation()
	 */
    public static int getBorderMoveOnlyDragOperation(int x, int y, Rectangle bounds) {
    	// 6-pixel move border
    	if (Rectangle.SINGLETON.setBounds(bounds).shrink(6, 6).contains(x, y))
    		return 0;
        return SWT.LEFT|SWT.TOP|SWT.RIGHT|SWT.BOTTOM;
    }

	/**
	 * A possible implementation for IInspectorFigure.getDragOperation()
	 */
    public static int getMoveOnlyDragOperation(int x, int y, Rectangle bounds) {
    	// grab anywhere, move-only
        return SWT.LEFT|SWT.TOP|SWT.RIGHT|SWT.BOTTOM;
    }
}
