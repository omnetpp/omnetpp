package org.omnetpp.simulation.ui;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.ListenerList;
import org.eclipse.jface.resource.JFaceResources;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseListener;
import org.eclipse.swt.events.PaintEvent;
import org.eclipse.swt.events.PaintListener;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.omnetpp.common.IConstants;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.engine.BigDecimal;

/**
 * 
 * @author Andras
 */
//TODO msg labels: place them in several rows (depending on available room)?
//TODO when there are many msgs at the same X coord: use vertical offset?
public class TimelineControl extends Canvas {
    private ITimelineContentProvider contentProvider;
    private ITimelineLabelProvider labelProvider;
    private ListenerList selectionListeners = new ListenerList();
    private boolean drawMinorTicks = true;
    private boolean adaptiveMinExponent = true;
    private boolean adaptiveMaxExponent = true;
    private int minExponent = -1;
    private int maxExponent = 0;
    private boolean enableTickLabels = true;
    private boolean enableMessageLabels = true;
    private Font tickLabelFont;  // null for default font
    private Font messageLabelFont;  // null for default font
    private int defaultNumMessageLabelRows = 2;

    private ISelection selection = new StructuredSelection();

    // drawing parameters (recomputed in each repaint call)
    private int zeroStartX;
    private int zeroEndX;
    private int arrowStartX;
    private int arrowEndX;
    private int minExponentX;
    private int maxExponentX;
    private int axisY;
    private int messageXCoords[] = new int[0];

    public TimelineControl(Composite parent, int style) {
        super(parent, style | SWT.DOUBLE_BUFFERED);

        Font smallFont = JFaceResources.getFont(IConstants.SMALL_FONT);
        tickLabelFont = smallFont;
        messageLabelFont = smallFont;

        addPaintListener(new PaintListener() {
            public void paintControl(PaintEvent e) {
                internalRepaint(e.gc);
            }
        });
        addMouseListener(new MouseListener() {
            @Override
            public void mouseDown(MouseEvent e) {
                selection = new StructuredSelection(findMessages(new Point(e.x, e.y), 3));
            }
            @Override
            public void mouseUp(MouseEvent e) {
                fireWidgetSelected(e);
            }
            @Override
            public void mouseDoubleClick(MouseEvent e) {
                if (e.button == 1)
                    fireWidgetDefaultSelected(e);
            }
        });

    }

    public void setContentProvider(ITimelineContentProvider contentProvider) {
        this.contentProvider = contentProvider;
        redraw();
    }

    public ITimelineContentProvider getContentProvider() {
        return contentProvider;
    }

    public void setLabelProvider(ITimelineLabelProvider labelProvider) {
        this.labelProvider = labelProvider;
        redraw();
    }

    public ITimelineLabelProvider getLabelProvider() {
        return labelProvider;
    }

    public int getMinExponent() {
        return minExponent;
    }

    public void setMinExponent(int minExponent) {
        this.minExponent = minExponent;
        redraw();
    }

    public int getMaxExponent() {
        return maxExponent;
    }

    public void setMaxExponent(int maxExponent) {
        this.maxExponent = maxExponent;
        redraw();
    }

    public void setAdaptiveExponents(boolean value) {
        setAdaptiveMinExponent(value);
        setAdaptiveMaxExponent(value);
    }

    public boolean isAdaptiveMinExponent() {
        return adaptiveMinExponent;
    }

    public void setAdaptiveMinExponent(boolean adaptiveMinExponent) {
        this.adaptiveMinExponent = adaptiveMinExponent;
        redraw();
    }

    public boolean isAdaptiveMaxExponent() {
        return adaptiveMaxExponent;
    }

    public void setAdaptiveMaxExponent(boolean adaptiveMaxExponent) {
        this.adaptiveMaxExponent = adaptiveMaxExponent;
        redraw();
    }

    public boolean getDrawMinorTicks() {
        return drawMinorTicks;
    }

