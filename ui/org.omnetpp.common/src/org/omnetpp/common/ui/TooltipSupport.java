package org.omnetpp.common.ui;

import org.eclipse.jface.internal.text.html.BrowserInformationControl;
import org.eclipse.jface.internal.text.html.HTMLTextPresenter;
import org.eclipse.jface.text.DefaultInformationControl;
import org.eclipse.jface.text.IInformationControl;
import org.eclipse.jface.text.IInformationControlCreator;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.DisposeEvent;
import org.eclipse.swt.events.DisposeListener;
import org.eclipse.swt.events.KeyEvent;
import org.eclipse.swt.events.KeyListener;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseListener;
import org.eclipse.swt.events.MouseMoveListener;
import org.eclipse.swt.events.MouseTrackListener;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Control;
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
// up right under the mouse cursor, and won't go away on mouse movement. solution: better placement?
public class TooltipSupport {
	protected IInformationControl tooltipControl;
	protected AllInOneListener eventListener = new AllInOneListener();
	protected ITooltipTextProvider tooltipProvider;

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
	public TooltipSupport(ITooltipTextProvider tooltipProvider) {
		this.tooltipProvider = tooltipProvider;
	}

	/**
	 * Create a tooltip support object, and adapt the given control.
	 */
	public TooltipSupport(final Control c, ITooltipTextProvider tooltipProvider) {
		this(tooltipProvider);
		adapt(c);
	}

	/**
	 * Add tooltip to a control.  
	 */
	public static TooltipSupport adapt(Control c, ITooltipTextProvider tooltipProvider) {
		return new TooltipSupport(c, tooltipProvider);
	}
	
	/**
	 * Adds tooltip support for the given control.
	 */
	public void adapt(final Control c) {
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
		c.removeMouseListener(eventListener);
		c.removeMouseMoveListener(eventListener);
		c.removeMouseTrackListener(eventListener);
		c.removeKeyListener(eventListener);
	}
	
	protected void displayTooltip(Control control, int x, int y) {
		String tooltipText = tooltipProvider.getTooltipFor(control, x, y);
		if (tooltipText!=null) {
			tooltipControl = getHoverControlCreator().createInformationControl(control.getShell());
			tooltipControl.setSizeConstraints(320, 200);
			tooltipControl.setInformation(tooltipText);
			tooltipControl.setLocation(control.toDisplay(x+5,y+20));
			Point size = tooltipControl.computeSizeHint();
			tooltipControl.setSize(size.x+3, size.y+3); // add some right/bottom margin 
			tooltipControl.setVisible(true);
		}
	}

	protected void removeTooltip() {
		if (tooltipControl!=null) {
			tooltipControl.setVisible(false);
			tooltipControl.dispose();
			tooltipControl = null;
		}
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

}


