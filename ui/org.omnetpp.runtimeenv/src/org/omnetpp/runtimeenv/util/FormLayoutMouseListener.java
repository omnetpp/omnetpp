package org.omnetpp.runtimeenv.util;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseListener;
import org.eclipse.swt.events.MouseMoveListener;
import org.eclipse.swt.events.MouseTrackListener;
import org.eclipse.swt.graphics.Cursor;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.layout.FormAttachment;
import org.eclipse.swt.layout.FormData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;


/**
 *
 * @author Levy
 */
//XXX from inner class of ReplayAnimationEditor
//XXX unreliable: often it does not allow resizing a control, then in the next second suddenly it does (???)
public class FormLayoutMouseListener implements MouseListener, MouseMoveListener, MouseTrackListener {
    protected Composite parent;
    protected boolean allowHorizontalResize;
    protected boolean allowVerticalResize;
    protected Control dragControl;
    protected int borderSize = 4;  // as inset of dragControl
    protected Point dragStart;
    protected Point dragStartControlSize;
    protected Point dragStartControlLocation;
    protected boolean dragLeft;
    protected boolean dragRight;
    protected boolean dragTop;
    protected boolean dragBottom;
    protected boolean dragMove;

    public FormLayoutMouseListener(Composite parent, boolean allowResize) {
        this(parent, allowResize, allowResize);
    }

    public FormLayoutMouseListener(Composite parent, boolean allowHorizontalResize, boolean allowVerticalResize) {
        this.parent = parent;
        this.allowHorizontalResize = allowHorizontalResize;
        this.allowVerticalResize = allowVerticalResize;
    }

    public void mouseDoubleClick(MouseEvent e) {
        extractDragControlInformation(e);

        if ((e.stateMask & SWT.CONTROL) != 0) {
//FIXME find another way to determine preferred size!!!!
//            if (dragControl.getChildren()[0] instanceof FigureCanvas) {
//                FigureCanvas figureCanvas = (FigureCanvas)dragControl.getChildren()[0];
//                Point size = figureCanvas.getPreferredSize();
//                FormData formData = (FormData)dragControl.getLayoutData();
//                formData.right = new FormAttachment(0, dragControl.getLocation().x + size.x + borderSize * 2);
//                formData.bottom = new FormAttachment(0, dragControl.getLocation().y + size.y + borderSize * 2);
//                parent.layout();
//            }
        }
        else {
            Point delta = new Point(0, 0);
            DockingLimits dockingLimits = new DockingLimits(10000, 0, 0, 10000);
            calculateDragMode(e);
            boolean oldDragLeft = dragLeft;
            boolean oldDragRight = dragRight;
            boolean oldDragTop = dragTop;
            boolean oldDragBottom = dragBottom;

            dragRight = dragTop = dragBottom = false;
            dragLeft = oldDragLeft & allowHorizontalResize;
            moveOrResizeDraggedControl(delta, dockingLimits);
            parent.layout();

            calculateDragMode(e);
            dragLeft = dragTop = dragBottom = false;
            dragRight = oldDragRight & allowHorizontalResize;
            moveOrResizeDraggedControl(delta, dockingLimits);
            parent.layout();

            calculateDragMode(e);
            dragLeft = dragRight = dragBottom = false;
            dragTop = oldDragTop & allowVerticalResize;
            moveOrResizeDraggedControl(delta, dockingLimits);
            parent.layout();

            calculateDragMode(e);
            dragLeft = dragRight = dragTop = false;
            dragBottom = oldDragBottom & allowVerticalResize;
            moveOrResizeDraggedControl(delta, dockingLimits);
            parent.layout();
        }

        dragControl = null;
    }

    public void mouseDown(MouseEvent e) {
        extractDragControlInformation(e);
        dragControl.moveAbove(null);
        dragStart = dragControl.toDisplay(e.x, e.y);

        calculateDragMode(e);
    }

    public void mouseUp(MouseEvent e) {
        dragControl = null;
    }

    public void mouseMove(MouseEvent e) {
        if (dragControl != null) {
            Control control = (Control)e.widget;
            Point p = control.toDisplay(e.x, e.y);
            Point delta = new Point(p.x - dragStart.x, p.y - dragStart.y);

            moveOrResizeDraggedControl(delta, new DockingLimits((e.stateMask & SWT.CONTROL) == 0 ? 10 : 0));

            parent.layout();
        }
        else
            updateCursor(e);
    }

    public void mouseEnter(MouseEvent e) {
        updateCursor(e);
    }

    public void mouseExit(MouseEvent e) {
        Control control = (Control)e.widget;
        control.setCursor(null);
    }

