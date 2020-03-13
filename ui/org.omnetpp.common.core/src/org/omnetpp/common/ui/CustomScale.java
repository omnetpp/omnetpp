/*--------------------------------------------------------------*
   Copyright (C) 2006-2015 OpenSim Ltd.

   This file is distributed WITHOUT ANY WARRANTY. See the file
   'License' for details on this and other legal matters.
 *--------------------------------------------------------------*/

package org.omnetpp.common.ui;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.ListenerList;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseListener;
import org.eclipse.swt.events.MouseMoveListener;
import org.eclipse.swt.events.PaintEvent;
import org.eclipse.swt.events.PaintListener;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.omnetpp.common.color.ColorFactory;

/**
 * Custom Scale control. Needed because the normal Scale's height cannot
 * be small enough to fit into a toolbar (Windows).
 *
 * Intended to be a drop-in replacement of Scale.
 *
 * Can be rendered in two styles: 3D or flat. 3D is the default; flat can be
 * selected with the SWT.FLAT style bit.
 *
 * @author Andras
 */
public class CustomScale extends Canvas {
    private static final int DEFAULT_WIDTH = 160;
    private static final int DEFAULT_HEIGHT = 16;

    // drawing constants
    private final int marginWidth = 5;
    private final int marginHeight = 1;
    private final int handleBorderRadius = 5;
    private final int handleWidth = 10;

    // configuration and state
    private int minimum = 0;
    private int maximum = 99;
    private int pageIncrement = 10;
    private int selection = 0;
    private ListenerList<SelectionListener> listeners = new ListenerList<>();

    protected class MyMouseListener implements MouseMoveListener, MouseListener {
        public void mouseMove(MouseEvent e) { setSelection(e); }
        public void mouseDown(MouseEvent e) { setSelection(e); }
        public void mouseUp(MouseEvent e) {}
        public void mouseDoubleClick(MouseEvent e) {}

        private void setSelection(MouseEvent e) {
            if (!isEnabled())
                return;
            if (e.button == 1 || (e.stateMask & SWT.BUTTON1) != 0) {
                selection = computeSelectionFromHandleX(e.x);
                redraw();
                notifySelectionListeners();
            }
        }

    };

    /**
     * Constructs a new CScale control.
     */
    public CustomScale(Composite parent, int style) {
        super(parent, style | SWT.DOUBLE_BUFFERED);

        addPaintListener(new PaintListener() {
            public void paintControl(PaintEvent e) {
                internalRepaint(e.gc);
            }
        });

        MyMouseListener mouseListener = new MyMouseListener();
        addMouseListener(mouseListener);
        addMouseMoveListener(mouseListener);
    }

    public int getMinimum() {
        return minimum;
    }

    public void setMinimum(int minimum) {
        this.minimum = minimum;
        ensureSelectionInRange();
        redraw();
        notifySelectionListeners();
    }

    public int getMaximum() {
        return maximum;
    }

    public void setMaximum(int maximum) {
        this.maximum = maximum;
        ensureSelectionInRange();
        redraw();
        notifySelectionListeners();
    }

    public int getPageIncrement() {
        return pageIncrement;
    }

    public void setPageIncrement(int pageIncrement) {
        this.pageIncrement = pageIncrement;
    }

    public int getSelection() {
        return selection;
    }

    public void setSelection(int value) {
        this.selection = value;
        ensureSelectionInRange();
        redraw();
        notifySelectionListeners();
    }

    private void ensureSelectionInRange() {
        if (selection < minimum)
            selection = minimum;
        if (selection > maximum)
            selection = maximum;
    }