    public void setDrawMinorTicks(boolean drawMinorTicks) {
        this.drawMinorTicks = drawMinorTicks;
        redraw();
    }

    public boolean getShowTickLabels() {
        return enableTickLabels;
    }

    public void setShowTickLabels(boolean enableTickLabels) {
        this.enableTickLabels = enableTickLabels;
        redraw();
    }

    public boolean getShowMessageLabels() {
        return enableMessageLabels;
    }

    public void setShowMessageLabels(boolean enableMessageLabels) {
        this.enableMessageLabels = enableMessageLabels;
        redraw();
    }

    public Font getTickLabelFont() {
        return tickLabelFont;
    }

    public void setTickLabelFont(Font tickLabelFont) {
        this.tickLabelFont = tickLabelFont;
        redraw();
    }

    public Font getMessageLabelFont() {
        return messageLabelFont;
    }

    public void setMessageLabelFont(Font messageLabelFont) {
        this.messageLabelFont = messageLabelFont;
        redraw();
    }

    public int getDefaultNumMessageLabelRows() {
        return defaultNumMessageLabelRows;
    }

    /**
     * This only affects the behavior of computeSize().
     */
    public void setDefaultNumMessageLabelRows(int defaultNumMessageLabelRows) {
        this.defaultNumMessageLabelRows = defaultNumMessageLabelRows;
    }

    public void addSelectionListener(SelectionListener listener) {
        selectionListeners.add(listener);
    }

    public void removeSelectionListener(SelectionListener listener) {
        selectionListeners.remove(listener);
    }

    /**
     * Returns the object (or objects) under the mouse when widget was last selected with
     * the mouse (clicked or double-clicked with any button), or the context menu was brought up.
     */
    public ISelection getSelection() {
        return selection;
    }

    /**
     * Returns the height recommended in collapsed state (= minimal sensible height):
     * no tick labels and no message labels, but enough room to display the messages themselves.
     */
    public int getCollapsedHeightHint() {
        return 20 /*16*/ + getBorderWidth();  // collapsed state: no tick labels, no message labels, etc
    }

    @Override
    public Point computeSize(int wHint, int hHint, boolean changed) {
        if (wHint == SWT.DEFAULT) {
            wHint = 500;
        }

        if (hHint == SWT.DEFAULT) {
            hHint = 0;
            if ((getStyle() & SWT.BORDER) != 0)
                hHint += 2*getBorderWidth();
            GC gc = (enableMessageLabels || enableTickLabels) ? new GC(this) : null;
            if (!enableMessageLabels)
                hHint += 3;
            else {
                gc.setFont(messageLabelFont!=null ? messageLabelFont : JFaceResources.getDefaultFont());
                hHint += defaultNumMessageLabelRows * gc.textExtent("Lj").y + 4;
            }
            if (!enableTickLabels)
                hHint += 4;
            else {
                gc.setFont(tickLabelFont!=null ? tickLabelFont : JFaceResources.getDefaultFont());
                hHint += gc.textExtent("10").y + 3;
            }
            if (gc != null) gc.dispose();
        }
        return new Point(wHint, hHint);
    }

