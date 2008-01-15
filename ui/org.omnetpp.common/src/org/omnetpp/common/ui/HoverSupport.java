package org.omnetpp.common.ui;

import java.util.HashMap;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.internal.text.html.HTMLTextPresenter;
import org.eclipse.jface.text.DefaultInformationControl;
import org.eclipse.jface.text.IInformationControl;
import org.eclipse.jface.text.IInformationControlCreator;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.DisposeEvent;
import org.eclipse.swt.events.DisposeListener;
import org.eclipse.swt.events.FocusEvent;
import org.eclipse.swt.events.FocusListener;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseTrackListener;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Monitor;
import org.eclipse.swt.widgets.Shell;

/**
 * Provides hover for a widget. SWT's Control.setTooltipText() has several
 * limitations which makes it unsuitable for presenting large multi-line
 * tooltips:
 *   - tends to wrap long lines (SWT's Tooltip class does that too)
 *   - tooltip disappears after about 5s (Windows), which is not enough time to
 *     read long texts
 *   - lazy generation of tooltip text is not possible (there's no such thing
 *     as TooltipAboutToShowListener)
 * This class overcomes these limitations.
 *
 * One instance can adapt several controls (i.e. controls may share the same
 * hover).
 *
 * @author Andras
 */
public class HoverSupport {
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

	protected IHoverTextProvider defaultHoverTextProvider = null;
	protected HashMap<Control,IHoverTextProvider> hoverTextProviders = new HashMap<Control, IHoverTextProvider>();
	protected Point hoverSizeConstraints = new Point(320, 200);

	protected IInformationControl hoverControl;
	protected IInformationControl informationControl;

	/**
	 * Listener added to each adapted widget: mouse hovering should show the tooltip
	 */
	protected MouseTrackListener mouseTrackListener = new MouseTrackListener() {
		public void mouseEnter(MouseEvent e) {}
		public void mouseExit(MouseEvent e) {}

		public void mouseHover(MouseEvent e) {
			if (e.widget instanceof Control && (e.stateMask & SWT.BUTTON_MASK) == 0)
				displayHover((Control)e.widget, e.x, e.y);
		}
	};

	/**
	 * A global (workbench-wide) event filter, responsible for removing the tooltip,
	 * or making it persistent (F2 key). This listener is hooked on Display when the first
	 * widget gets adapted, and unhooked when the last widget gets forgotten (or disposed).
	 *
	 * Global event filters are not without dangers; we chose to use it because this way
	 * we can catch keyboard events wherever the focus is.
	 */
	protected Listener eventFilter = new Listener() {
		public void handleEvent(Event e) {
			if (hoverControl != null) {
				if (e.type == SWT.KeyDown && e.keyCode == SWT.F2)
					makeHoverSticky();
				else
					removeHover();
			}
		}
	};

	/**
	 * Create a hover support object.
	 */
	public HoverSupport() {
	}

	/**
	 * Create a hover support object.
	 */
	public HoverSupport(IHoverTextProvider defaultHoverTextProvider) {
		this.defaultHoverTextProvider = defaultHoverTextProvider;
	}

	protected void hookGlobalEventFilter() {
		// System.out.println("HoverSupport: hooking global listener");
		Display.getDefault().addFilter(SWT.MouseExit, eventFilter);
		Display.getDefault().addFilter(SWT.MouseMove, eventFilter);
		Display.getDefault().addFilter(SWT.MouseDown, eventFilter);
		Display.getDefault().addFilter(SWT.KeyDown, eventFilter);
	}

	protected void unhookGlobalEventFilter() {
		// System.out.println("HoverSupport: unhooking global listener");
		Display.getDefault().removeFilter(SWT.MouseExit, eventFilter);
		Display.getDefault().removeFilter(SWT.MouseMove, eventFilter);
		Display.getDefault().removeFilter(SWT.MouseDown, eventFilter);
		Display.getDefault().removeFilter(SWT.KeyDown, eventFilter);
	}

