package org.omnetpp.simulation.inspectors;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.jface.action.ControlContribution;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseMoveListener;
import org.eclipse.swt.graphics.Cursor;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.SimulationUIConstants;

/**
 * TODO
 * @author Andras
 */
public class DragHandle extends ControlContribution {
    private static final Cursor DRAG_CURSOR = new Cursor(Display.getCurrent(), SWT.CURSOR_HAND);
    private static final Image IMG_DRAGHANDLE = SimulationPlugin.getCachedImage(SimulationUIConstants.IMG_TOOL_DRAGHANDLE);

    private IInspectorPart inspector;
    private Label dragHandle;

    private boolean dragInProgress = false;
    private int moveOffsetX, moveOffsetY;


    public DragHandle(IInspectorPart inspector) {
        super("");
        this.inspector = inspector;
    }

    @Override
    protected Control createControl(Composite parent) {
        dragHandle = new Label(parent, SWT.NONE);
        dragHandle.setImage(IMG_DRAGHANDLE);
        dragHandle.setToolTipText("Drag handle");

        // configure drag handle (allows inspector to be moved around with the mouse)
        dragHandle.addMouseListener(new MouseAdapter() {
            public void mouseDown(MouseEvent e) {
                dragInProgress = true;
                Point p = Display.getCurrent().getCursorLocation();
                IInspectorContainer inspectorContainer = inspector.getContainer();
                org.eclipse.draw2d.geometry.Point pp = inspectorContainer.translateCanvasToAbsoluteFigureCoordinates(p.x, p.y); // well, p is screen coord not canvas, but here it only affects moveOffset
                Rectangle figureBounds = inspector.getFigure().getBounds();
                moveOffsetX = figureBounds.x - pp.x;
                moveOffsetY = figureBounds.y - pp.y;
                dragHandle.setCursor(DRAG_CURSOR);
                inspector.raiseToTop();

                // By default draw2d updates canvas bounds continually while dragging, which
                // results in a weird effect: when dragging up an inspector from near the bottom
                // of the canvas and the canvas shrinks as a result, the mouse overtakes the
                // drag handle and other interesting things happen. The workaround is to disable
                // updating the canvas bounds while dragging; this can be achieved by setting
                // preferredSize.
                IFigure contents = inspectorContainer.getControl().getContents();
                contents.setPreferredSize(contents.getPreferredSize());
            }
            public void mouseUp(MouseEvent e) {
                dragInProgress = false;
                dragHandle.setCursor(null);

                // restore no preferredSize
                IInspectorContainer inspectorContainer = inspector.getContainer();
                IFigure contents = inspectorContainer.getControl().getContents();
                contents.setPreferredSize(null);
            }
        });

        dragHandle.addMouseMoveListener(new MouseMoveListener() {
            public void mouseMove(MouseEvent e) {
                if (dragInProgress && (e.stateMask&SWT.BUTTON_MASK) != SWT.BUTTON1)
                    dragInProgress = false;
                if (dragInProgress) {
                    Point p = Display.getCurrent().getCursorLocation();
                    IInspectorContainer inspectorContainer = inspector.getContainer();
                    org.eclipse.draw2d.geometry.Point pp = inspectorContainer.translateCanvasToAbsoluteFigureCoordinates(p.x, p.y); // well, p is screen coord not canvas, but here it only affects moveOffset
                    IInspectorFigure figure = inspector.getFigure();
                    Rectangle r = figure.getBounds().getCopy();
                    r.setLocation(pp.x + moveOffsetX, pp.y + moveOffsetY);

                    // don't allow inspectors to stick out on the left or top, because
                    // it can be difficult or impossible to bring them back
                    if (r.x < 0) r.x = 0;
                    if (r.y < 0) r.y = 0;
                    figure.getParent().setConstraint(figure, r);
                }
            }
        });

        return dragHandle;
    }

}

