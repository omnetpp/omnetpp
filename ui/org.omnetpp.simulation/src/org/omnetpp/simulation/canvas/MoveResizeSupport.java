package org.omnetpp.simulation.canvas;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.MouseEvent;
import org.eclipse.draw2d.MouseListener;
import org.eclipse.draw2d.MouseMotionListener;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseMoveListener;
import org.eclipse.swt.events.MouseTrackAdapter;
import org.eclipse.swt.graphics.Cursor;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.simulation.inspectors.IInspectorPart;


/**
 *
 * @author Andras
 */
public class MoveResizeSupport {
    private static final Cursor CURSOR_SIZEE = new Cursor(Display.getDefault(), SWT.CURSOR_SIZEE); //TODO into some share resource file
    private static final Cursor CURSOR_SIZEW = new Cursor(Display.getDefault(), SWT.CURSOR_SIZEW);
    private static final Cursor CURSOR_SIZEN = new Cursor(Display.getDefault(), SWT.CURSOR_SIZEN);
    private static final Cursor CURSOR_SIZES = new Cursor(Display.getDefault(), SWT.CURSOR_SIZES);
    private static final Cursor CURSOR_SIZENE = new Cursor(Display.getDefault(), SWT.CURSOR_SIZENE);
    private static final Cursor CURSOR_SIZENW = new Cursor(Display.getDefault(), SWT.CURSOR_SIZENW);
    private static final Cursor CURSOR_SIZESE = new Cursor(Display.getDefault(), SWT.CURSOR_SIZESE);
    private static final Cursor CURSOR_SIZESW = new Cursor(Display.getDefault(), SWT.CURSOR_SIZESW);
    //private static final Cursor CURSOR_SIZEALL = new Cursor(Display.getDefault(), SWT.CURSOR_SIZEALL);
    //private static final Cursor CURSOR_HAND = new Cursor(Display.getDefault(), SWT.CURSOR_HAND);
    //private static final Cursor CURSOR_DRAGGINGHAND = CustomCursors.DRAGGER;
    private static final Cursor CURSOR_ARROW = new Cursor(Display.getDefault(), SWT.CURSOR_ARROW);

    private SimulationCanvas simulationCanvas;
    private MouseListener inspectorMouseListener;
    private MouseMotionListener inspectorMouseMotionListener;
    private org.eclipse.swt.events.MouseListener swtMouseListener;
    private org.eclipse.swt.events.MouseMoveListener swtMouseMoveListener;
    private org.eclipse.swt.events.MouseTrackAdapter swtMouseTrackListener;

    private Point dragStart; // mouse when drag starts
    private Rectangle dragStartInspectorFigureBounds;  // bounds of inspector's *root* figure when drag starts
    private int dragOperation; // SWT.LEFT/RIGHT/TOP/BOTTOM; move if all are set

    protected boolean backgroundDragged;
    protected Point backgroundDragInitialScroll;