	/**
	 * Adds hover support for the given control, with the default
	 * hover text provider that was given in the constructor call.
	 */
	public void adapt(Control c) {
		adapt(c, defaultHoverTextProvider);
	}

	/**
	 * Adds hover support for the given control.
	 */
	public void adapt(final Control c, IHoverTextProvider hoverTextProvider) {
		if (hoverTextProviders.isEmpty())
			hookGlobalEventFilter();

		Assert.isTrue(hoverTextProvider != null);
		Assert.isTrue(!hoverTextProviders.containsKey(c), "control already registered");
		hoverTextProviders.put(c, hoverTextProvider);

		c.addMouseTrackListener(mouseTrackListener);
		c.addDisposeListener(new DisposeListener() {
			public void widgetDisposed(DisposeEvent e) {
				forget(c);
			}
		});
	}

	/**
	 * Removes hover support from the given control. This does NOT need to
	 * be called when the widget gets disposed, because this class listens on
	 * widgetDisposed() automatically.
	 */
	//XXX but widgetDisposed is not always called when the widget gets disposed
	public void forget(Control c) {
		c.removeMouseTrackListener(mouseTrackListener);
		hoverTextProviders.remove(c);

		if (hoverTextProviders.isEmpty())
			unhookGlobalEventFilter();
	}

	protected void displayHover(Control control, int x, int y) {
		if (hoverControl != null || informationControl != null)
			return; // we are already showing a hover or a (sticky) information control

		IHoverTextProvider hoverTextProvider = hoverTextProviders.get(control);
		SizeConstraint preferredSize = new SizeConstraint();
		String hoverText = hoverTextProvider.getHoverTextFor(control, x, y, preferredSize);
		if (hoverText != null) {
			hoverControl = getHoverControlCreator().createInformationControl(control.getShell());
			configureControl(hoverControl, hoverText, control.toDisplay(x, y), preferredSize);
			hoverControl.addDisposeListener(new DisposeListener() {
				public void widgetDisposed(DisposeEvent e) {
					hoverControl = null;
				}
			});
		}
	}

	protected void removeHover() {
		if (hoverControl != null)
			hoverControl.dispose();
	}

	/**
	 * Makes the currently displayed hover "sticky". This is the method to be called
	 * from the hander of the SHOW_INFORMATION command (bound to "F2" by default).
	 */
    public void makeHoverSticky() {
        Control control = Display.getDefault().getCursorControl();
        if (control != null)
            makeHoverSticky(control);
    }

    /**
     * Creates a sticky hover for the given widget. 
     */
    public void makeHoverSticky(Control control) {
        removeHover();

        IHoverTextProvider hoverProvider = hoverTextProviders.get(control);
        if (hoverProvider != null) {
            Point p = Display.getDefault().getCursorLocation();
            SizeConstraint preferredSize = new SizeConstraint();
            String hoverText = hoverProvider.getHoverTextFor(control, control.toControl(p).x, control.toControl(p).y, preferredSize);
            if (hoverText != null) {
                // create the control
                informationControl = getInformationPresenterControlCreator().createInformationControl(control.getShell());
                configureControl(informationControl, hoverText, p, preferredSize);
                informationControl.setFocus();

                // it should close on losing the focus
                informationControl.addDisposeListener(new DisposeListener() {
                    public void widgetDisposed(DisposeEvent e) {
                        informationControl = null;
                    }
                });
                informationControl.addFocusListener(new FocusListener() {
                    public void focusGained(FocusEvent e) {
                    }
                    public void focusLost(FocusEvent e) {
                    	// async exec needed because on browser creation the focus is temporarily lost
                    	// and transferred to the browser (this would close the infoControl on browser creation)
            			Display.getCurrent().asyncExec(new Runnable() {
            				public void run() {
            					if (informationControl == null || !informationControl.isFocusControl())
            						informationControl.dispose();
            				}
            			});
                    }
                });
            }
		}
	}

