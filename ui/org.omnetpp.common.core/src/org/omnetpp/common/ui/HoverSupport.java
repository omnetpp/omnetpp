/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.ui;

import java.util.HashMap;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.internal.text.InformationControlReplacer;
import org.eclipse.jface.internal.text.html.BrowserInformationControl;
import org.eclipse.jface.text.AbstractHoverInformationControlManager;
import org.eclipse.jface.text.AbstractReusableInformationControlCreator;
import org.eclipse.jface.text.DefaultInformationControl;
import org.eclipse.jface.text.IInformationControl;
import org.eclipse.jface.text.IInformationControlCreator;
import org.eclipse.jface.text.IInformationControlExtension3;
import org.eclipse.jface.util.Geometry;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ControlEvent;
import org.eclipse.swt.events.ControlListener;
import org.eclipse.swt.events.DisposeEvent;
import org.eclipse.swt.events.DisposeListener;
import org.eclipse.swt.events.FocusEvent;
import org.eclipse.swt.events.FocusListener;
import org.eclipse.swt.events.KeyEvent;
import org.eclipse.swt.events.KeyListener;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseListener;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Monitor;
import org.eclipse.swt.widgets.Shell;
import org.omnetpp.common.util.ReflectionUtils;


@SuppressWarnings("restriction")
public class HoverSupport {
    private static final String AFFORDANCE = "Press 'F2' for focus";
    // Note: this is a copy of the stylesheet used by JDT (we want to avoid depending on the JDT plugins)
    private static final String HTML_PROLOG =
        "<html><head><style CHARSET=\"ISO-8859-1\" TYPE=\"text/css\">\n" +
        "/* Font definitions*/\n" +
        "html   { font-family: 'Tahoma',sans-serif; font-size: 8pt; font-style: normal; font-weight: normal; }\n" +
        "body, h1, h2, h3, h4, h5, h6, p, table, td, caption, th, ul, ol, dl, li, dd, dt { font-size:1em; }\n" +
        "pre    { font-family: monospace; }\n" +
        "/* Margins */\n" +
        "body   { overflow: auto; margin-top: 0px; margin-bottom: 0.5em; margin-left: 0.3em; margin-right: 0px; }\n" +
        "h1     { margin-top: 0.3em; margin-bottom: 0.04em; }\n" +
        "h2     { margin-top: 2em; margin-bottom: 0.25em; }\n" +
        "h3     { margin-top: 1.7em; margin-bottom: 0.25em; }\n" +
        "h4     { margin-top: 2em; margin-bottom: 0.3em; }\n" +
        "h5     { margin-top: 0px; margin-bottom: 0px; }\n" +
        "p      { margin-top: 1em; margin-bottom: 1em; }\n" +
        "pre    { margin-left: 0.6em; margin-top: 0.3em }\n" +
        "ul     { margin-top: 0px; margin-bottom: 1em; }\n" +
        "li     { margin-top: 0px; margin-bottom: 0px; }\n" +
        "li p   { margin-top: 0px; margin-bottom: 0px; }\n" +
        "ol     { margin-top: 0px; margin-bottom: 1em; }\n" +
        "dl     { margin-top: 0px; margin-bottom: 1em; }\n" +
        "dt     { margin-top: 0px; margin-bottom: 0px; font-weight: bold; }\n" +
        "dd     { margin-top: 0px; margin-bottom: 0px; }\n" +
        "/* Styles and colors */\n" +
        "a:link    { color: #0000FF; }\n" +
        "a:hover   { color: #000080; }\n" +
        "a:visited { text-decoration: underline; }\n" +
        "h4        { font-style: italic; }\n" +
        "strong    { font-weight: bold; }\n" +
        "em        { font-style: italic; }\n" +
        "var       { font-style: italic; }\n" +
        "th        { font-weight: bold; }\n" +
        "</style></head>\n" +
        "<body text=\"#000000\" bgcolor=\"#ffffe1\">\n";
    private static final String HTML_EPILOG =
        "</body></html>\n";

    private class LocalHoverInformationControlManager extends AbstractHoverInformationControlManager {
        /**
         * A global (workbench-wide) event filter for making a hover persistent (F2 key).
         * This listener is hooked on Display when the first widget gets adapted,
         * and unhooked when the last widget gets forgotten (or disposed).
         *
         * Global event filters are not without dangers; we chose to use it because this way
         * we can catch keyboard events wherever the focus is.
         */
        protected Listener eventFilter = new Listener() {
            public void handleEvent(Event e) {
                if (e.type == SWT.KeyDown && e.keyCode == SWT.F2 && !getInternalAccessor().isReplaceInProgress()) {
                    getInternalAccessor().replaceInformationControl(true);
                    e.type = SWT.NONE;
                }
            }
        };