    public void mouseHover(MouseEvent e) {
    }

    protected class DockingLimits {
        public int maximumSmallerValueDecrease;
        public int maximumSmallerValueIncrease;
        public int maximumBiggerValueDecrease;
        public int maximumBiggerValueIncrease;

        public DockingLimits(int maximumValueChange) {
            this(maximumValueChange, maximumValueChange, maximumValueChange, maximumValueChange);
        }

        public DockingLimits(int maximumSmallerValueDecrease, int maximumSmallerValueIncrease, int maximumBiggerValueDecrease, int maximumBiggerValueIncrease) {
            this.maximumSmallerValueDecrease = maximumSmallerValueDecrease;
            this.maximumSmallerValueIncrease = maximumSmallerValueIncrease;
            this.maximumBiggerValueDecrease = maximumBiggerValueDecrease;
            this.maximumBiggerValueIncrease = maximumBiggerValueIncrease;
        }
    }

    protected void extractDragControlInformation(MouseEvent e) {
        dragControl = (Control) e.widget;
        dragStartControlSize = dragControl.getSize();
        dragStartControlLocation = dragControl.getLocation();
    }

    protected void moveOrResizeDraggedControl(Point delta, DockingLimits dockingLimits)
    {
        int left = dragStartControlLocation.x + delta.x;
        int right = dragStartControlLocation.x + dragStartControlSize.x + delta.x;
        int top = dragStartControlLocation.y + delta.y;
        int bottom = dragStartControlLocation.y + dragStartControlSize.y + delta.y;
        FormAttachment[] horizontalFormAttachments = getDockingAttachments(true, left, right, dockingLimits);
        FormAttachment[] verticalFormAttachments = getDockingAttachments(false, top, bottom, dockingLimits);
        FormData formData = (FormData)dragControl.getLayoutData();
        if (dragLeft)
            formData.left = horizontalFormAttachments != null ? horizontalFormAttachments[0] : new FormAttachment(0, left);
            if (dragRight)
                formData.right = horizontalFormAttachments != null ? horizontalFormAttachments[1] : new FormAttachment(0, right);
                if (dragTop)
                    formData.top = verticalFormAttachments != null ? verticalFormAttachments[0] : new FormAttachment(0, top);
                    if (dragBottom)
                        formData.bottom = verticalFormAttachments != null ? verticalFormAttachments[1] : new FormAttachment(0, bottom);
    }

    protected void updateCursor(MouseEvent e) {
        Control control = (Control)e.widget;

        calculateDragMode(e);

        int cursorType;
        if (dragMove)
            cursorType = SWT.CURSOR_SIZEALL;
        else if (dragLeft && !dragRight && !dragTop && !dragBottom)
            cursorType = SWT.CURSOR_SIZEW;
        else if (!dragLeft && dragRight && !dragTop && !dragBottom)
            cursorType = SWT.CURSOR_SIZEE;
        else if (!dragLeft && !dragRight && dragTop && !dragBottom)
            cursorType = SWT.CURSOR_SIZEN;
        else if (!dragLeft && !dragRight && !dragTop && dragBottom)
            cursorType = SWT.CURSOR_SIZES;
        else if (dragLeft && !dragRight && dragTop && !dragBottom)
            cursorType = SWT.CURSOR_SIZENW;
        else if (!dragLeft && dragRight && dragTop && !dragBottom)
            cursorType = SWT.CURSOR_SIZENE;
        else if (!dragLeft && dragRight && !dragTop && dragBottom)
            cursorType = SWT.CURSOR_SIZESE;
        else if (dragLeft && !dragRight && !dragTop && dragBottom)
            cursorType = SWT.CURSOR_SIZESW;
        else
            cursorType = SWT.CURSOR_ARROW;

        control.setCursor(new Cursor(null, cursorType));
    }

    protected void calculateDragMode(MouseEvent e) {
        Control control = ((Control)e.widget);
        Point size = control.getSize();
        int dragMinHandleSize = 20;
        int dragHandleWidth = Math.min(dragMinHandleSize, size.x / 6);
        int dragHandleHeight = Math.min(dragMinHandleSize, size.y / 6);
        int dragX = getDragValue(e.x, size.x, dragHandleWidth);
        int dragY = getDragValue(e.y, size.y, dragHandleHeight);

        dragLeft = dragX == 0 || dragX == 3 || dragY == 3;
        dragRight = dragX == 2 || dragX == 3 || dragY == 3;

        if (!allowHorizontalResize)
            dragLeft = dragRight = dragLeft || dragRight;

        dragTop = dragY == 0 || dragY == 3 || dragX == 3;
        dragBottom = dragY == 2 || dragY == 3 || dragX == 3;

        if (!allowVerticalResize)
            dragTop = dragBottom = dragTop || dragBottom;

        dragMove = dragLeft && dragRight && dragTop & dragBottom;
    }

