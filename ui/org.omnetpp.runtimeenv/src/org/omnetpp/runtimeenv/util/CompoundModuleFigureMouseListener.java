package org.omnetpp.runtimeenv.util;

import org.eclipse.draw2d.MouseEvent;
import org.eclipse.draw2d.MouseListener;
import org.eclipse.draw2d.MouseMotionListener;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.runtimeenv.figures.CompoundModuleFigureEx;


/**
 * 
 * @author Andras 
 */
//TODO mouse cursor! (resize arrows, etc)
public class CompoundModuleFigureMouseListener implements MouseListener, MouseMotionListener {
    protected CompoundModuleFigureEx figure;
    protected Point dragStart;
    protected Rectangle dragStartFigureBounds;
    protected boolean dragLeft, dragRight, dragTop, dragBottom, dragMove;

    public CompoundModuleFigureMouseListener(CompoundModuleFigureEx figure) {
        this.figure = figure;
        figure.addMouseListener(this);
        figure.addMouseMotionListener(this);
    }
    
    @Override
    public void mouseDoubleClicked(MouseEvent me) { 
    }

    @Override
    public void mousePressed(MouseEvent me) {
        dragLeft = dragTop = dragRight = dragBottom = dragMove = false;
        if (figure.getDragHandlesShown()) {
            dragStart = me.getLocation();
            dragStartFigureBounds = figure.getBounds().getCopy();
            dragLeft = Math.abs(figure.getBounds().x - me.x) < 5;
            dragTop = Math.abs(figure.getBounds().y - me.y) < 5;
            dragRight = Math.abs(figure.getBounds().right() - me.x) < 5;
            dragBottom = Math.abs(figure.getBounds().bottom() - me.y) < 5;
            dragMove = !(dragLeft || dragRight || dragTop || dragBottom);
            me.consume(); // otherwise dragging too fast breaks the drag (!!??)
        }
    }

    @Override
    public void mouseReleased(MouseEvent me) {
    }

    @Override
    public void mouseDragged(MouseEvent me) {
        if (dragMove || dragLeft || dragRight || dragTop || dragBottom) {
            int dx = me.x - dragStart.x;
            int dy = me.y - dragStart.y;
            Rectangle r = dragStartFigureBounds.getCopy();
            Dimension min = figure.getMinimumSize();
            if (dragMove) {
                r.x += dx; 
                r.y += dy;
            }
            if (dragLeft) {
                r.width = Math.max(r.width-dx, min.width);
                r.x = dragStartFigureBounds.right() - r.width;
            }
            if (dragTop) {
                r.height = Math.max(r.height-dy, min.height);
                r.y = dragStartFigureBounds.bottom() - r.height;
            }
            if (dragRight) {
                r.width = Math.max(r.width+dx, min.width);
            }
            if (dragBottom) {
                r.height = Math.max(r.height+dy, min.height);
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