    /**
     * Constructor
     */
    public MoveResizeSupport(SimulationCanvas canvas) {
        this.simulationCanvas = canvas;

        inspectorMouseListener = new MouseListener.Stub() {
            @Override
            public void mousePressed(MouseEvent me) {
                IFigure figure = (IFigure) me.getSource();
                IInspectorPart inspector = simulationCanvas.findInspectorFor(figure);
                dragStart = me.getLocation();
                dragStartInspectorFigureBounds = inspector.getFigure().getBounds().getCopy();
                dragOperation = inspector.getDragOperation(figure, me.x, me.y);
                me.consume(); // otherwise dragging too fast breaks the drag (!!??)

                if ((me.getState()&SWT.CONTROL) != 0)
                    simulationCanvas.toggleSelection(inspector);
                else
                    simulationCanvas.select(inspector, true);

                inspector.raiseToTop();

                // By default draw2d updates canvas bounds continually while dragging, which
                // results in a weird effect: when dragging up an inspector from near the bottom
                // of the canvas and the canvas shrinks as a result, the mouse overtakes the
                // drag handle and other interesting things happen. The workaround is to disable
                // updating the canvas bounds while dragging; this can be achieved by setting
                // preferredSize. (See also DragHandle.)
                IFigure contents = simulationCanvas.getContents();
                contents.setPreferredSize(contents.getPreferredSize());
            }

            @Override
            public void mouseReleased(MouseEvent me) {
                dragOperation = 0;
                dragStart = null;
                simulationCanvas.getContents().setPreferredSize(null);  // restore original state
            }
        };

        inspectorMouseMotionListener = new MouseMotionListener.Stub() {
            @Override
            public void mouseMoved(MouseEvent me) {
                // update cursor
                IFigure figure = (IFigure) me.getSource();
                IInspectorPart inspector = simulationCanvas.findInspectorFor(figure);
                int dragOperation = inspector.getDragOperation(figure, me.x, me.y);
                Cursor cursor = getCursor(dragOperation);
                figure.setCursor(cursor);
            }

            @Override
            public void mouseDragged(MouseEvent me) {
                // set bounds of the inspector's main figure, via setConstraints()
                if (dragOperation != 0 && dragStart != null) {
                    int dx = me.x - dragStart.x;
                    int dy = me.y - dragStart.y;

                    IFigure figure = (IFigure) me.getSource();
                    IInspectorPart inspector = simulationCanvas.findInspectorFor(figure);
                    moveOrResizeInspectorFigure(dx, dy, inspector);
                }
            }
        };

        swtMouseListener = new MouseAdapter() {
            @Override
            public void mouseDown(org.eclipse.swt.events.MouseEvent e) {
                Control control = (Control) e.widget;
                IInspectorPart inspector = simulationCanvas.findInspectorFor(control);
                org.eclipse.swt.graphics.Point mouse = Display.getCurrent().getCursorLocation();
                dragStart = new Point(mouse.x, mouse.y);  // note: don't use (e.x,e.y) because they are control-relative and the control itself will move!
                dragStartInspectorFigureBounds = inspector.getFigure().getBounds().getCopy();
                dragOperation = inspector.getDragOperation(control, e.x, e.y);

                if ((e.stateMask&SWT.CONTROL) != 0)
                    simulationCanvas.toggleSelection(inspector);
                else
                    simulationCanvas.select(inspector, true);

                inspector.raiseToTop();

                // By default draw2d updates canvas bounds continually while dragging, which
                // results in a weird effect: when dragging up an inspector from near the bottom
                // of the canvas and the canvas shrinks as a result, the mouse overtakes the
                // drag handle and other interesting things happen. The workaround is to disable
                // updating the canvas bounds while dragging; this can be achieved by setting
                // preferredSize. (See also DragHandle.)
                IFigure contents = simulationCanvas.getContents();
                contents.setPreferredSize(contents.getPreferredSize());
            }

            @Override
            public void mouseUp(org.eclipse.swt.events.MouseEvent e) {
                dragOperation = 0;
                dragStart = null;
                simulationCanvas.getContents().setPreferredSize(null);  // restore original state
            }
        };

        swtMouseMoveListener = new MouseMoveListener() {
            @Override
            public void mouseMove(org.eclipse.swt.events.MouseEvent e) {
                Control control = (Control) e.widget;
                IInspectorPart inspector = simulationCanvas.findInspectorFor(control);

                if (dragOperation == 0) {
                    // update cursor
                    int dragOperation = inspector.getDragOperation(control, e.x, e.y);
                    Cursor cursor = getCursor(dragOperation);
                    control.setCursor(cursor);
                }
                else {
                    // perform move or resize
                    org.eclipse.swt.graphics.Point mouse = Display.getCurrent().getCursorLocation();
                    int dx = mouse.x - dragStart.x;
                    int dy = mouse.y - dragStart.y;
                    moveOrResizeInspectorFigure(dx, dy, inspector);
                }
            }
        };

        swtMouseTrackListener = new MouseTrackAdapter() {
            @Override
            public void mouseExit(org.eclipse.swt.events.MouseEvent e) {
                Control control = (Control) e.widget;
                control.setCursor(null); // because otherwise its child controls would inherit the (resize) cursor
            }
        };

        // clicking the background should deselect all
        simulationCanvas.getContents().addMouseListener(new MouseListener.Stub() {
            @Override
            public void mousePressed(MouseEvent me) {
                if (me.button == 1) {
                    // prepare for dragging
                    backgroundDragged = false;
                    org.eclipse.swt.graphics.Point mouse = Display.getCurrent().getCursorLocation();
                    dragStart = new Point(mouse.x, mouse.y);  // note: don't use (me.x,me.y) because they are figure-relative and the figure itself will move!
                    backgroundDragInitialScroll = simulationCanvas.getViewport().getViewLocation();
                    me.consume();  // this makes the figure capture the mouse during dragging!
                }
            }
            @Override
            public void mouseReleased(MouseEvent me) {
                if (me.button == 1 && !backgroundDragged)
                    simulationCanvas.deselectAll();
            }
        });

        // dragging the background should scroll the canvas
        simulationCanvas.getContents().addMouseMotionListener(new MouseMotionListener.Stub() {
            @Override
            public void mouseDragged(MouseEvent me) {
                if ((me.getState()&SWT.BUTTON_MASK) == SWT.BUTTON1) {
                    backgroundDragged = true;
                    org.eclipse.swt.graphics.Point mouse = Display.getCurrent().getCursorLocation();
                    int dx = mouse.x - dragStart.x;
                    int dy = mouse.y - dragStart.y;
                    simulationCanvas.getViewport().setViewLocation(backgroundDragInitialScroll.x - dx, backgroundDragInitialScroll.y - dy);
                }
            }
        });

    }