        public LocalHoverInformationControlManager(IInformationControlCreator creator) {
            super(creator);
        }

        @Override
        protected void computeInformation() {
            Point location = getHoverEventLocation();
            IInformationControl informationControl = getInformationControl();
            Point mouseLocation = Display.getDefault().getCursorLocation();
            SizeConstraint preferredSize = new SizeConstraint();
            String hoverText = hoverTextProviders.get(getSubjectControl()).getHoverTextFor(getSubjectControl(), location.x, location.y, preferredSize);
            org.eclipse.swt.graphics.Rectangle subjectArea = new org.eclipse.swt.graphics.Rectangle(location.x, location.y, 1, 1);
            setInformation(hoverText, subjectArea);
            if (hoverText != null) {
                informationControl.setSizeConstraints(hoverSizeConstraints.x, hoverSizeConstraints.y);
                setInformation(hoverText, subjectArea);
                Point size = informationControl.computeSizeHint(); //issue: BrowserInformationControl is always at least 80 pixels high -- this is hardcoded :(
                size.x = calculateSize(hoverSizeConstraints.x, size.x, preferredSize.minimumWidth, preferredSize.preferredWidth);
                size.y = calculateSize(hoverSizeConstraints.y, size.y, preferredSize.minimumHeight, preferredSize.preferredHeight);
                informationControl.setSize(size.x, size.y);
                informationControl.setLocation(calculateHoverPosition(mouseLocation, size));
            }
        }

        @Override
        protected void showInformationControl(org.eclipse.swt.graphics.Rectangle subjectArea) {
            hookGlobalEventFilter();
            super.showInformationControl(subjectArea);
        }

        @Override
        protected void hideInformationControl() {
            try {
                super.hideInformationControl();
            }
            finally {
                unhookGlobalEventFilter();
            }
        }

        private void hookGlobalEventFilter() {
            Display.getDefault().addFilter(SWT.KeyDown, eventFilter);
        }

        private void unhookGlobalEventFilter() {
            Display.getDefault().removeFilter(SWT.KeyDown, eventFilter);
        }

        private int calculateSize(int hoverSizeConstraint, int sizeHint, int minimumSize, int preferredSize) {
            int size = sizeHint;

            if (minimumSize != SWT.DEFAULT)
                size = Math.max(minimumSize, size);

            if (preferredSize != SWT.DEFAULT)
                size = Math.min(preferredSize, hoverSizeConstraint);

            return size;
        }

        private Point calculateHoverPosition(Point mouse, Point size) {
            Monitor monitor = findMonitorByPosition(mouse);
            if (monitor == null)
                monitor = Display.getCurrent().getPrimaryMonitor();

            Rectangle screen = monitor.getBounds();
            Point p = new Point(mouse.x + 5, mouse.y + 20);
            p.x = Math.min(p.x, screen.x + screen.width - size.x - 5);
            if (p.y + 20 + size.y > screen.y + screen.height)
                p.y = mouse.y - size.y - 20; // if no room below mouse, show it above
            return p;
        }

        private Monitor findMonitorByPosition(Point position) {
            Monitor[] monitors = Display.getCurrent().getMonitors();
            for (Monitor monitor : monitors)
                if (monitor.getBounds().contains(position))
                    return monitor;
            return null;
        }
    }

    private static class BrowserInformationControlCreator extends AbstractReusableInformationControlCreator {
        @Override
        public IInformationControl doCreateInformationControl(Shell shell) {
            if (BrowserInformationControl.isAvailable(shell))
                return new BrowserInformationControl(shell, null, AFFORDANCE) {
                    @Override
                    public IInformationControlCreator getInformationPresenterControlCreator() {
                        return HoverSupport.getInformationPresenterControlCreator();
                    }
            };
            else
                return new DefaultInformationControl(shell, false);
        }
    }


    private static class StickyBrowserInformationControlCreator extends AbstractReusableInformationControlCreator {
        @Override
        public IInformationControl doCreateInformationControl(Shell shell) {
            if (BrowserInformationControl.isAvailable(shell))
                return new BrowserInformationControl(shell, null, true);
            else
                return new DefaultInformationControl(shell, true);
        }
    }

