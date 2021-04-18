package org.omnetpp.common.ui;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseTrackListener;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;

/**
 * Send MouseEnter/MouseExit notifications. This works for composites as well,
 * that is, you won't receive a MouseExit event when the mouse enters one of the
 * children of the composite.
 *
 * @author andras
 */
public class MouseTracker implements Listener {
    static MouseTracker instance;

    private static class Item {
        Composite composite;
        boolean inside;
        MouseTrackListener listener;
    }

    private List<Item> listeners = new ArrayList<>();

    private MouseTracker() {
    }

    public static MouseTracker getInstance() {
        if (instance == null)
            instance = new MouseTracker();
        return instance;
    }

    private void install() {
        Display.getCurrent().addFilter(SWT.MouseEnter, this);
        Display.getCurrent().addFilter(SWT.MouseExit, this);
        Display.getCurrent().addFilter(SWT.Resize, this);
    }

    private void uninstall() {
        Display.getCurrent().removeFilter(SWT.MouseEnter, this);
        Display.getCurrent().removeFilter(SWT.MouseExit, this);
        Display.getCurrent().removeFilter(SWT.Resize, this);
    }

    public void addMouseTrackListener(Composite c, MouseTrackListener listener) {
        if (listeners.isEmpty())
            install();
        Item i = new Item();
        i.composite = c;
        i.inside = false;
        i.listener = listener;
        listeners.add(i);
    }

    public void removeMouseTrackListener(Composite c, MouseTrackListener listener) {
        listeners.removeIf((i) -> i.composite == c && i.listener == listener);
        if (listeners.isEmpty())
            uninstall();
    }

    public void handleEvent(Event e) {
        boolean hasDisposed = false;
        for (Item i : listeners) {
            Composite c = i.composite;
            if (c.isDisposed())
                hasDisposed = true;
            else {
                boolean containsMouse = containsMouse(c);
                if (i.inside != containsMouse) {
                    i.inside = containsMouse;
                    if (containsMouse)
                        i.listener.mouseEnter(new MouseEvent(e));
                    else
                        i.listener.mouseExit(new MouseEvent(e));
                }
            }
        }
        if (hasDisposed) {
            listeners.removeIf((i) -> i.composite.isDisposed());
            if (listeners.isEmpty())
                uninstall();
        }
    }

    public static boolean containsMouse(Composite c) {
        Point p = Display.getCurrent().getCursorLocation();
        return c.getBounds().contains(c.getParent().toControl(p));
    }
}
