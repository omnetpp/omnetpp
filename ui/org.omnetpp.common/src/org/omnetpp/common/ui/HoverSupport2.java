package org.omnetpp.common.ui;

import java.util.HashMap;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.internal.text.html.BrowserInformationControl;
import org.eclipse.jface.internal.text.html.HTMLTextPresenter;
import org.eclipse.jface.text.DefaultInformationControl;
import org.eclipse.jface.text.IInformationControl;
import org.eclipse.jface.text.IInformationControlCreator;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.DisposeEvent;
import org.eclipse.swt.events.DisposeListener;
import org.eclipse.swt.events.FocusEvent;
import org.eclipse.swt.events.FocusListener;
import org.eclipse.swt.events.KeyEvent;
import org.eclipse.swt.events.KeyListener;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseListener;
import org.eclipse.swt.events.MouseMoveListener;
import org.eclipse.swt.events.MouseTrackListener;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;

/**
 * NOT IN USE.  Please use HoverSupport!
 *
 * This class uses keyboard/mouse listeners added to the widget ONLY. It has limitations,
 * e.g. tooltips that appear during cell editing/content assist don't go away
 * on user typing etc. Another flavor of this class, HoverSupport, uses global 
 * event filters  to circumvent this problem, which however might a bit too strong 
 * and potentially dangerous.
 * 
 * @author Andras
 */
public class HoverSupport2 {
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
        "pre    { margin-left: 0.6em; }\n" +
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

    protected AllInOneListener eventListener = new AllInOneListener();
	protected IHoverTextProvider defaultHoverTextProvider = null;
	protected HashMap<Control,IHoverTextProvider> hoverTextProviders = new HashMap<Control, IHoverTextProvider>(); 
	protected Point hoverSizeConstaints = new Point(320, 200);
	
	protected IInformationControl hoverControl;
	protected IInformationControl informationControl;
	

	private class AllInOneListener implements MouseListener, MouseTrackListener, MouseMoveListener, KeyListener, FocusListener {
		public void mouseDoubleClick(MouseEvent e) {}
		public void keyReleased(KeyEvent e) {}

		public void mouseHover(MouseEvent e) {
			if (e.widget instanceof Control && (e.stateMask & SWT.BUTTON_MASK) == 0)
				displayHover((Control)e.widget, e.x, e.y);
		}

		public void mouseMove(MouseEvent e) {
			removeHover();
		}

		public void mouseDown(MouseEvent e) {
			removeHover();
		}

		public void keyPressed(KeyEvent e) {
			if (e.keyCode == SWT.F2 && hoverControl != null)  //XXX query real key binding and use that, etc
				makeHoverSticky();
			else
				removeHover();
		}

		public void mouseUp(MouseEvent e) {
			removeHover();
		}

		public void mouseEnter(MouseEvent e) {
			removeHover();
		}

		public void mouseExit(MouseEvent e) {
			removeHover();
		}

		public void focusGained(FocusEvent e) {
			removeHover();
		}

		public void focusLost(FocusEvent e) {
			removeHover();
		}
	}

	/**
	 * Create a hover support object.
	 */
	public HoverSupport2() {
	}

	/**
	 * Create a hover support object.
	 */
	public HoverSupport2(IHoverTextProvider defaultHoverTextProvider) {
		this.defaultHoverTextProvider = defaultHoverTextProvider;
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
		Assert.isTrue(hoverTextProvider != null);
		Assert.isTrue(!hoverTextProviders.containsKey(c), "control already registered");
		hoverTextProviders.put(c, hoverTextProvider);

		c.addMouseListener(eventListener);
		c.addMouseMoveListener(eventListener);
		c.addMouseTrackListener(eventListener);
		c.addKeyListener(eventListener);
		c.addFocusListener(eventListener);
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
	public void forget(Control c) {
		hoverTextProviders.remove(c);

		c.removeMouseListener(eventListener);
		c.removeMouseMoveListener(eventListener);
		c.removeMouseTrackListener(eventListener);
		c.removeKeyListener(eventListener);
		c.removeFocusListener(eventListener);
	}

	protected void displayHover(Control control, int x, int y) {
		if (informationControl != null)
			return; // we are already showing a (sticky) information control

		IHoverTextProvider hoverTextProvider = hoverTextProviders.get(control);
		SizeConstraint preferredSize = new SizeConstraint();
		String hoverText = hoverTextProvider.getHoverTextFor(control, x, y, preferredSize);
		if (hoverText != null) {
			hoverControl = getHoverControlCreator().createInformationControl(control.getShell());
			configureControl(hoverControl, hoverText, control.toDisplay(x, y));
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
		removeHover();
		
		Display.getDefault().getCursorLocation();
		Control control = Display.getDefault().getCursorControl();
		Point p = Display.getDefault().getCursorLocation();

		IHoverTextProvider hoverProvider = hoverTextProviders.get(control);
		SizeConstraint preferredSize = new SizeConstraint();
		String hoverText = hoverProvider.getHoverTextFor(control, control.toControl(p).x, control.toControl(p).y, preferredSize);
		if (hoverText != null) {
			// create the control
			informationControl = getInformationPresenterControlCreator().createInformationControl(control.getShell());
			configureControl(informationControl, hoverText, p);
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
					informationControl.dispose();
				}
			});
		}
	}

	protected void configureControl(IInformationControl informationControl, String hoverText, Point mouseLocation) {
		informationControl.setSizeConstraints(hoverSizeConstaints.x, hoverSizeConstaints.y);
		informationControl.setInformation(hoverText);
		Point size = informationControl.computeSizeHint(); //XXX issue: BrowserInformationControl is always at least 80 pixels high -- this is hardcoded :(
		informationControl.setSize(size.x, size.y);
		informationControl.setLocation(calculateHoverPosition(mouseLocation, size));
		informationControl.setVisible(true);
	}

	protected Point calculateHoverPosition(Point mouse, Point size) {
		Rectangle screen = Display.getCurrent().getBounds();
		Point p = new Point(mouse.x + 5, mouse.y + 10);
		p.x = Math.min(p.x, screen.width - size.x - 5);
		if (p.y + 10 + size.y > screen.y + screen.height)
			p.y = mouse.y - size.y - 10; // if no room below mouse, show it above
		return p;
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
		return hoverSizeConstaints;
	}

	/**
	 * Sets the maximum size for the hover widget.
	 */
	public void setHoverSizeConstaints(Point hoverSizeConstaints) {
		this.hoverSizeConstaints = hoverSizeConstaints;
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
			public IInformationControl createInformationControl(Shell parent) {
				// for more info, see JavadocHover class in JDT
				if (BrowserInformationControl.isAvailable(parent))
					return new BrowserInformationControl(parent, null, true);
				else
					return new DefaultInformationControl(parent, new HTMLTextPresenter(false));
			}
		};
	}

	/**
	 * Utility method for IInformationProviderExtension2.
	 */
	public static IInformationControlCreator getInformationPresenterControlCreator() {
		return new IInformationControlCreator() {
			public IInformationControl createInformationControl(Shell parent) {
				if (BrowserInformationControl.isAvailable(parent))
					return new BrowserInformationControl(parent, null, true);
				else
					return new DefaultInformationControl(parent, new HTMLTextPresenter(false));
			}
		};
	}

	/**
	 * Wraps an HTML formatted string with a stylesheet for hover display
	 * @param htmlText
	 * @return
	 */
	public static String addHTMLStyleSheet(String htmlText) {
	    return htmlText != null ? HTML_PROLOG + htmlText + HTML_EPILOG : null;
	}

}


