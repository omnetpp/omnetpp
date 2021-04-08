/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.ui;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.ListenerList;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.DisposeEvent;
import org.eclipse.swt.events.DisposeListener;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseListener;
import org.eclipse.swt.events.MouseMoveListener;
import org.eclipse.swt.events.PaintEvent;
import org.eclipse.swt.events.PaintListener;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.ToolTip;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.util.StringUtils;

/**
 * Widget that presents a lever that can be pulled in two directions, to select
 * a double between -1.0 and 1.0. Once released, the lever jumps back to the
 * middle position. The lever can also be leverHooked (locked down at its current
 * position) by dragging it downwards with the mouse.
 *
 * The widget can also display a secondary mark which is of static nature back
 * but can be dragged by the user. By default the mark is hidden, and can only
 * be displayed programmatically (setMarkValue()).
 *
 * @author Andras
 */
public class Lever extends Canvas {
    public static final int LEVER = 1; // SelectionEvent.detail value: lever value changed
    public static final int MARK = 2;  // SelectionEvent.detail value: mark value changed

    private double leverValue = 0;  // -1.0 to 1.0
    private boolean leverDragged = false;
    private boolean leverHooked = false;
    private int mouseDownY;
    private ListenerList listeners = new ListenerList();
    private List<Double> tickPos = new ArrayList<Double>();
    private List<String> tickText = new ArrayList<String>();
    private boolean markShown = false;
    private double markValue = 0.5;
    private boolean markDragged = false;
    private boolean allowNegativeMark = true;
    private ILeverLabelProvider labelProvider;
    private ToolTip tooltip;

    protected class MyMouseListener implements MouseListener, MouseMoveListener {
        public void mouseDown(MouseEvent e) {
            if (!isEnabled())
                return;
            if (markShown && markContainsPoint(e.x, e.y)) {
                markDragged = true;
                redraw();  // no notifyListeners(), only when mark reaches its final position
            }
            else {
                leverDragged = true;
                leverHooked = false;
                leverValue = leverValueFromX(e.x);
                mouseDownY = e.y;
                redraw();
                notifySelectionListeners(LEVER);
            }
        }

        public void mouseUp(MouseEvent e) {
            if (!isEnabled())
                return;
            if (markDragged) {
                tooltip.setVisible(false);
                markDragged = false;
                redraw();
                notifySelectionListeners(MARK);
            }
            if (leverDragged) {
                if (!leverHooked)
                    leverValue = 0;
                tooltip.setVisible(false);
                leverDragged = false;
                redraw();
                notifySelectionListeners(LEVER);
            }
        }

        public void mouseMove(MouseEvent e) {
            if (!isEnabled())
                return;
            if (markDragged) {
                markValue = leverValueFromX(e.x);
                if (!allowNegativeMark && markValue < 0)
                    markValue = 0;
                redraw();
                notifySelectionListeners(MARK);
                return;
            }
            if (leverDragged) {
                leverHooked = e.y > mouseDownY + 8;
                if (!leverHooked)
                    leverValue = leverValueFromX(e.x);
                redraw();
                notifySelectionListeners(LEVER);
            }
        }

        public void mouseDoubleClick(MouseEvent e) {
        }
    };

    /**
     * Constructs a new Lever control.
     */
    public Lever(Composite parent, int style) {
        super(parent, style | SWT.DOUBLE_BUFFERED);
        tooltip = new ToolTip(getShell(), SWT.NONE);
        tooltip.setAutoHide(false);
        tooltip.setVisible(false);

        addPaintListener(new PaintListener() {
            public void paintControl(PaintEvent e) {
                internalRedraw(e.gc);
            }
        });
        addDisposeListener(new DisposeListener() {
            public void widgetDisposed(DisposeEvent e) {
                tooltip.dispose();
            }
        });

        MyMouseListener mouseListener = new MyMouseListener();
        addMouseListener(mouseListener);
        addMouseMoveListener(mouseListener);
    }

    /**
     * Sets the selection (the lever's position) to the given value.
     * @param value  must be between -1.0 and 1.0, inclusive
     */
    public void setSelection(double value) {
        if (leverValue != value) {
            if (value < -1 || value > 1)
                throw new IllegalArgumentException("value must be between -1.0 and 1.0, inclusive");
            leverValue = value;
            redraw();
            notifySelectionListeners(LEVER);
        }
    }

    /**
     * Returns the selection (the lever's position) as number between -1.0 and 1.0, inclusive.
     */
    public double getSelection() {
        return leverValue;
    }

