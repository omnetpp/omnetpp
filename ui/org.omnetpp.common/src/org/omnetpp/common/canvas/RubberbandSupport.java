package org.omnetpp.common.canvas;

import static org.eclipse.draw2d.PositionConstants.EAST;
import static org.eclipse.draw2d.PositionConstants.NORTH;
import static org.eclipse.draw2d.PositionConstants.SOUTH;
import static org.eclipse.draw2d.PositionConstants.WEST;

import org.eclipse.draw2d.Cursors;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseListener;
import org.eclipse.swt.events.MouseMoveListener;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.color.ColorFactory;

/**
 * Adds rubber-band selection support to a canvas. When the user drags out
 * a rubber band rectangle, rubberBandSelectionMade() is called.
 *
 * @author andras
 */
public abstract class RubberbandSupport {
	
	private Canvas canvas;
	private int modifierKeys = 0; // modifier key that needs to be held down for rubber-band (e.g. SWT.CTRL)
	private Rectangle rubberBand = null;
	private Rectangle rubberBandBounds = null; // rubberbandable area; null means "whole canvas"
	
	// If true then the rubberband is drawn from the mouse listener
	// otherwise the drawRubberBand() function should be called from the paint listener of the canvas.
	private boolean drawRubberBand = false;

	public RubberbandSupport(Canvas canvas, int modifierKeys) {
		this.canvas = canvas;
		this.modifierKeys = modifierKeys;
		addListeners();
	}

	/**
	 * Returns the bounds of the data area to which the rubber band is clipped
	 * A null value stands for the whole canvas area.
	 */
	public Rectangle getRubberBandBounds() {
		return rubberBandBounds;
	}

	/**
	 * Sets the bounds of the data area to which the rubber band is clipped.
	 * null is permitted, and stands for the whole canvas area.
	 */
	public void setRubberBandBounds(Rectangle rubberBandBounds) {
		this.rubberBandBounds = rubberBandBounds;
	}
	
	/**
	 * Return the modifier key(s) that need to be held down for rubber-band (e.g. SWT.CTRL).
	 * Corresponds to SWT Event's stateMask. 
	 */
	public int getModifierKeys() {
		return modifierKeys;
	}

	/**
	 * Sets the modifier key(s) that need to be held down for rubber-band (e.g. SWT.CTRL).
	 * Corresponds to SWT Event's stateMask. Zero means no modifier key is needed.  
	 */
	public void setModifierKeys(int modifierKeys) {
		this.modifierKeys = modifierKeys;
	}

	/**
	 * Called back when an area was dragged out in the chart.
	 */
	public abstract void rubberBandSelectionMade(Rectangle r);
	
	/**
	 * Implements rubber band selection. Successful selections
	 * will cause rubberBandSelectionMade() to be called.
	 */
	private void addListeners() {
		canvas.addMouseListener(new MouseListener() {
			public void mouseDown(MouseEvent e) {
	    		if (rubberBand==null && e.button==1 && 
	    			((e.stateMask & SWT.MODIFIER_MASK)==modifierKeys) && 
	    			(rubberBandBounds==null || rubberBandBounds.contains(e.x, e.y))) 
	    		{
					// start selection
					rubberBand = new Rectangle(e.x, e.y, 0, 0);
					if (drawRubberBand) {
						GC gc = new GC(canvas);
						drawRubberBand(gc, rubberBand);
					}
					else
						canvas.redraw();
				}
	    		if (rubberBand!=null && e.button!=1) {
	    			// cancel selection
		    		rubberBand = null;
		    		canvas.redraw();
	    		}
	    	}
	    	public void mouseUp(MouseEvent e) {
				if (rubberBand!=null) {
	    			// finish/cancel selection
					canvas.redraw();
					if (e.button==1) {
						// note: firing a SelectionEvent wouldn't work (width,
						// height won't make it through Listener.handleEvent())
						fixNegativeSizes(rubberBand);
						if (rubberBand.width>2 || rubberBand.height>2) // not just a click
							rubberBandSelectionMade(rubberBand);
					}
					rubberBand = null;
	    		}
	    	}
			public void mouseDoubleClick(MouseEvent e) {
			}
	    });

		canvas.addMouseMoveListener(new MouseMoveListener() {
			public void mouseMove(MouseEvent e) {
				if (rubberBand!=null) {
					Rectangle origRect = new Rectangle(rubberBand.x, rubberBand.y, rubberBand.width, rubberBand.height);
					rubberBand.width = e.x - rubberBand.x;
					rubberBand.height = e.y - rubberBand.y;
					clipToBounds(rubberBand, rubberBandBounds==null ? canvas.getClientArea() : rubberBandBounds);
					
					if ((e.stateMask & SWT.MODIFIER_MASK) == modifierKeys) {
						int direction = (rubberBand.width >= 0 ? EAST : WEST) |
										(rubberBand.height >= 0 ? SOUTH : NORTH);
						canvas.setCursor(Cursors.getDirectionalCursor(direction));
					}
					
					// erase, then draw new with updated coordinates
					if (drawRubberBand) {
						GC gc = new GC(canvas);
						gc.setForeground(canvas.getDisplay().getSystemColor(SWT.COLOR_RED));
						drawRubberBand(gc, origRect); // XXX how does it erase the old rect ???
						drawRubberBand(gc, rubberBand);
					}
					else
						canvas.redraw();
				}
			}
	    });
	}

