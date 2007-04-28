package org.omnetpp.common.ui;

import org.eclipse.jface.text.DefaultInformationControl;
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

/**
 * Provides tooltip for a widget. SWT's Control.setTooltipText() has several
 * limitations which makes it unsuitable for presenting large multi-line
 * tooltips:
 *   - tends to wrap long lines (SWT's Tooltip class does that too)
 *   - tooltip disappears after about 5s (Windows), which is not enough to
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
//XXX problem: if mouse is near the right-bottom corner of the screen, tooltip comes
// up right under the mouse cursor, and won't go away on mouse movement. solution: better placement?
public class TooltipSupport {
	protected DefaultInformationControl tooltipWidget;
	protected AllInOneListener eventListener = new AllInOneListener();
	protected ITooltipProvider tooltipProvider;

	private class AllInOneListener implements MouseListener, MouseTrackListener, MouseMoveListener, KeyListener {
		public void mouseDoubleClick(MouseEvent e) {}
		public void mouseUp(MouseEvent e) {}
		public void mouseEnter(MouseEvent e) {}
		public void mouseExit(MouseEvent e) {}
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
	}
	
	/**
	 * Create a tooltip support object.
	 */
	public TooltipSupport(ITooltipProvider tooltipProvider) {
		this.tooltipProvider = tooltipProvider;
	}

	/**
	 * Create a tooltip support object, and adapt the given control.
	 */
	public TooltipSupport(final Control c, ITooltipProvider tooltipProvider) {
		this(tooltipProvider);
		adapt(c);
	}

	/**
	 * Add tooltip to a control.  
	 */
	public static TooltipSupport adapt(Control c, ITooltipProvider tooltipProvider) {
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
			tooltipWidget = new DefaultInformationControl(control.getShell());
			tooltipWidget.setInformation(" "+tooltipText.replaceAll("\n", "\n ")); // prefix each line with a space, for left margin
			tooltipWidget.setLocation(control.toDisplay(x+5,y+20));
			Point size = tooltipWidget.computeSizeHint();
			tooltipWidget.setSize(size.x+3, size.y+3); // add some right/bottom margin 
			tooltipWidget.setVisible(true);
		}
	}

	protected void removeTooltip() {
		if (tooltipWidget!=null) {
			tooltipWidget.setVisible(false);
			tooltipWidget.dispose();
			tooltipWidget = null;
		}
	}
}