    /**
     * Sets whether the lever is leverHooked to its current position.
     */
    public void setHooked(boolean hooked) {
        if (this.leverHooked != hooked) {
            this.leverHooked = hooked;
            redraw();
            notifySelectionListeners(LEVER);
        }
    }

    /**
     * Returns true if the lever is leverHooked to its current position.
     */
    public boolean isHooked() {
        return leverHooked;
    }

    public void setMarkShown(boolean markShown) {
        if (this.markShown != markShown) {
            this.markShown = markShown;
            redraw();
            notifySelectionListeners(MARK);
        }
    }

    public boolean isMarkShown() {
        return markShown;
    }

    /**
     * Sets the secondary mark's position to the given value.
     * @param markValue  must be between -1.0 (or 0, see setAllowNegativeMark()) and 1.0, inclusive
     */
    public void setMarkValue(double markValue) {
        if (this.markValue != markValue) {
            if (markValue < -1 || markValue > 1)
                throw new IllegalArgumentException("value must be between -1.0 and 1.0, inclusive");
            if (!allowNegativeMark && markValue < 0)
                throw new IllegalArgumentException("negative mark values are not allowed");
            this.markValue = markValue;
            redraw();
            notifySelectionListeners(MARK);
        }
    }

    /**
     * Returns the value of the secondary mark
     */
    public double getMarkValue() {
        return markValue;
    }

    public void setAllowNegativeMark(boolean allowNegativeMark) {
        this.allowNegativeMark = allowNegativeMark;
        if (!allowNegativeMark && markValue < 0)
            setMarkValue(0);
    }

    public boolean getAllowNegativeMark() {
        return allowNegativeMark;
    }

    public void setLabelProvider(ILeverLabelProvider labelProvider) {
        this.labelProvider = labelProvider;
    }

    public ILeverLabelProvider getLabelProvider() {
        return labelProvider;
    }

    /**
     * Adds a listener that will be notifier when the widget's value
     * (lever's position) or the leverHooked state changes.
     */
    public void addSelectionListener(SelectionListener listener) {
        listeners.add(listener);
    }

    /**
     * Removes the given selection listener.
     */
    public void removeSelectionListener(SelectionListener listener) {
        listeners.remove(listener);
    }

    public void addTick(double pos, String text) {
        tickPos.add(pos);
        tickText.add(text);
        redraw();
    }

    public void clearTicks() {
        tickPos.clear();
        tickText.clear();
        redraw();
    }

    private void notifySelectionListeners(int detail) {
        Event e = new Event();
        e.display = Display.getCurrent();
        e.widget = this;
        e.data = this;
        e.detail = detail;
        SelectionEvent event = new SelectionEvent(e);
        event.display = Display.getCurrent();
        for (Object o : listeners.getListeners())
            ((SelectionListener)o).widgetSelected(event);
    }

    @Override
    public Point computeSize(int wHint, int hHint, boolean changed) {
        if (wHint == SWT.DEFAULT) wHint = 300;
        if (hHint == SWT.DEFAULT) hHint = 24;
        return super.computeSize(wHint, hHint, changed);
    }