    protected Cursor getCursor(int dragOperation) {
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

            case SWT.LEFT|SWT.RIGHT|SWT.TOP|SWT.BOTTOM: cursor = CURSOR_ARROW /*HAND,SIZEALL*/; break;
            default: cursor = CURSOR_ARROW;
        }
        return cursor;
    }

    protected void moveOrResizeInspectorFigure(int dx, int dy, IInspectorPart inspector) {
        IFigure inspectorFigure = inspector.getFigure();
        Dimension min = inspectorFigure.getMinimumSize();
        Dimension max = inspectorFigure.getMaximumSize();
        Rectangle r = dragStartInspectorFigureBounds.getCopy();
        if (dragOperation == (SWT.LEFT|SWT.RIGHT|SWT.TOP|SWT.BOTTOM)) {
            r.x += dx;
            r.y += dy;
        }
        else {
            if ((dragOperation&SWT.LEFT) != 0) {
                r.width = clip(r.width-dx, min.width, max.width);
                r.x = dragStartInspectorFigureBounds.right() - r.width;
            }
            if ((dragOperation&SWT.TOP) != 0) {
                r.height = clip(r.height-dy, min.height, max.height);
                r.y = dragStartInspectorFigureBounds.bottom() - r.height;
            }
            if ((dragOperation&SWT.RIGHT) != 0) {
                r.width = clip(r.width+dx, min.width, max.width);
            }
            if ((dragOperation&SWT.BOTTOM) != 0) {
                r.height = clip(r.height+dy, min.height, max.height);
            }
        }
        if (r.x < 0) r.x = 0;
        if (r.y < 0) r.y = 0;
        inspectorFigure.getParent().setConstraint(inspectorFigure, r);
    }

    private static int clip(int x, int min, int max) {
        if (x < min) x = min;
        if (x > max) x = max;
        return x;
    }

    /**
     * Adds mouse listeners to the given figure. Figure must be part of the given inspector.
     * The inspector will be asked which part of the figure represent what operation
     * (see IInspectorPart.getDragOperation(IFigure, int x, int y)).
     */
    public void adopt(IFigure figure) {
        figure.addMouseListener(inspectorMouseListener);
        figure.addMouseMotionListener(inspectorMouseMotionListener);
    }

    public void forget(IFigure figure) {
        figure.removeMouseListener(inspectorMouseListener);
        figure.removeMouseMotionListener(inspectorMouseMotionListener);
    }

    public void adopt(Control control) {
        control.addMouseListener(swtMouseListener);
        control.addMouseMoveListener(swtMouseMoveListener);
        control.addMouseTrackListener(swtMouseTrackListener);
    }

    public void forget(Control control) {
        control.removeMouseListener(swtMouseListener);
        control.removeMouseMoveListener(swtMouseMoveListener);
        control.removeMouseTrackListener(swtMouseTrackListener);
    }

}