    protected void internalRepaint(GC gc) {
        Assert.isNotNull(contentProvider, "content provider must be set");
        BigDecimal now = contentProvider.getBaseTime();
        Object[] messages = contentProvider.getMessages();

        gc.setAntialias(SWT.ON);
        Font defaultFont = gc.getFont();
        Rectangle r = getClientArea();
        gc.setFont(tickLabelFont!=null ? tickLabelFont : defaultFont);
        Point maxTickLabelSize = gc.textExtent("+100ms");
        int tickLabelHeight = maxTickLabelSize.y;

        boolean showTickLabels = enableTickLabels && (r.height >= tickLabelHeight+5); 
        axisY = showTickLabels ? r.height - tickLabelHeight - 1 : (r.height-1)/2;
        zeroStartX = maxTickLabelSize.x/2;
        zeroEndX = zeroStartX + 20;
        arrowStartX = zeroEndX + 20;
        arrowEndX = r.width - 8;
        minExponentX = arrowStartX + maxTickLabelSize.x;
        maxExponentX = arrowEndX - maxTickLabelSize.x;

        // adaptive modification of displayed exponent range
        if (isAdaptiveMinExponent())
            updateMinExponent(now, messages);
        if (isAdaptiveMaxExponent())
            updateMaxExponent(now, messages);

        // draw axis
        gc.setForeground(ColorFactory.BLACK);
        gc.setLineStyle(SWT.LINE_DOT);
        gc.drawLine(arrowStartX, axisY, minExponentX, axisY);
        gc.drawLine(maxExponentX, axisY, arrowEndX, axisY);
        gc.setLineStyle(SWT.LINE_SOLID);
        gc.drawLine(minExponentX, axisY, maxExponentX, axisY);
        gc.drawLine(arrowEndX-7, axisY-2, arrowEndX, axisY);
        gc.drawLine(arrowEndX-7, axisY+2, arrowEndX, axisY);

        // draw zero delta tray
        gc.setLineStyle(SWT.LINE_SOLID);
        gc.setForeground(ColorFactory.GREY50);
        int h=5;
        gc.drawLine(zeroStartX-3, axisY-h, zeroStartX-3, axisY+h);
        gc.drawLine(zeroEndX+3, axisY-h, zeroEndX+3, axisY+h);
        gc.drawLine(zeroStartX-3, axisY-h, zeroStartX-1, axisY-h);
        gc.drawLine(zeroStartX-3, axisY+h, zeroStartX-1, axisY+h);
        gc.drawLine(zeroEndX+3, axisY-h, zeroEndX+1, axisY-h);
        gc.drawLine(zeroEndX+3, axisY+h, zeroEndX+1, axisY+h);

        // draw ticks
        int tickSpacing = (maxExponentX-minExponentX) / (maxExponent-minExponent);
        boolean reduceTickLabels = tickSpacing < maxTickLabelSize.x+10;  // if there's not enough space, we only write out 1ns, 1us, 1ms, 1s,1000s etc.
        gc.setFont(tickLabelFont!=null ? tickLabelFont : defaultFont);
        for (int exp = minExponent-1; exp <= maxExponent; exp++) {
            if (exp >= minExponent) {
                // tick
                int x = getXForExponent(exp);
                gc.setForeground(ColorFactory.BLACK);
                gc.drawLine(x, axisY-4, x, axisY+4);

                // label
                if (showTickLabels && (!reduceTickLabels || exp % 3 == 0)) {
                    gc.setForeground(ColorFactory.GREY50);
                    String label = getTickLabel(1,exp);
                    int labelWidth = gc.textExtent(label).x;
                    gc.drawText(label, x - labelWidth/2, axisY+3, true);
                }
            }

            if (drawMinorTicks) {
                for (int i = 2; i <= 9; i++) {
                    int x = getXForTimeDelta(new BigDecimal(i,exp));
                    gc.drawLine(x, axisY-2, x, axisY+2);
                }
            }
        }
        if (showTickLabels) {
            gc.setForeground(ColorFactory.GREY50);
            String label = "+0s";
            int labelWidth = gc.textExtent(label).x;
            gc.drawText(label, (zeroEndX+zeroStartX)/2 - labelWidth/2, axisY+3, true);
        }

        messageXCoords = computeMessageCoordinates(now, messages);

        // paint messages
        gc.setFont(messageLabelFont!=null ? messageLabelFont : defaultFont);
        int messageLabelHeight = gc.textExtent("Lj").y;
        int numRows = (axisY-3) / messageLabelHeight;  // we can place message labels in multiple rows, depending on available space
        int[] labelRowLastX = new int[numRows]; // right edge of last label written in each row, with [0] being the bottom and [numRows-1] the top line
        for (int i = 0; i < numRows; i++)
            labelRowLastX[i] = Integer.MIN_VALUE;
        int bottomLabelRowY = axisY-3 - messageLabelHeight;

        for (int i = 0; i < messages.length; i++) {
            // draw symbol
            Object msg = messages[i];
            int x = messageXCoords[i];
            labelProvider.drawMessageSymbol(msg, gc, x, axisY);

            // draw label
            if (enableMessageLabels && numRows > 0) {
                String label = labelProvider.getMessageLabel(msg);
                int labelWidth = gc.textExtent(label).x;
                int labelX = x - labelWidth/2;
                int row;
                for (row = 0; row < numRows; row++)
                    if (labelRowLastX[row] < labelX)
                        break;
                if (row < numRows) {
                    gc.setForeground(ColorFactory.BLACK);
                    gc.drawText(label, labelX, bottomLabelRowY - row*messageLabelHeight, true);
                    labelRowLastX[row] = labelX + labelWidth;
                }
            }
        }
    }

