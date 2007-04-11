package org.omnetpp.common.canvas;

import org.eclipse.core.runtime.Assert;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.ScrollBar;

/**
 * A canvas that supports a scrollable area which can be very large,
 * as the area's width/height are stored as 64-bit numbers. This class
 * overcomes the scrollbar's 32-bit limit.
 * 
 * @author andras
 */
public abstract class LargeScrollableCanvas extends Canvas {
	
	private long virtualWidth, virtualHeight; // 64-bit size of the "virtual canvas"
	private long viewX, viewY; // 64-bit coordinates of top-left corner of the viewport
	private int hShift, vShift; // used for scrollbar mapping 
	private Rectangle viewportRect; // the scrolled area within the canvas

	public LargeScrollableCanvas(Composite parent, int style) {
		super(parent, style | SWT.H_SCROLL | SWT.V_SCROLL);
		init();
	}

	private void init() {
		getHorizontalBar().setIncrement(10);
		getVerticalBar().setIncrement(10);
		
		// set an initial viewport rectangle to prevent null pointer exceptions;
		// clients are expected to set a proper viewport rectangle inside Resize events.
		setViewportRectangle(new Rectangle(0,0,1,1));
		
		//
		// Note: there is no need to reconfigure the scrollbars on Resize events,
		// because scrollbar state only depends on virtualWidth/Height and viewportRect,
		// and both of them have explicit setter methods.
		//
		// Rather, users of this class are expected to hook on resize events and
		// adjust viewportRect from there.
		//

		getHorizontalBar().addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				horizontalBarChanged();
			}
		});
		getVerticalBar().addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				verticalBarChanged();
			}
		});
	}

	public void setVirtualSize(long width, long height) {
		this.virtualWidth = width;
		this.virtualHeight = height;
		configureScrollbars();
	}

	public long getVirtualHeight() {
		return virtualHeight;
	}

	public void setVirtualHeight(long height) {
		this.virtualHeight = height;
		configureScrollbars();
	}

	public long getVirtualWidth() {
		return virtualWidth;
	}

	public void setVirtualWidth(long width) {
		this.virtualWidth = width;
		configureScrollbars();
	}

	/**
	 * The virtual coordinate of the left of the viewport (clientArea minus insets).
	 */
	public long getViewportLeft() {
		return viewX;
	}

	public long getViewportRight() {
		return viewX + getViewportWidth();
	}

	/**
	 * The virtual coordinate of the top of the viewport (clientArea minus insets).
	 */
	public long getViewportTop() {
		return viewY;
	}

	public long getViewportBottom() {
		return viewY + getViewportHeight();
	}

	/**
	 * Sets viewportLeft.
	 */
	public void scrollHorizontalTo(long x) {
		this.viewX = clipX(x);
		adjustScrollbars();
		redraw();
	}

	/**
	 * Sets viewportTop.
	 */
	public void scrollVerticalTo(long y) {
		this.viewY = clipY(y);
		adjustScrollbars();
		redraw();
	}
	
	public void scrollHorizontalToRange(long x1, long x2) {
		Assert.isTrue(x2 >= x1);
		long dx = x2 - x1;
		
		if (dx > getViewportWidth())
			scrollHorizontalTo((x2 + x1 - getViewportWidth()) / 2);
		else if (x1 < viewX)
			scrollHorizontalTo(x1);
		else if (x2 > viewX + getViewportWidth())
			scrollHorizontalTo(x2 - getViewportWidth());
	}
	
	public void scrollVerticalToRange(long y1, long y2) {
		Assert.isTrue(y2 >= y1);
		long dy = y2 - y1;
		
		if (dy > getViewportHeight())
			scrollVerticalTo((y2 + y1 - getViewportHeight()) / 2);
		else if (y1 < viewY)
			scrollVerticalTo(y1);
		else if (y2 > viewY + getVirtualHeight())
			scrollVerticalTo(y2 - getViewportHeight());
	}
	
	private long clipX(long x) {
		return clip(x, virtualWidth-getViewportWidth());
	}
	
	private long clipY(long y) {
		return clip(y, virtualHeight-getViewportHeight());
	}

	/**
	 * Utility function to clip x to (0, upperBound). If upperBound<0, 0 is returned.
	 */
	private static long clip(long x, long upperBound) {
		if (x > upperBound) 
			x = upperBound;  // if negative, we'll fix it below
		if (x < 0) 
			x = 0;
		return x;
	}

	/**
	 * Returns the viewport (clientArea minus insets) in canvas coordinates. 
	 */
	public Rectangle getViewportRectangle() {
		return new Rectangle(viewportRect.x, viewportRect.y, viewportRect.width, viewportRect.height);
	}

	public void setViewportRectangle(Rectangle r) {
		viewportRect = new Rectangle(r.x, r.y, r.width, r.height);
		configureScrollbars();
	}

	public int virtualToCanvasX(long x) {
		return (int)(x - getViewportLeft() + viewportRect.x);
	}
	
	public int virtualToCanvasY(long y) {
		return (int)(y - getViewportTop() + viewportRect.y);
	}

	public Rectangle virtualToCanvasRect(LargeRect r) {
		return new Rectangle(virtualToCanvasX(r.x), virtualToCanvasY(r.y), (int)r.width, (int)r.height);
	}

	public long canvasToVirtualX(int x) {
		return x + getViewportLeft() - viewportRect.x;
	}
	
	public long canvasToVirtualY(int y) {
		return y + getViewportTop() - viewportRect.y;
	}

	public LargeRect canvasToVirtualRect(Rectangle r) {
		return new LargeRect(r.x - viewportRect.x + getViewportLeft(), r.y - viewportRect.y + getViewportTop(), r.width, r.height);
	}

	/**
	 * Returns the width of the viewport (clientArea minus insets).
	 */
	public int getViewportWidth() {
		return viewportRect.width;
	}

	/**
	 * Returns the height of the viewport (clientArea minus insets).
	 */
	public int getViewportHeight() {
		return viewportRect.height;
	}

	private void horizontalBarChanged() {
		viewX = clipX(((long)getHorizontalBar().getSelection()) << hShift);
		redraw();
	}

	private void verticalBarChanged() {
		viewY = clipY(((long)getVerticalBar().getSelection()) << vShift);
		redraw();
	}
	
	/**
	 * Should be called when viewport or virtual size changes.
	 */
	protected void configureScrollbars() {
		viewX = clipX(viewX);
		viewY = clipY(viewY);
		hShift = configureScrollbar(getHorizontalBar(), virtualWidth, viewX, getViewportWidth());
		vShift = configureScrollbar(getVerticalBar(), virtualHeight, viewY, getViewportHeight());
	}

	private int configureScrollbar(ScrollBar sb, long virtualSize, long virtualPos, int widgetSize) {
		if (widgetSize >= virtualSize) {
			// hide scrollbar when not needed
			sb.setVisible(false);
			return 0;
		}
		else {
			// show scrollbar
			sb.setVisible(true);
			
			// count how many bits we need to shift to fit into 32 bits
			int shift = 0;
			while (virtualSize>>shift > Integer.MAX_VALUE) 
				shift++;

			int newMax = (int)(virtualSize>>shift);
			int newThumb = (int)(widgetSize >> shift);
			int newSel = (int)(virtualPos >> shift);
			int newPageIncr = (int)((widgetSize>>shift > 0) ? widgetSize>>shift : 1);
				
			sb.setMinimum(0);
			sb.setMaximum(newMax);
			sb.setThumb(newThumb);
			sb.setSelection(newSel);
			sb.setPageIncrement(newPageIncr);
			return shift;
		}
	}

	private void adjustScrollbars() {
		getHorizontalBar().setSelection((int)(viewX >> hShift));
		getVerticalBar().setSelection((int)(viewY >> vShift));
	}
}
