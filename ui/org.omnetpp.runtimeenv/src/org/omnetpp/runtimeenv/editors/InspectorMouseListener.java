package org.omnetpp.runtimeenv.editors;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.MouseEvent;
import org.eclipse.draw2d.MouseListener;
import org.eclipse.draw2d.MouseMotionListener;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;


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

    public InspectorMouseListener(IInspectorPart inspectorPart) {
        this.inspectorPart = inspectorPart;
        inspectorPart.getFigure().addMouseListener(this);
        inspectorPart.getFigure().addMouseMotionListener(this);
    }

    @Override
    public void mouseDoubleClicked(MouseEvent me) { 
    }

    @Override
    public void mousePressed(MouseEvent me) {
        dragOperation = 0;
        if (inspectorPart.isSelected()) {
            IInspectorFigure figure = inspectorPart.getFigure();
            dragStart = me.getLocation();
            dragStartFigureBounds = figure.getBounds().getCopy();
            dragOperation = figure.getDragOperation(me.x, me.y);
            me.consume(); // otherwise dragging too fast breaks the drag (!!??)
        }
    }

    @Override
    public void mouseReleased(MouseEvent me) {
    }

    private static int limit(int x, int min, int max) {
    	if (x<min) x = min;
    	if (x>max) x = max;
    	return x;
    }
    
    @Override
    public void mouseDragged(MouseEvent me) {
        IFigure figure = inspectorPart.getFigure();
        if (dragOperation != 0) {
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
                    r.width = limit(r.width-dx, min.width, max.width);
                    r.x = dragStartFigureBounds.right() - r.width;
                }
                if ((dragOperation&SWT.TOP) != 0) {
                    r.height = limit(r.height-dy, min.height, max.height);
                    r.y = dragStartFigureBounds.bottom() - r.height;
                }
                if ((dragOperation&SWT.RIGHT) != 0) {
                    r.width = limit(r.width+dx, min.width, max.width);
                }
                if ((dragOperation&SWT.BOTTOM) != 0) {
                    r.height = limit(r.height+dy, min.height, max.height);
                }
            }
            figure.getParent().setConstraint(figure, r);
        }
    }

    @Override
    public void mouseEntered(MouseEvent me) {
    }

    @Override
    public void mouseExited(MouseEvent me) {
    }

    @Override
    public void mouseHover(MouseEvent me) {
    }

    @Override
    public void mouseMoved(MouseEvent me) {
    }
}
