/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.canvas;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.ListenerList;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.jface.util.IPropertyChangeListener;
import org.eclipse.jface.util.PropertyChangeEvent;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.ScrollBar;

/**
 * A canvas that supports a scrollable area which can be very large,
 * as the area's width/height are stored as 64-bit numbers. This class
 * overcomes the scrollbar's 32-bit limit.
 *
 * @author andras
 */
public abstract class LargeScrollableCanvas extends Canvas {

    public static final String PROP_VIEW_X = "viewX";
    public static final String PROP_VIEW_Y = "viewY";

    private long virtualWidth, virtualHeight; // 64-bit size of the "virtual canvas"
    private long viewX, viewY; // 64-bit coordinates of top-left corner of the viewport
    private int hShift, vShift; // used for scrollbar mapping
    private Rectangle viewportRect; // the scrolled area within the canvas

    private ListenerList listeners = new ListenerList();

    public LargeScrollableCanvas(Composite parent, int style) {
        super(parent, style | SWT.H_SCROLL | SWT.V_SCROLL);
        init();
    }

    private void init() {
        getHorizontalBar().setIncrement(10);
        getVerticalBar().setIncrement(10);

        // set an initial viewport rectangle to prevent null pointer exceptions;
        // clients are expected to set a proper viewport rectangle inside Resize events.
        setViewportRectangle(new Rectangle(0,0,1,1));

        //
        // Note: there is no need to reconfigure the scrollbars on Resize events,
        // because scrollbar state only depends on virtualWidth/Height and viewportRect,
        // and both of them have explicit setter methods.
        //
        // Rather, users of this class are expected to hook on resize events and
        // adjust viewportRect from there.
        //

        getHorizontalBar().addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                horizontalScrollBarChanged(e);
            }
        });
        getVerticalBar().addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                verticalScrollBarChanged(e);
            }
        });
    }

    public void setVirtualSize(long width, long height) {
        width = Math.max(width, 1);
        height = Math.max(height, 1);

        this.virtualWidth = width;
        this.virtualHeight = height;
        configureScrollBars();
    }

    public long getVirtualHeight() {
        return virtualHeight;
    }

    public void setVirtualHeight(long height) {
        setVirtualSize(this.virtualWidth, height);
    }

    public long getVirtualWidth() {
        return virtualWidth;
    }

    public void setVirtualWidth(long width) {
        setVirtualSize(width, this.virtualHeight);
    }

    /**
     * The virtual coordinate of the left of the viewport (clientArea minus insets).
     */
    public long getViewportLeft() {
        return viewX;
    }

    public long getViewportRight() {
        return viewX + getViewportWidth();
    }

    /**
     * The virtual coordinate of the top of the viewport (clientArea minus insets).
     */
    public long getViewportTop() {
        return viewY;
    }

    public long getViewportBottom() {
        return viewY + getViewportHeight();
    }

    /**
     * Scroll horizontally with the given amount of pixels.
     */
    public void scrollHorizontal(long dx) {
        scrollHorizontalTo(getViewportLeft() + dx);
    }

    /**
     * Scroll vertically with the given amount of pixels.
     */
    public void scrollVertical(long dy) {
        scrollVerticalTo(getViewportTop() + dy);
    }

    /**
     * Sets viewportLeft.
     */
    public void scrollHorizontalTo(long x) {
        long oldValue = this.viewX;
        this.viewX = clipX(x);
        adjustHorizontalScrollBar();
        firePropertyChangeEvent(PROP_VIEW_X, oldValue, this.viewX);
        redraw();
    }

    /**
     * Sets viewportTop.
     */
    public void scrollVerticalTo(long y) {
        long oldValue = this.viewY;
        this.viewY = clipY(y);
        adjustVerticalScrollBar();
        firePropertyChangeEvent(PROP_VIEW_Y, oldValue, this.viewY);
        redraw();
    }

    /**
     * Makes the horizontal range visible with as little scrolling as possible.
     */
    public void scrollHorizontalToRange(long x1, long x2) {
        Assert.isTrue(x2 >= x1);
        long dx = x2 - x1;

        if (dx > getViewportWidth())
            scrollHorizontalTo((x2 + x1 - getViewportWidth()) / 2);
        else if (x1 < viewX)
            scrollHorizontalTo(x1);
        else if (x2 > viewX + getViewportWidth())
            scrollHorizontalTo(x2 - getViewportWidth());
    }

    /**
     * Makes the vertical range visible with as little scrolling as possible.
     */
    public void scrollVerticalToRange(long y1, long y2) {
        Assert.isTrue(y2 >= y1);
        long dy = y2 - y1;

        if (dy > getViewportHeight())
            scrollVerticalTo((y2 + y1 - getViewportHeight()) / 2);
        else if (y1 < viewY)
            scrollVerticalTo(y1);
        else if (y2 > viewY + getViewportHeight())
            scrollVerticalTo(y2 - getViewportHeight());
    }

    protected long clipX(long x) {
        return clip(x, virtualWidth - getViewportWidth());
    }

    protected long clipY(long y) {
        return clip(y, virtualHeight - getViewportHeight());
    }

    /**
     * Utility function to clip x to (0, upperBound). If upperBound<0, 0 is returned.
     */
    private static long clip(long x, long upperBound) {
        if (x > upperBound)
            x = upperBound;  // if negative, we'll fix it below
        if (x < 0)
            x = 0;
        return x;
    }

    /**
     * Returns the viewport (clientArea minus insets) in canvas coordinates.
     */
    public Rectangle getViewportRectangle() {
        return new Rectangle(viewportRect.x, viewportRect.y, viewportRect.width, viewportRect.height);
    }

    public void setViewportRectangle(Rectangle r) {
        viewportRect = new Rectangle(r.x, r.y, r.width, r.height);
        configureScrollBars();
    }

    public int virtualToCanvasX(long x) {
        return (int)(x - getViewportLeft() + viewportRect.x);
    }

    public int virtualToCanvasY(long y) {
        return (int)(y - getViewportTop() + viewportRect.y);
    }

    public Rectangle virtualToCanvasRect(LargeRect r) {
        return new Rectangle(virtualToCanvasX(r.x), virtualToCanvasY(r.y), (int)r.width, (int)r.height);
    }

    public long canvasToVirtualX(int x) {
        return x + getViewportLeft() - viewportRect.x;
    }

    public long canvasToVirtualY(int y) {
        return y + getViewportTop() - viewportRect.y;
    }

    public LargeRect canvasToVirtualRect(Rectangle r) {
        return new LargeRect(r.x - viewportRect.x + getViewportLeft(), r.y - viewportRect.y + getViewportTop(), r.width, r.height);
    }

    /**
     * Returns the width of the viewport (clientArea minus insets).
     */
    public int getViewportWidth() {
        return viewportRect.width;
    }

    /**
     * Returns the height of the viewport (clientArea minus insets).
     */
    public int getViewportHeight() {
        return viewportRect.height;
    }

    protected void horizontalScrollBarChanged(SelectionEvent e) {
        viewX = clipX(((long)getHorizontalBar().getSelection()) << hShift);
        redraw();
    }

    protected void verticalScrollBarChanged(SelectionEvent e) {
        viewY = clipY(((long)getVerticalBar().getSelection()) << vShift);
        redraw();
    }

    /**
     * Should be called when viewport or virtual size changes.
     */
    protected void configureScrollBars() {
        hShift = configureHorizontalScrollBar(getHorizontalBar(), virtualWidth, viewX, getViewportWidth());
        vShift = configureVerticalScrollBar(getVerticalBar(), virtualHeight, viewY, getViewportHeight());
    }

    protected int configureHorizontalScrollBar(ScrollBar scrollBar, long virtualSize, long virtualPosition, int widgetSize) {
        viewX = clipX(virtualPosition);
        return configureScrollBar(scrollBar, virtualSize, virtualPosition, widgetSize);
    }

    protected int configureVerticalScrollBar(ScrollBar scrollBar, long virtualSize, long virtualPosition, int widgetSize) {
        viewY = clipY(virtualPosition);
        return configureScrollBar(scrollBar, virtualSize, virtualPosition, widgetSize);
    }

    protected int configureScrollBar(ScrollBar scrollBar, long virtualSize, long virtualPos, int widgetSize) {
        if (widgetSize >= virtualSize) {
            // hide scrollbar when not needed
            scrollBar.setVisible(false);
            return 0;
        }
        else {
            // show scrollbar
            scrollBar.setVisible(true);

            // count how many bits we need to shift to fit into 32 bits
            int shift = 0;
            while (virtualSize>>shift > Integer.MAX_VALUE)
                shift++;

            int newMax = (int)(virtualSize>>shift);
            int newThumb = (widgetSize >> shift);
            int newSel = (int)(virtualPos >> shift);
            int newPageIncr = ((widgetSize>>shift > 0) ? widgetSize>>shift : 1);

            scrollBar.setMinimum(0);
            scrollBar.setMaximum(newMax);
            scrollBar.setThumb(newThumb);
            scrollBar.setSelection(newSel);
            scrollBar.setPageIncrement(newPageIncr);
            return shift;
        }
    }

    protected void adjustHorizontalScrollBar() {
        getHorizontalBar().setSelection((int)(viewX >> hShift));
    }

    protected void adjustVerticalScrollBar() {
        getVerticalBar().setSelection((int)(viewY >> vShift));
    }

    public void addPropertyChangeListener(IPropertyChangeListener listener) {
        listeners.add(listener);
    }

    public void removePropertyChangeListener(IPropertyChangeListener listener) {
        listeners.remove(listener);
    }

    protected void firePropertyChangeEvent(String property, Object oldValue, Object newValue) {
        PropertyChangeEvent event = new PropertyChangeEvent(this, property, oldValue, newValue);
        for (Object listener : listeners.getListeners()) {
            ((IPropertyChangeListener)listener).propertyChange(event);
        }
    }
}