    protected void internalRedraw(GC gc) {
        gc.setAntialias(SWT.ON);

        Rectangle r = getBounds();

        int middleX = r.width / 2;

        // draw the slot with the teeth
        int toothWidth = 3;
        int toothGap = 3;
        int numTeeth = (r.width - toothWidth) / (toothWidth + toothGap);
        int numPoints = 4 + 4*numTeeth;
        int[] p = new int[2*numPoints];
        int slotTop = 7;
        int slotBottom = 15;
        int slotMid = 11;
        int slotLeft = 0;
        int slotRight = slotLeft + toothWidth + numTeeth*(toothGap + toothWidth);
        p[0] = slotRight; p[1] = slotBottom;
        p[2] = slotRight; p[3] = slotTop;
        p[4] = slotLeft; p[5] = slotTop;
        p[6] = slotLeft; p[7] = slotBottom;
        int x = slotLeft;
        int pp = 8;
        for (int i=0; i<numTeeth; i++) {
            x += toothGap;
            p[pp++] = x; p[pp++] = slotBottom;
            p[pp++] = x; p[pp++] = slotMid;
            x += toothWidth;
            p[pp++] = x; p[pp++] = slotMid;
            p[pp++] = x; p[pp++] = slotBottom;
        }

        gc.setForeground(ColorFactory.GREY90);
        gc.setBackground(ColorFactory.GREY100);
        gc.fillPolygon(p);
        gc.drawPolygon(p);

        // draw the middle mark
        gc.setForeground(isEnabled() ? ColorFactory.GREY40 : ColorFactory.GREY80);
        gc.drawLine(middleX,  0, middleX, r.height-1);
        gc.setLineWidth(1);

        // draw the ticks
        for (int i = 0; i < tickPos.size(); i++) {
            int tickX = xFromLeverValue(tickPos.get(i));
            gc.drawLine(tickX,  0, tickX, 5);  //TODO better y coords!
            gc.drawText(tickText.get(i), tickX+1 - 7, 10, SWT.DRAW_TRANSPARENT); //TODO better placement!
        }

        // draw the secondary mark (normally below the handle)
        if (markShown && !markDragged)
            drawMark(gc);

        // draw the handle
        int hookDeltaY = 3;
        int handleTop = (leverHooked ? hookDeltaY : 0);
        int handleHeight = r.height - 2 - hookDeltaY;
        int handleX = xFromLeverValue(leverValue);
        int handleWidth = 10;
        int handleBorderRadius = 5;

        if (isEnabled()) {
            gc.setForeground(ColorFactory.GREY10);
            gc.setBackground(leverHooked ? ColorFactory.GREY50: ColorFactory.GREY70);
        }
        else {
            // in disabled state, paint in near white
            gc.setForeground(ColorFactory.GREY90);
            gc.setBackground(ColorFactory.GREY100);
        }
        gc.fillRoundRectangle(handleX-handleWidth/2, handleTop, handleWidth, handleHeight, handleBorderRadius, handleBorderRadius);
        gc.drawRoundRectangle(handleX-handleWidth/2, handleTop, handleWidth, handleHeight, handleBorderRadius, handleBorderRadius);

        // display tooltip
        if (leverDragged && labelProvider != null) {
            String text = labelProvider.getTextForLever(leverValue);
            if (!StringUtils.isEmpty(text)) {
                setToolTipText(text);
                tooltip.setVisible(false);
                tooltip.setMessage(text);
                Point tooltipPos = Display.getCurrent().map(this, null, handleX, -20);
                tooltip.setLocation(tooltipPos);
                tooltip.setVisible(true);
            }
        }

        // draw the secondary mark while it's being dragged (draw above the handle)
        if (markShown && markDragged)
            drawMark(gc);
    }

    private void drawMark(GC gc) {
        // draw mark
        int markX = xFromLeverValue(markValue);
        int[] q = new int[10];
        int markTop = 10;
        int markHalfWidth = 3;
        int markHalfHeight = 5;
        int markHeight = 10;
        q[0] = markX; q[1] = markTop;
        q[2] = markX - markHalfWidth; q[3] = markTop + markHalfHeight;
        q[4] = markX - markHalfWidth; q[5] = markTop + markHeight;
        q[6] = markX + markHalfWidth; q[7] = markTop + markHeight;
        q[8] = markX + markHalfWidth; q[9] = markTop + markHalfHeight;
        if (isEnabled()) {
            gc.setForeground(ColorFactory.GREY30);
            gc.setBackground(ColorFactory.GREY50);
        }
        else {
            // in disabled state, paint in near white
            gc.setForeground(ColorFactory.GREY90);
            gc.setBackground(ColorFactory.GREY100);
        }
        gc.fillPolygon(q);
        gc.drawPolygon(q);

        // display tooltip
        if (markDragged && labelProvider != null) {
            String text = labelProvider.getTextForMark(markValue);
            if (!StringUtils.isEmpty(text)) {
                setToolTipText(text);
                tooltip.setVisible(false);
                tooltip.setMessage(text);
                Point tooltipPos = Display.getCurrent().map(this, null, markX, -20);
                tooltip.setLocation(tooltipPos);
                tooltip.setVisible(true);
            }
        }

    }

    private boolean markContainsPoint(int x, int y) {
        int markX = xFromLeverValue(markValue);
        int markTop = 10;
        int markHalfWidth = 3;
        int markHeight = 10;
        Rectangle r = new Rectangle(markX-markHalfWidth, markTop, 2*markHalfWidth, markHeight);
        return r.contains(x, y);
    }

    private double leverValueFromX(int handleX) {
        int middleX = getSize().x / 2;
        int handleMinX = 10;  // note: keep consistent with the internalRepaint() method
        double value = (handleX - middleX) / (double)(middleX - handleMinX);
        if (value < -1)
            value = -1;
        else if (value > 1)
            value = 1;
        return value;
    }

    private int xFromLeverValue(double value) {
        Assert.isTrue(value >= -1 && value <= 1);
        int middleX = getSize().x / 2;
        int handleMinX = 10;  // note: keep consistent with the internalRepaint() method
        return middleX + (int)(value * (middleX - handleMinX));
    }

}
