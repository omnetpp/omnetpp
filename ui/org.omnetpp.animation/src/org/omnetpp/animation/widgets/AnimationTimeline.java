package org.omnetpp.animation.widgets;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.SWTGraphics;
import org.eclipse.swt.events.PaintEvent;
import org.eclipse.swt.events.PaintListener;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Composite;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.util.GraphicsUtils;
import org.omnetpp.eventlog.engine.IEvent;
import org.omnetpp.eventlog.engine.IEventLog;

public class AnimationTimeline
    extends Canvas
    implements IAnimationListener
{
    public static int HEIGHT = 32;

    private AnimationController animationController;

    private String oldSimulationTime = "";

    public AnimationTimeline(Composite parent, int style) {
        super(parent, style);
        addPaintListener(new PaintListener() {
            public void paintControl(final PaintEvent e) {
                paint(e.gc);
            }
        });
    }

    public void setAnimationController(AnimationController animationController) {
        this.animationController = animationController;
        animationController.addAnimationListener(this);
    }

    @Override
    public Point computeSize(int wHint, int hHint, boolean changed) {
        return new Point(wHint, HEIGHT);
    }

    @Override
    public void dispose() {
        super.dispose();
        if (animationController != null)
            animationController.removeAnimationListener(this);
    }

    public void runningStateChanged(boolean isRunning) {
    }

    public void animationPositionChanged(AnimationPosition animationPosition) {
        if (!isDisposed())
            redraw();
    }

    private void paint(GC gc) {
        Graphics graphics = new SWTGraphics(gc);
        drawEventNumber(graphics);
        drawSimulationTime(graphics);
        drawAnimationTime(graphics);
        drawEventLine(graphics);
    }

    private void drawEventNumber(Graphics graphics) {
        graphics.drawText("Event #" + animationController.getEventNumber(), 0, 0);
    }

    private void drawSimulationTime(Graphics graphics) {
        Rectangle clientArea = getClientArea();
        int width = Math.max(150, clientArea.width / 3);
        String header = "T=";
        graphics.drawText(header, width, 0);
        width += GraphicsUtils.getTextExtent(graphics, header).x;
        String newSimulationTime = animationController.getSimulationTime().toString();
        int i;
        for (i = 0; i < oldSimulationTime.length() && i < newSimulationTime.length() && oldSimulationTime.charAt(i) == newSimulationTime.charAt(i); i++);
        graphics.setForegroundColor(ColorFactory.BLACK);
        String samePart = newSimulationTime.substring(0, i);
        graphics.drawText(samePart, width, 0);
        width += GraphicsUtils.getTextExtent(graphics, samePart).x;
        graphics.setForegroundColor(ColorFactory.DARK_GREY);
        graphics.drawText(newSimulationTime.substring(i), width, 0);
        oldSimulationTime = newSimulationTime;
        graphics.setForegroundColor(ColorFactory.BLACK);
    }

    private void drawAnimationTime(Graphics graphics) {
        Rectangle clientArea = getClientArea();
        int width = Math.max(300, clientArea.width / 3 * 2);
        graphics.drawText("A=" + animationController.getAnimationTime(), width, 0);
    }

    private void drawEventLine(Graphics graphics) {
        Rectangle clientArea = getClientArea();
        int y = clientArea.y + clientArea.height / 2;
        graphics.drawLine(clientArea.x, y, clientArea.x + clientArea.width, y);
        IEventLog eventLog = animationController.getEventLogInput().getEventLog();
        IEvent event = eventLog.getEventForEventNumber(animationController.getEventNumber());
        IEvent firstEvent = event;
        IEvent lastEvent = event;
        for (int i = 0; i < 10; i++) {
            if (firstEvent.getPreviousEvent() != null)
                firstEvent = firstEvent.getPreviousEvent();
            if (lastEvent.getNextEvent() != null)
                lastEvent = lastEvent.getNextEvent();
        }
    }
}