	private void drawRubberBand(GC gc, Rectangle rect) {
		// Debug.println("rubberBand="+rect);
		Rectangle r = new Rectangle(rect.x, rect.y, rect.width, rect.height);
		// needed because gc.drawFocus() doesn't accept negative width/height
		fixNegativeSizes(r);
		
		
		if (drawRubberBand) {
			// On windows when Desktop Control Panel/Appearance/Effects/"Hide underlined letters for keyboard navigation until I press the Alt key"
			// is turned on, the focus rectangles are not displayed until the user press the Alt key or switch to another 
			// window and back. See WM_QUERYUISTATE,WM_UPDATEUISTATE.
			// Commenting out the following lines can solve the problem, but uses discouraged access
			
//					GCData data = gc.getGCData();
//					data.uiState &= (~OS.UISF_HIDEFOCUS);
			gc.setForeground(Display.getCurrent().getSystemColor(SWT.COLOR_GRAY));	// used if drawFocus() is not supported by the OS 
			gc.drawFocus(r.x, r.y, r.width, r.height);
		}
		else {
			gc.setForeground(ColorFactory.GREY8);
			int antialias = gc.getAntialias();
			gc.setAntialias(SWT.OFF);
			gc.setLineStyle(SWT.LINE_DOT);
			gc.drawRectangle(r);
			gc.setAntialias(antialias);
		}
	}
	
	/**
	 * To be called from the Paint handler of the canvas if draw == false.
	 */
	public void drawRubberband(GC gc) {
		//Debug.format("Focus: %s %x%n", canvas.getDisplay().getFocusControl(), canvas.getDisplay().getFocusControl().hashCode());
		//Debug.format("UIState: "+OS.SendMessage (canvas.handle, OS.WM_QUERYUISTATE, 0, 0));
		if (rubberBand != null)
			drawRubberBand(gc, rubberBand);
	}

	private static void fixNegativeSizes(Rectangle r) {
		if (r.width<0) {
			r.width = -r.width;
			r.x -= r.width;
		}
		if (r.height<0) {
			r.height = -r.height;
			r.y -= r.height;
		}
	}

	private static void clipToBounds(Rectangle r, Rectangle r2) {
		// r may have negative height/width which intersect() doesn't like.
		// so first fix this, then calculate intersect, then restore negative width/height
		boolean widthNegative = r.width<0;
		boolean heightNegative = r.height<0;
		if (r.width<0) {
			r.width = -r.width;
			r.x -= r.width;
		}
		if (r.height<0) {
			r.height = -r.height;
			r.y -= r.height;
		}
		r.intersect(r2);
		if (widthNegative) {
			r.x += r.width;
			r.width = -r.width;
		}
		if (heightNegative) {
			r.y += r.height;
			r.height = -r.height;
		}
	}

}