    protected int getDragValue(int p, int controlSize, int dragHandleSize) {
        if (0 <= p && p <= dragHandleSize)
            return 0;
        else if (controlSize / 2 - dragHandleSize <= p && p <= controlSize / 2 + dragHandleSize)
            return 1;
        else if (controlSize - dragHandleSize <= p && p <= controlSize)
            return 2;
        else
            return 3;
    }

    protected FormAttachment[] getDockingAttachments(boolean horizontal, int smallerValue, int biggerValue, DockingLimits dockingLimits) {
        FormAttachment[] formAttachments = new FormAttachment[] {null, null};
        Rectangle parentRectangle = parent.getClientArea();
        int minValue = 0;
        int maxValue = horizontal ? parentRectangle.width : parentRectangle.height;
        int bestDockingDistance = Integer.MAX_VALUE;
        int size = biggerValue - smallerValue;
        boolean dragSmaller = horizontal ? dragLeft : dragTop;
        boolean dragBigger = horizontal ? dragRight : dragBottom;

        // docking to parent
        bestDockingDistance = setDockingFormAttachments(formAttachments, bestDockingDistance, dockingLimits, null, dragSmaller, smallerValue, minValue, true, 0, size);
        bestDockingDistance = setDockingFormAttachments(formAttachments, bestDockingDistance, dockingLimits, null, dragBigger, biggerValue, maxValue, false, 100, size);

        // docking to siblings
        for (Control childControl : parent.getChildren()) {
            if (childControl == dragControl)
                continue;

            Point childLocation = childControl.getLocation();
            Point childSize = childControl.getSize();
            int childSmallerValue = horizontal ? childLocation.x : childLocation.y;
            int childBiggerValue = horizontal ? childLocation.x + childSize.x : childLocation.y + childSize.y;

            bestDockingDistance = setDockingFormAttachments(formAttachments, bestDockingDistance, dockingLimits, childControl, dragSmaller, smallerValue, childSmallerValue, true, 0, size);
            bestDockingDistance = setDockingFormAttachments(formAttachments, bestDockingDistance, dockingLimits, childControl, dragSmaller, smallerValue, childBiggerValue, true, 0, size);
            bestDockingDistance = setDockingFormAttachments(formAttachments, bestDockingDistance, dockingLimits, childControl, dragBigger, biggerValue, childSmallerValue, false, 0, size);
            bestDockingDistance = setDockingFormAttachments(formAttachments, bestDockingDistance, dockingLimits, childControl, dragBigger, biggerValue, childBiggerValue, false, 0, size);
        }

        if (bestDockingDistance != Integer.MAX_VALUE)
            return formAttachments;
        else
            return null;
    }

    protected int setDockingFormAttachments(FormAttachment[] formAttachments,
            int bestDistance,
            DockingLimits dockingLimits,
            Control dockingControl,
            boolean dragFlag,
            int currentValue,
            int dockingValue,
            boolean smallerValue,
            int parentPercent,
            int size) {
        int currentDockingDistance = Math.abs(currentValue - dockingValue);
        int maximumValueIncrease = smallerValue ? dockingLimits.maximumSmallerValueIncrease : dockingLimits.maximumBiggerValueIncrease;
        int maximumValueDecrease = smallerValue ? dockingLimits.maximumSmallerValueDecrease : dockingLimits.maximumBiggerValueDecrease;

        if (dragFlag &&
                currentDockingDistance < bestDistance &&
                dockingValue - maximumValueIncrease <= currentValue &&
                currentValue <= dockingValue + maximumValueDecrease)
        {
            /* TODO: if you ever want to attach to siblings then use this and break dependency cycles
				if (dockingControl != null) {
					formAttachments[0] = new FormAttachment(dockingControl, smallerValue ? 0 : -size);
					formAttachments[1] = new FormAttachment(dockingControl, smallerValue ? size : 0);
				}
             */
            formAttachments[0] = new FormAttachment(parentPercent, (parentPercent == 0 ? dockingValue : 0) + (smallerValue ? 0 : -size));
            formAttachments[1] = new FormAttachment(parentPercent, (parentPercent == 0 ? dockingValue : 0) + (smallerValue ? size : 0));

            return currentDockingDistance;
        }
        else
            return bestDistance;
    }
}