    // KLUDGE: this class is completely superfluous and should be deleted if the already existing eclipse code becomes reusable... :(
    private static class LocalInformationControlReplacer extends InformationControlReplacer {
        // KLUDGE: the contents of thie inner class is copied over from StickyHoverManager
        // so much for code reuse... :(
        class Closer implements IInformationControlCloser, ControlListener, MouseListener, KeyListener, FocusListener, Listener {
            //TODO: Catch 'Esc' key in fInformationControlToClose: Don't dispose, just hideInformationControl().
            // This would allow to reuse the information control also when the user explicitly closes it.

            //TODO: if subject control is a Scrollable, should add selection listeners to both scroll bars
            // (and remove the ViewPortListener, which only listens to vertical scrolling)

            /** The subject control. */
            private Control fSubjectControl;
            /** Indicates whether this closer is active. */
            private boolean fIsActive= false;
            /** The display. */
            private Display fDisplay;

            /*
             * @see IInformationControlCloser#setSubjectControl(Control)
             */
            public void setSubjectControl(Control control) {
                fSubjectControl= control;
            }

            /*
             * @see IInformationControlCloser#setInformationControl(IInformationControl)
             */
            public void setInformationControl(IInformationControl control) {
                // NOTE: we use getCurrentInformationControl2() from the outer class
            }

            /*
             * @see IInformationControlCloser#start(Rectangle)
             */
            public void start(org.eclipse.swt.graphics.Rectangle informationArea) {

                if (fIsActive)
                    return;
                fIsActive= true;

                if (fSubjectControl != null && !fSubjectControl.isDisposed()) {
                    fSubjectControl.addControlListener(this);
                    fSubjectControl.addMouseListener(this);
                    fSubjectControl.addKeyListener(this);
                }

                //fTextViewer.addViewportListener(this);

                IInformationControl fInformationControlToClose= getCurrentInformationControl2();
                if (fInformationControlToClose != null)
                    fInformationControlToClose.addFocusListener(this);

                fDisplay= fSubjectControl.getDisplay();
                if (!fDisplay.isDisposed()) {
                    fDisplay.addFilter(SWT.MouseMove, this);
                    fDisplay.addFilter(SWT.FocusOut, this);
                }
            }

            /*
             * @see IInformationControlCloser#stop()
             */
            public void stop() {

                if (!fIsActive)
                    return;
                fIsActive= false;

                //fTextViewer.removeViewportListener(this);

                if (fSubjectControl != null && !fSubjectControl.isDisposed()) {
                    fSubjectControl.removeControlListener(this);
                    fSubjectControl.removeMouseListener(this);
                    fSubjectControl.removeKeyListener(this);
                }

                IInformationControl fInformationControlToClose= getCurrentInformationControl2();
                if (fInformationControlToClose != null)
                    fInformationControlToClose.removeFocusListener(this);

                if (fDisplay != null && !fDisplay.isDisposed()) {
                    fDisplay.removeFilter(SWT.MouseMove, this);
                    fDisplay.removeFilter(SWT.FocusOut, this);
                }

                fDisplay= null;
            }

            public void controlResized(ControlEvent e) { hideInformationControl(); }
            public void controlMoved(ControlEvent e) { hideInformationControl(); }
            public void mouseDown(MouseEvent e) { hideInformationControl(); }
            public void mouseUp(MouseEvent e) { }
            public void mouseDoubleClick(MouseEvent e) { hideInformationControl(); }
            public void keyPressed(KeyEvent e) { hideInformationControl(); }
            public void keyReleased(KeyEvent e) { }
            public void focusGained(FocusEvent e) { }

            public void focusLost(FocusEvent e) {
                Display d= fSubjectControl.getDisplay();
                d.asyncExec(new Runnable() {
                    // Without the asyncExec, mouse clicks to the workbench window are swallowed.
                    public void run() {
                        hideInformationControl();
                    }
                });
            }