	protected void configureControl(IInformationControl informationControl, String hoverText, Point mouseLocation, SizeConstraint preferredSize) {
		informationControl.setSizeConstraints(hoverSizeConstraints.x, hoverSizeConstraints.y);
		informationControl.setInformation(hoverText);
		Point size = informationControl.computeSizeHint(); //XXX issue: BrowserInformationControl is always at least 80 pixels high -- this is hardcoded :(
		size.x = calculateSize(hoverSizeConstraints.x, size.x, preferredSize.minimumWidth, preferredSize.preferredWidth);
		size.y = calculateSize(hoverSizeConstraints.y, size.y, preferredSize.minimumHeight, preferredSize.preferredHeight); 
		informationControl.setSize(size.x, size.y);
		informationControl.setLocation(calculateHoverPosition(mouseLocation, size));
		informationControl.setVisible(true);
	}

	private int calculateSize(int hoverSizeConstraint, int sizeHint, int minimumSize, int preferredSize) {
		int size = sizeHint;

		if (minimumSize != SWT.DEFAULT)
			size = Math.max(minimumSize, size);

		if (preferredSize != SWT.DEFAULT)
			size = Math.min(preferredSize, hoverSizeConstraint);

		return size;
	}

	protected Point calculateHoverPosition(Point mouse, Point size) {
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

	/**
	 * Returns the default over text provider.
	 */
	public IHoverTextProvider getDefaultHoverTextProvider() {
		return defaultHoverTextProvider;
	}

	/**
	 * Sets the default over text provider. This will be used for control adapted
	 * after this call without a hover provider.
	 */
	public void setDefaultHoverTextProvider(IHoverTextProvider defaultHoverTextProvider) {
		this.defaultHoverTextProvider = defaultHoverTextProvider;
	}

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
	 * Returns the hover control if currently displayed, otherwise null.
	 */
	public IInformationControl getHoverControl() {
		return hoverControl;
	}

	/**
	 * Returns the (sticky) information control if currently displayed, otherwise null.
	 */
	public IInformationControl getInformationControl() {
		return informationControl;
	}

	/**
	 * Utility method for ITextHoverExtension.
	 */
	public static IInformationControlCreator getHoverControlCreator() {
		return new IInformationControlCreator() {
			@SuppressWarnings("restriction")
			public IInformationControl createInformationControl(Shell parent) {
				// for more info, see JavadocHover class in JDT
				int shellStyle = SWT.TOOL | SWT.ON_TOP;
				int style = SWT.NONE;
				String tooltipAffordanceString = "Press 'F2' for focus."; //TODO use EditorsUI.getTooltipAffordanceString();
				if (BrowserInformationControl.isAvailable(parent))
					return new BrowserInformationControl(parent, shellStyle, style, tooltipAffordanceString);
				else
					return new DefaultInformationControl(parent, shellStyle, style, new HTMLTextPresenter(false), tooltipAffordanceString);
			}
		};
	}

	/**
	 * Utility method for IInformationProviderExtension2.
	 */
	public static IInformationControlCreator getInformationPresenterControlCreator() {
		return new IInformationControlCreator() {
			@SuppressWarnings("restriction")
			public IInformationControl createInformationControl(Shell parent) {
				// for more info, see JavadocHover class in JDT
				int shellStyle = SWT.RESIZE | SWT.TOOL;
				int style = SWT.V_SCROLL | SWT.H_SCROLL;
				if (BrowserInformationControl.isAvailable(parent))
					return new BrowserInformationControl(parent, shellStyle, style);
				else
					return new DefaultInformationControl(parent, shellStyle, style, new HTMLTextPresenter(false));
			}
		};
	}

	/**
	 * Wraps an HTML formatted string with a stylesheet for hover display
	 */
	public static String addHTMLStyleSheet(String htmlText) {
	    return htmlText != null ? HTML_PROLOG + htmlText + HTML_EPILOG : null;
	}

}


