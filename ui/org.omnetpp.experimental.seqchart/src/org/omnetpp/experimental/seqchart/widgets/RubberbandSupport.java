package org.omnetpp.experimental.seqchart.widgets;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseListener;
import org.eclipse.swt.events.MouseMoveListener;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Canvas;

/**
 * Adds rubber-band selection support to a canvas. When the user drags out
 * a rubber band rectangle, rubberBandSelectionMade() is called.
 *
 * @author andras
 */
public abstract class RubberbandSupport {

	private Canvas canvas;
	private Rectangle rubberBand = null;
	private Rectangle rubberBandBounds = null; // rubberbandable area; null means "whole canvas"

	public RubberbandSupport(Canvas canvas) {
		this.canvas = canvas;
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
	 * Called back when an area was dragged out in the chart.
	 */
	public abstract void rubberBandSelectionMade(Rectangle r);

	/**
	 * Called back when the user clicked the canvas without dragging out a rubber band.
	 */
	public abstract void clicked(int x, int y);
	
	/**
	 * Implements rubber band selection. Successful selections
	 * will cause rubberBandSelectionMade() to be called.
	 */
	private void addListeners() {
		canvas.addMouseListener(new MouseListener() {
	    	public void mouseDown(MouseEvent e) {
	    		if (rubberBand==null && e.button==1 && (rubberBandBounds==null || rubberBandBounds.contains(e.x, e.y))) {
					// start selection
					rubberBand = new Rectangle(e.x, e.y, 0, 0);
					GC gc = new GC(canvas);
					drawRubberBand(gc, rubberBand);
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
						if (rubberBand.width<=2 && rubberBand.height<=2)
							clicked(e.x, e.y);
						else
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
					// erase, then draw new with updated coordinates
					GC gc = new GC(canvas);
					gc.setForeground(canvas.getDisplay().getSystemColor(SWT.COLOR_RED));
					drawRubberBand(gc, rubberBand);
					rubberBand.width = e.x - rubberBand.x;
					rubberBand.height = e.y - rubberBand.y;
					clipToBounds(rubberBand, rubberBandBounds==null ? canvas.getClientArea() : rubberBandBounds);
					drawRubberBand(gc, rubberBand);
				}
			}
	    });
	}

	private static void drawRubberBand(GC gc, Rectangle rect) {
		// needed because gc.drawFocus() doesn't accept negative width/height
		Rectangle r = new Rectangle(rect.x, rect.y, rect.width, rect.height);
		fixNegativeSizes(r);
		gc.drawFocus(r.x, r.y, r.width, r.height);
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