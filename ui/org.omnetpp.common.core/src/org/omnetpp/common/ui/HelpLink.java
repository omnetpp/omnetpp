/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.ui;

import org.eclipse.jface.text.IInformationControl;
import org.eclipse.swt.events.FocusAdapter;
import org.eclipse.swt.events.FocusEvent;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Link;
import org.eclipse.swt.widgets.Monitor;
import org.eclipse.swt.widgets.Shell;

/**
 * A Link control (similar to HTML links in appearance) that displays
 * a focused persistent information control when the link is clicked.
 * The information control is closed it loses focus or the user hits Esc.
 *
 * Do not use it in dialogs or other composites that also have HoverSupport
 * installed. This control does not tell HoverSupport to refrain from
 * showing information controls while this HelpLink has its own open.
 *
 * @author AnRdras
 */
public class HelpLink {
    private Link link;
    private String hoverText;
    private Point hoverSize;

    public HelpLink(Composite parent, int style) {
        link = new Link(parent, style);
        link.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                openInformationControl();
            }
        });
    }

    public Link getLink() {
        return link;
    }

    public void setText(String text) {
        link.setText(text);
    }

    public String getText() {
        return link.getText();
    }

    public void setHoverText(String text) {
        this.hoverText = text;
    }

    public String getHoverText() {
        return hoverText;
    }

    public Point getHoverSize() {
        return hoverSize;
    }

    public void setHoverSize(Point hoverSize) {
        this.hoverSize = hoverSize;
    }

    public void openInformationControl() {
        final IInformationControl c = createInformationControl(Display.getCurrent().getActiveShell());
        c.setSizeConstraints(600, 400);
        c.setInformation(HoverSupport.addHTMLStyleSheet(getHoverText()));
        Point size = hoverSize;
        if (size == null) {
            size = c.computeSizeHint(); //new Point(600, 400);
            size.y = (int)(2.0 * size.y);   // computeSizeHint() consistently underestimates height
            if (size.x > 600) size.x = 600;
            if (size.y > 400) size.y = 400;
        }
        Point mouse = Display.getCurrent().getCursorLocation();
        Rectangle linkBounds = Display.getCurrent().map(link.getParent(), null, link.getBounds());
        Point loc = linkBounds.contains(mouse) ? new Point(mouse.x-20, mouse.y+15) : new Point(linkBounds.x + linkBounds.width - size.x, linkBounds.y + linkBounds.height + 5);
        loc = adjustLocation(loc, size);
        c.setLocation(loc);
        c.setSize(size.x, size.y);
        c.setVisible(true);
        c.setFocus();
        c.addFocusListener(new FocusAdapter() {
            @Override
            public void focusLost(FocusEvent e) {
                c.dispose();
            }
        });
    }

    protected IInformationControl createInformationControl(Shell shell) {
        return new StyledTextInformationControl(shell, true);
    }

    protected Point adjustLocation(Point loc, Point size) {
        // ensure location is on the screen
        Monitor monitor = findMonitorByPosition(loc);
        if (monitor == null)
            monitor = Display.getCurrent().getPrimaryMonitor();

        Rectangle screen = monitor.getBounds();
        Point p = new Point(loc.x, loc.y);
        p.x = Math.min(p.x, screen.x + screen.width - size.x - 5);
        if (p.y + 20 + size.y > screen.y + screen.height)
            p.y = loc.y - size.y - 30; // if no room below mouse, show it above
        return p;
    }

    protected Monitor findMonitorByPosition(Point position) {
        Monitor[] monitors = Display.getCurrent().getMonitors();
        for (Monitor monitor : monitors)
            if (monitor.getBounds().contains(position))
                return monitor;
        return null;
    }

}
