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
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.editors.text.EditorsUI;

/**
 * Provides tooltip for a widget. SWT's Control.setTooltipText() has several
 * limitations which makes it unsuitable for presenting large multi-line
 * tooltips:
 *   - tends to wrap long lines (SWT's Tooltip class does that too)
 *   - tooltip disappears after about 5s (Windows), which is not enough time to
 *     read long texts
 *   - lazy generation of tooltip text is not possible (there's no such thing 
 *     as TooltipAboutToShowListener)
 * This class overcomes these limitations.
 * 
 * One instance can adapt several controls (ie. controls may share the same 
 * tooltip).
 *   
 * @author Andras
 */
//XXX rename: HoverSupport!
//XXX problem: if mouse is near the right-bottom corner of the screen, tooltip comes
//up right under the mouse cursor, and won't go away on mouse movement. solution: better placement?
public class TooltipSupport {
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
	protected ITooltipTextProvider defaultTooltipProvider = null;
	protected HashMap<Control,ITooltipTextProvider> tooltipProviders = new HashMap<Control, ITooltipTextProvider>(); 

	protected IInformationControl tooltipControl;
	private IInformationControl informationControl;

	private class AllInOneListener implements MouseListener, MouseTrackListener, MouseMoveListener, KeyListener {
		public void mouseDoubleClick(MouseEvent e) {}
		public void keyReleased(KeyEvent e) {}

		public void mouseHover(MouseEvent e) {
			if (e.widget instanceof Control && (e.stateMask & SWT.BUTTON_MASK) == 0)
				displayTooltip((Control)e.widget, e.x, e.y);
		}

		public void mouseMove(MouseEvent e) {
			removeTooltip();
		}

		public void mouseDown(MouseEvent e) {
			removeTooltip();
		}

		public void keyPressed(KeyEvent e) {
			if (e.keyCode == SWT.F2 && tooltipControl != null)  //XXX query real key binding and use that, etc
				makeTooltipSticky();
			else
				removeTooltip();
		}

		public void mouseUp(MouseEvent e) {
			removeTooltip();
		}

		public void mouseEnter(MouseEvent e) {
			removeTooltip();
		}

		public void mouseExit(MouseEvent e) {
			removeTooltip();
		}
	}

	/**
	 * Create a tooltip support object.
	 */
	public TooltipSupport() {
	}

	/**
	 * Create a tooltip support object.
	 */
	public TooltipSupport(ITooltipTextProvider defaultTooltipProvider) {
		this.defaultTooltipProvider = defaultTooltipProvider;
	}

	/**
	 * Adds tooltip support for the given control, with the default
	 * tooltip text provider that was given in the constructor call.
	 */
	public void adapt(Control c) {
		adapt(c, defaultTooltipProvider);
	}

	/**
	 * Adds tooltip support for the given control.
	 */
	public void adapt(final Control c, ITooltipTextProvider tooltipProvider) {
		Assert.isTrue(tooltipProvider != null);
		Assert.isTrue(!tooltipProviders.containsKey(c), "control already registered");
		tooltipProviders.put(c, tooltipProvider);

		c.addMouseListener(eventListener);
		c.addMouseMoveListener(eventListener);
		c.addMouseTrackListener(eventListener);
		c.addKeyListener(eventListener);
		c.addDisposeListener(new DisposeListener() {
			public void widgetDisposed(DisposeEvent e) {
				forget(c);
			}
		});
	}

	/**
	 * Removes tooltip support from the given control. This does NOT need to 
	 * be called when the widget gets disposed, because this class listens on
	 * widgetDisposed() automatically.
	 */
	public void forget(Control c) {
		tooltipProviders.remove(c);

		c.removeMouseListener(eventListener);
		c.removeMouseMoveListener(eventListener);
		c.removeMouseTrackListener(eventListener);
		c.removeKeyListener(eventListener);
	}

	protected void displayTooltip(Control control, int x, int y) {
		if (informationControl != null)
			return; // we are already showing a (sticky) information control

		ITooltipTextProvider tooltipProvider = tooltipProviders.get(control);
		String tooltipText = tooltipProvider.getTooltipFor(control, x, y);
		if (tooltipText != null) {
			tooltipControl = getHoverControlCreator().createInformationControl(control.getShell());
			configureControl(tooltipControl, tooltipText, control.toDisplay(x, y));
			tooltipControl.addDisposeListener(new DisposeListener() {
				public void widgetDisposed(DisposeEvent e) {
					tooltipControl = null;
				}
			});
		}
	}

	protected void removeTooltip() {
		if (tooltipControl != null)
			tooltipControl.dispose();
	}

	protected void makeTooltipSticky() {
		removeTooltip();
		
		Display.getDefault().getCursorLocation();
		Control control = Display.getDefault().getCursorControl();
		Point p = Display.getDefault().getCursorLocation();

		ITooltipTextProvider tooltipProvider = tooltipProviders.get(control);
		String tooltipText = tooltipProvider.getTooltipFor(control, control.toControl(p).x, control.toControl(p).y);
		if (tooltipText != null) {
			// create the control
			informationControl = getInformationPresenterControlCreator().createInformationControl(control.getShell());
			configureControl(informationControl, tooltipText, p);
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

	protected void configureControl(IInformationControl informationControl, String tooltipText, Point mouseLocation) {
		informationControl.setSizeConstraints(320, 200);
		informationControl.setInformation(tooltipText);
		informationControl.setLocation(new Point(mouseLocation.x + 5, mouseLocation.y + 20));
		Point size = informationControl.computeSizeHint();
		informationControl.setSize((int)(size.x), size.y); // add some right/bottom margin 
		informationControl.setVisible(true);
	}

	/**
	 * Utility method for ITextHoverExtension.
	 */
	public static IInformationControlCreator getHoverControlCreator() {
		return new IInformationControlCreator() {
			@SuppressWarnings("restriction")
			public IInformationControl createInformationControl(Shell parent) {
				// for more info, see JavadocHover class in JDT
				int shellStyle = SWT.TOOL;
				int style = SWT.NONE;
				if (BrowserInformationControl.isAvailable(parent))
					return new BrowserInformationControl(parent, shellStyle, style, EditorsUI.getTooltipAffordanceString());
				else
					return new DefaultInformationControl(parent, shellStyle, style, new HTMLTextPresenter(false), EditorsUI.getTooltipAffordanceString());
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
	 * Wraps an HTML formatted string with a stylsheet for tooltip display
	 * @param htmlText
	 * @return
	 */
	public static String addHTMLStyleSheet(String htmlText) {
	    return htmlText != null ? HTML_PROLOG + htmlText + HTML_EPILOG : null;
	}
	    

}