            public void handleEvent(Event event) {
                if (event.type == SWT.MouseMove) {
                    if (!(event.widget instanceof Control) || event.widget.isDisposed())
                        return;

                    IInformationControl infoControl= getCurrentInformationControl2();
                    if (infoControl != null && !infoControl.isFocusControl() && infoControl instanceof IInformationControlExtension3) {
                        IInformationControlExtension3 iControl3= (IInformationControlExtension3) infoControl;
                        org.eclipse.swt.graphics.Rectangle controlBounds= iControl3.getBounds();
                        if (controlBounds != null) {
                            Point mouseLoc= event.display.map((Control) event.widget, null, event.x, event.y);
                            int margin= getKeepUpMargin();
                            Geometry.expand(controlBounds, margin, margin, margin, margin);
                            if (!controlBounds.contains(mouseLoc)) {
                                hideInformationControl();
                            }
                        }

                    } else {
                        /*
                         * TODO: need better understanding of why/if this is needed.
                         * Looks like the same panic code we have in org.eclipse.jface.text.AbstractHoverInformationControlManager.Closer.handleMouseMove(Event)
                         */
                        if (fDisplay != null && !fDisplay.isDisposed())
                            fDisplay.removeFilter(SWT.MouseMove, this);
                    }

                } else if (event.type == SWT.FocusOut) {
                    IInformationControl iControl= getCurrentInformationControl2();
                    if (iControl != null && ! iControl.isFocusControl())
                        hideInformationControl();
                }
            }
        }

        private LocalInformationControlReplacer(IInformationControlCreator creator) {
            super(creator);
            setCloser(new Closer());
        }
    }

    private HashMap<Control,IHoverTextProvider> hoverTextProviders = new HashMap<Control, IHoverTextProvider>();

    private HashMap<Control, LocalHoverInformationControlManager> hoverInformationControlManagers = new HashMap<Control, LocalHoverInformationControlManager>();

    private Point hoverSizeConstraints = new Point(320, 200);

    /**
     * Returns the maximum size for the hover widget.
     */
    public Point getHoverSizeConstaints() {
        return hoverSizeConstraints;
    }

    /**
     * Sets the maximum size for the hover widget.
     */
    public void setHoverSizeConstaints(int width, int height) {
        this.hoverSizeConstraints = new Point(width, height);
    }

    /**
     * Adds hover support for the given control.
     */
    public void adapt(final Control control, IHoverTextProvider hoverTextProvider) {
        Assert.isTrue(hoverTextProvider != null);
        Assert.isTrue(!hoverTextProviders.containsKey(control), "control already registered");
        hoverTextProviders.put(control, hoverTextProvider);
        control.addDisposeListener(new DisposeListener() {
            public void widgetDisposed(DisposeEvent e) {
                forget(control);
            }
        });
        LocalInformationControlReplacer informationControlReplacer = new LocalInformationControlReplacer(getInformationPresenterControlCreator());
        informationControlReplacer.install(control);
        LocalHoverInformationControlManager hoverInformationControlManager = new LocalHoverInformationControlManager(getHoverControlCreator());
        hoverInformationControlManager.getInternalAccessor().setInformationControlReplacer(informationControlReplacer);
        hoverInformationControlManager.install(control);
        hoverInformationControlManagers.put(control, hoverInformationControlManager);
    }

    /**
     * Removes hover support from the given control. This does NOT need to
     * be called when the widget gets disposed, because this class listens on
     * widgetDisposed() automatically.
     */
    //XXX but widgetDisposed is not always called when the widget gets disposed
    public void forget(Control control) {
        hoverTextProviders.remove(control);
        hoverInformationControlManagers.remove(control);
    }

    /**
     * Creates a sticky hover for the given widget.
     */
    public void makeHoverSticky(Control control) {
        // KLUDGE: unfortunately the fHoverArea in the internal MouseTracker is null, we workaround this by using reflection utils... :(
        LocalHoverInformationControlManager hoverInformationControlManager = hoverInformationControlManagers.get(control);
        ReflectionUtils.setFieldValue(ReflectionUtils.getFieldValue(hoverInformationControlManager, "fMouseTracker"), "fHoverArea", Display.getCurrent().getBounds());
        hoverInformationControlManager.showInformation();
        hoverInformationControlManager.getInternalAccessor().replaceInformationControl(true);
    }

    /**
     * Utility method for ITextHoverExtension.
     */
    public static IInformationControlCreator getHoverControlCreator() {
        return new BrowserInformationControlCreator();
    }

    /**
     * Utility method for IInformationProviderExtension2.
     */
    public static IInformationControlCreator getInformationPresenterControlCreator() {
        return new StickyBrowserInformationControlCreator();
    }

    /**
     * Wraps an HTML formatted string with a stylesheet for hover display
     */
    public static String addHTMLStyleSheet(String htmlText) {
        return htmlText != null ? HTML_PROLOG + htmlText + HTML_EPILOG : null;
    }
}