    /**
     * Adds a listener that will be notifier when the widget's value changes.
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

    private void notifySelectionListeners() {
        Event e = new Event();
        e.display = Display.getCurrent();
        e.widget = this;
        e.data = this;
        SelectionEvent event = new SelectionEvent(e);
        event.display = Display.getCurrent();
        for (SelectionListener listener : listeners)
            listener.widgetSelected(event);
    }

    @Override
    public Point computeSize(int wHint, int hHint, boolean changed) {
        if (wHint == SWT.DEFAULT) wHint = DEFAULT_WIDTH;
        if (hHint == SWT.DEFAULT) hHint = DEFAULT_HEIGHT;
        return super.computeSize(wHint, hHint, changed);
    }

    protected void internalRepaint(GC gc) {
        Rectangle r = getClientArea();

        if ((getStyle() & SWT.FLAT) != 0) {
            // draw in flat appearance
            gc.setAntialias(SWT.ON);

            // draw a middle mark
            gc.setForeground(isEnabled() ? ColorFactory.GREY40 : ColorFactory.GREY80);
            gc.setLineWidth(1);
            gc.drawLine(r.width / 2,  0, r.width / 2, r.height-1);

            // draw middle slot
            gc.setBackground(isEnabled() ? ColorFactory.GREY80 : ColorFactory.GREY90);
            gc.setForeground(isEnabled() ? ColorFactory.GREY70 : ColorFactory.GREY85);
            gc.fillRectangle(marginWidth, r.height/2 - 3, r.width-2*marginWidth, 6);
            gc.drawRectangle(marginWidth, r.height/2 - 3, r.width-2*marginWidth, 6);
            gc.setLineWidth(1);

            // draw the handle
            int handleTop = marginHeight;
            int handleHeight = r.height - 2 * marginHeight;
            int handleLeft = computeHandleXFromSelection(selection) - handleWidth/2;

            if (isEnabled()) {
                gc.setForeground(ColorFactory.GREY10);
                gc.setBackground(ColorFactory.GREY60);
            }
            else {
                // in disabled state, paint in near white
                gc.setForeground(ColorFactory.GREY90);
                gc.setBackground(ColorFactory.GREY100);
            }
            gc.fillRoundRectangle(handleLeft, handleTop, handleWidth-1, handleHeight-1, handleBorderRadius, handleBorderRadius);
            gc.drawRoundRectangle(handleLeft, handleTop, handleWidth-1, handleHeight-1, handleBorderRadius, handleBorderRadius);
        }
        else {
            // draw it in 3D appearance

            // draw a middle mark
            drawSunkenBorder(gc, new Rectangle(r.width/2, 0, 2, r.height-1), 1, isEnabled() ? ColorFactory.GREY80 :ColorFactory.GREY100);

            // draw middle slot
            drawSunkenBorder(gc, new Rectangle(marginWidth, r.height/2 - marginHeight - 1, r.width-2*marginWidth, 6), 1, isEnabled() ? ColorFactory.GREY80 : ColorFactory.GREY100);

            // draw the handle
            int handleTop = marginHeight;
            int handleHeight = r.height - 2 * marginHeight;
            int handleLeft = computeHandleXFromSelection(selection) - handleWidth/2;
            drawRaisedBorder(gc, new Rectangle(handleLeft, handleTop, handleWidth-1, handleHeight-1), 1, isEnabled() ? ColorFactory.GREY60 : ColorFactory.GREY90);
        }
    }

    private int computeSelectionFromHandleX(int handleX) {
        int handleLeft = handleX - handleWidth/2;
        Rectangle r = getClientArea();
        double selection01 = (handleLeft - marginWidth  + handleWidth/2) / (double)(r.width - 2*marginWidth);
        int selection = minimum + (int)(selection01 * (double)(maximum - minimum));
        selection = Math.max(minimum, Math.min(maximum, selection));
        return selection;
    }

    private int computeHandleXFromSelection(int selection) {
        double selection01 = (selection-minimum) / (double)(maximum - minimum);
        Rectangle r = getClientArea();
        int handleX = marginWidth + (int)(selection01 * (r.width - 2*marginWidth));
        return handleX;
    }

    private static void drawRaisedBorder(GC gc, Rectangle r, int borderWidth, Color fillColor) {
        draw3DBorder(gc, r, borderWidth, fillColor, new Color[] {
                gc.getDevice().getSystemColor(SWT.COLOR_WIDGET_HIGHLIGHT_SHADOW),
                gc.getDevice().getSystemColor(SWT.COLOR_WIDGET_LIGHT_SHADOW),
                gc.getDevice().getSystemColor(SWT.COLOR_WIDGET_DARK_SHADOW),
                gc.getDevice().getSystemColor(SWT.COLOR_WIDGET_NORMAL_SHADOW)
        });
    }

    private static void drawSunkenBorder(GC gc, Rectangle r, int borderWidth, Color fillColor) {
        draw3DBorder(gc, r, borderWidth, fillColor, new Color[] {
                gc.getDevice().getSystemColor(SWT.COLOR_WIDGET_DARK_SHADOW),
                gc.getDevice().getSystemColor(SWT.COLOR_WIDGET_NORMAL_SHADOW),
                gc.getDevice().getSystemColor(SWT.COLOR_WIDGET_HIGHLIGHT_SHADOW),
                gc.getDevice().getSystemColor(SWT.COLOR_WIDGET_LIGHT_SHADOW),
        });
    }

    private static void draw3DBorder(GC gc, Rectangle r, int borderWidth, Color fillColor, Color[] colors) {
        Assert.isTrue(borderWidth == 0  || borderWidth==1 || borderWidth==2);

        if (fillColor != null) {
            gc.setBackground(fillColor);
            gc.fillRectangle(r);
        }

        if (borderWidth > 0) {
            gc.setForeground(colors[0]);
            gc.drawLine(r.x, r.y, r.x + r.width - 1, r.y);
            gc.drawLine(r.x, r.y, r.x, r.y + r.height - 1);
        }

        if (borderWidth > 1) {
            gc.setForeground(colors[1]);
            gc.drawLine(r.x + 1, r.y + 1, r.x + r.width - 2, r.y + 1);
            gc.drawLine(r.x + 1, r.y + 1, r.x + 1, r.y + r.height - 2);
        }

        if (borderWidth > 0) {
            gc.setForeground(colors[2]);
            gc.drawLine(r.x + r.width - 1, r.y, r.x + r.width - 1, r.y + r.height - 1);
            gc.drawLine(r.x, r.y + r.height - 1, r.x + r.width - 1, r.y + r.height - 1);
        }

        if (borderWidth > 1) {
            gc.setForeground(colors[3]);
            gc.drawLine(r.x + r.width - 2, r.y + 1, r.x + r.width - 2, r.y + r.height - 2);
            gc.drawLine(r.x + 1, r.y + r.height - 2, r.x + r.width - 2, r.y + r.height - 2);
        }
    }

}
