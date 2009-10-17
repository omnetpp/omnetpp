package org.omnetpp.runtimeenv.editors;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.MouseEvent;
import org.eclipse.draw2d.MouseListener;
import org.eclipse.draw2d.MouseMotionListener;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Cursor;
import org.eclipse.swt.widgets.Display;


/**
 * 
 * @author Andras 
 */
//TODO mouse cursor! (resize arrows, etc)
public class InspectorMouseListener implements MouseListener, MouseMotionListener {
    protected IInspectorPart inspectorPart;
    protected Point dragStart;
    protected Rectangle dragStartFigureBounds;
    protected int dragOperation; // SWT.LEFT/RIGHT/TOP/BOTTOM; move if all are set

    private static Cursor CURSOR_SIZEE = new Cursor(Display.getDefault(), SWT.CURSOR_SIZEE);
    private static Cursor CURSOR_SIZEW = new Cursor(Display.getDefault(), SWT.CURSOR_SIZEW);
    private static Cursor CURSOR_SIZEN = new Cursor(Display.getDefault(), SWT.CURSOR_SIZEN);
    private static Cursor CURSOR_SIZES = new Cursor(Display.getDefault(), SWT.CURSOR_SIZES);
    private static Cursor CURSOR_SIZENE = new Cursor(Display.getDefault(), SWT.CURSOR_SIZENE);
    private static Cursor CURSOR_SIZENW = new Cursor(Display.getDefault(), SWT.CURSOR_SIZENW);
    private static Cursor CURSOR_SIZESE = new Cursor(Display.getDefault(), SWT.CURSOR_SIZESE);
    private static Cursor CURSOR_SIZESW = new Cursor(Display.getDefault(), SWT.CURSOR_SIZESW);
    private static Cursor CURSOR_SIZEALL = new Cursor(Display.getDefault(), SWT.CURSOR_SIZEALL);
    private static Cursor CURSOR_ARROW = new Cursor(Display.getDefault(), SWT.CURSOR_ARROW);
    
    public InspectorMouseListener(IInspectorPart inspectorPart) {
        this.inspectorPart = inspectorPart;
        inspectorPart.getFigure().addMouseListener(this);
        inspectorPart.getFigure().addMouseMotionListener(this);
    }

    //@Override
    public void mouseDoubleClicked(MouseEvent me) { 
    }

    //@Override
    public void mousePressed(MouseEvent me) {
        dragOperation = 0;
//        if (inspectorPart.isSelected()) {
            IInspectorFigure figure = inspectorPart.getFigure();
            dragStart = me.getLocation();
            dragStartFigureBounds = figure.getBounds().getCopy();
            dragOperation = figure.getDragOperation(me.x, me.y);
            me.consume(); // otherwise dragging too fast breaks the drag (!!??)
//        }
    }

    //@Override
    public void mouseReleased(MouseEvent me) {
    	dragOperation = 0;
    	dragStart = null;
    }

    private static int clip(int x, int min, int max) {
    	if (x<min) x = min;
    	if (x>max) x = max;
    	return x;
    }
    
    //@Override
    public void mouseDragged(MouseEvent me) {
        IFigure figure = inspectorPart.getFigure();
        if (dragOperation != 0 && dragStart != null) {
            int dx = me.x - dragStart.x;
            int dy = me.y - dragStart.y;
            Rectangle r = dragStartFigureBounds.getCopy();
            Dimension min = figure.getMinimumSize();
            Dimension max = figure.getMaximumSize();
            if (dragOperation == (SWT.LEFT|SWT.RIGHT|SWT.TOP|SWT.BOTTOM)) {
                r.x += dx; 
                r.y += dy;
            }
            else {
                if ((dragOperation&SWT.LEFT) != 0) {
                    r.width = clip(r.width-dx, min.width, max.width);
                    r.x = dragStartFigureBounds.right() - r.width;
                }
                if ((dragOperation&SWT.TOP) != 0) {
                    r.height = clip(r.height-dy, min.height, max.height);
                    r.y = dragStartFigureBounds.bottom() - r.height;
                }
                if ((dragOperation&SWT.RIGHT) != 0) {
                    r.width = clip(r.width+dx, min.width, max.width);
                }
                if ((dragOperation&SWT.BOTTOM) != 0) {
                    r.height = clip(r.height+dy, min.height, max.height);
                }
            }
            figure.getParent().setConstraint(figure, r);
        }
    }

    //@Override
    public void mouseEntered(MouseEvent me) {
    }

    //@Override
    public void mouseExited(MouseEvent me) {
    }

    //@Override
    public void mouseHover(MouseEvent me) {
    }

    //@Override
    public void mouseMoved(MouseEvent me) {
    	IInspectorFigure figure = inspectorPart.getFigure();
    	dragOperation = figure.getDragOperation(me.x, me.y);

    	Cursor cursor;
    	switch (dragOperation) {
    	case SWT.LEFT: cursor = CURSOR_SIZEW; break;
    	case SWT.RIGHT: cursor = CURSOR_SIZEE; break;
    	case SWT.TOP: cursor = CURSOR_SIZEN; break;
    	case SWT.BOTTOM: cursor = CURSOR_SIZES; break;
    
    	case SWT.LEFT|SWT.TOP: cursor = CURSOR_SIZENW; break;
    	case SWT.RIGHT|SWT.TOP: cursor = CURSOR_SIZENE; break;
    	case SWT.LEFT|SWT.BOTTOM: cursor = CURSOR_SIZESW; break;
    	case SWT.RIGHT|SWT.BOTTOM: cursor = CURSOR_SIZESE; break;
    
    	case SWT.LEFT|SWT.RIGHT|SWT.TOP|SWT.BOTTOM: cursor = CURSOR_SIZEALL; break;
    	default: cursor = CURSOR_ARROW;
    	}
    	figure.setCursor(cursor);
    }
}