    protected void updateMinExponent(BigDecimal now, Object[] messages) {
        if (messages.length == 0)
            return;  // nothing to do

        // find the smallest timestamp which is greater than "now" (ignore equal or
        // smaller ones, because they don't yield a finite log10(delta)),
        // and ensure minExponent is small enough to accommodate it.
        // Note that messages[] is ordered by timestamp.
        BigDecimal currentMinTime = null;
        for (Object msg : messages)
            if (contentProvider.getMessageTime(msg).greater(now))
                {currentMinTime = contentProvider.getMessageTime(msg); break;}
        if (currentMinTime != null) {
            double currentMinDelta = currentMinTime.subtract(now).doubleValue();
            int currentMinExponent = (int)Math.floor(Math.log10(currentMinDelta));
            if (minExponent > currentMinExponent)
                minExponent = currentMinExponent;
        }
    }

    protected void updateMaxExponent(BigDecimal now, Object[] messages) {
        if (messages.length == 0)
            return;  // nothing to do

        // find the largest timestamp, and ensure maxExponent is large enough to accommodate it
        BigDecimal currentMaxTime = contentProvider.getMessageTime(messages[messages.length-1]);
        if (currentMaxTime.greater(now)) {
            double currentMaxDelta = currentMaxTime.subtract(now).doubleValue();
            int currentMaxExponent = (int)Math.ceil(Math.log10(currentMaxDelta));
            if (maxExponent < currentMaxExponent)
                maxExponent = currentMaxExponent;
        }
    }

    protected int[] computeMessageCoordinates(BigDecimal now, Object[] messages) {
        // count how many messages are exactly for "now"
        int i = 0;
        for (i = 0; i < messages.length && contentProvider.getMessageTime(messages[i]).equals(now); i++);
        int numNow = i;

        // place those messages in the zero-delta (+0s) area
        int[] coords = new int[messages.length];
        double dx = Math.min(5.0, (zeroEndX-zeroStartX) / (double)numNow); // spacing: depending on available space, but maximum 5 pixels
        double x = (zeroStartX+zeroEndX)/2 - (numNow-1)*dx/2; 
        for (i = 0; i < numNow; i++, x += dx)
            coords[i] = (int)x;

        // place other messages on the axis
        double smallestExp = (numNow < messages.length) ? Math.log10(contentProvider.getMessageTime(messages[numNow]).subtract(now).doubleValue()) : Double.NaN /*unused*/;
        double largestExp = (messages.length > 0) ? Math.log10(contentProvider.getMessageTime(messages[messages.length-1]).subtract(now).doubleValue()) : Double.NaN /*unused*/;

        for (i = numNow; i < messages.length; i++) {
            BigDecimal delta = contentProvider.getMessageTime(messages[i]).subtract(now);
            double exp = Math.log10(delta.doubleValue());
            if (exp < minExponent) {
                // out of range (too small): map to dotted line segment between arrowStartX and minExponentX 
                coords[i] = arrowStartX + (int)((minExponentX - arrowStartX) * (exp - smallestExp) / (minExponent - smallestExp));
            }
            else if (exp > maxExponent) {
                // out of range (too large): map to dotted line segment between maxExponentX and arrowEndX 
                coords[i] = maxExponentX + (int)((arrowEndX - maxExponentX) * (exp - maxExponent) / (largestExp - maxExponent));
            }
            else {
                // normal: map to [minExponentX,maxExponentX]
                coords[i] = getXForExponent(exp);
            }
        }
        return coords;
    }

    public Object[] findMessages(Point p, int halo) {
        Object[] messages = contentProvider.getMessages();
        if (messages.length != messageXCoords.length)
            return new Object[0];  // we are out of sync!

        int n = messageXCoords.length;
        List<Object> result = new ArrayList<Object>();
        // here we could use binary search if needed because messageXCoords[] is ordered, but I think plain linear search this will do...
        int x1 = p.x - halo, x2 = p.x + halo;
        for (int i = 0; i < n; i++)
            if (messageXCoords[i] >= x1 && messageXCoords[i] <= x2)
                result.add(messages[i]);
        return result.toArray();
    }

    /**
     * Format the given number (time in seconds) in an easy-to-digest form, such as 
     * "1ms", "10ms", "100ms", "1s", "10s", "100s", "1000s", "1E4s", etc.
     */
    protected String getTickLabel(int mantissa, int exp) {
        Assert.isTrue(mantissa >= 1 && mantissa <= 9);
        String unit;
        if (exp < -15) {unit = "as"; exp += 18;}
        else if (exp < -12) {unit = "fs"; exp += 15;}
        else if (exp < -9) {unit = "ps"; exp += 12;}
        else if (exp < -6) {unit = "ns"; exp += 9;}
        else if (exp < -3) {unit = "us"; exp += 6;}
        else if (exp < 0) {unit = "ms"; exp += 3;}
        else unit = "s";
        String label = "+" + (Math.abs(exp)<=3 ? new BigDecimal(mantissa, exp).toString() : mantissa + "e" + exp) + unit;
        return label;
    }

    protected int getXForTimeDelta(BigDecimal delta) {
        return getXForExponent(Math.log10(delta.doubleValue()));
    }

    protected int getXForExponent(double exp) {
        exp = Math.max(minExponent, Math.min(maxExponent, exp));
        int x = minExponentX + (int)((maxExponentX-minExponentX) * (exp - minExponent) / (maxExponent - minExponent));  // map [min,max] to [margin,width-margin]
        return x;
    }

    // seems to be unused
    protected BigDecimal getTimeDeltaForX(int x) {
        Assert.isTrue(x >= minExponentX && x <= maxExponentX); 
        double exp = minExponent + (x - minExponentX) / (double)(maxExponentX-minExponentX) * (maxExponent - minExponent);
        return new BigDecimal(Math.pow(10, exp));
    }

    protected void fireWidgetSelected(MouseEvent me) {
        SelectionEvent e = makeSelectionEvent(me);
        for (Object l : selectionListeners.getListeners())
            ((SelectionListener)l).widgetSelected(e);
    }

    protected void fireWidgetDefaultSelected(MouseEvent me) {
        SelectionEvent e = makeSelectionEvent(me);
        for (Object l : selectionListeners.getListeners())
            ((SelectionListener)l).widgetDefaultSelected(e);
    }

    protected SelectionEvent makeSelectionEvent(MouseEvent me) {
        Event e = new Event();
        e.display = me.display;
        e.widget = me.widget;
        e.data = selection;
        e.x = me.x;
        e.y = me.y;
        SelectionEvent se = new SelectionEvent(e);
        se.display = Display.getCurrent();
        return se;
    }


}
