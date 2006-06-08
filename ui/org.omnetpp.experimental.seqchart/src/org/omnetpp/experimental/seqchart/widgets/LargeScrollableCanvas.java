package org.omnetpp.experimental.seqchart.widgets;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ControlAdapter;
import org.eclipse.swt.events.ControlEvent;
import org.eclipse.swt.events.PaintEvent;
import org.eclipse.swt.events.PaintListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.ScrollBar;

/**
 * An improved canvas class that supports: (1) displaying part of a very large 
 * client area (h,w>MAXINT) with scrollbar, and (2) caching of (part of) the drawing
 * in off-screen image buffers for performance improvement. Use of both features are
 * optional. 
 * 
 * Other utility functinality: dragging the area with the mouse ("hand cursor");
 * rubberbanding.
 * 
 * @author andras
 */
public class LargeScrollableCanvas extends Canvas {
	
	private long virtualWidth, virtualHeight; // 64-bit size of the "virtual canvas"
	private long viewX, viewY; // 64-bit coordinates of top-left corner of the viewport
	private int hShift, vShift; // used for scrollbar mapping 
	
	public LargeScrollableCanvas(Composite parent, int style) {
		super(parent, style | SWT.H_SCROLL | SWT.V_SCROLL);
		init();
	}

	private void init() {
		getHorizontalBar().setIncrement(20);
		getVerticalBar().setIncrement(20);
		
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
		addPaintListener(new PaintListener() {
			public void paintControl(PaintEvent e) {
				paint(e.gc);
			}
		});
		addControlListener(new ControlAdapter() {
			public void controlResized(ControlEvent e) {
				configureScrollbars();
			}
		});
	}

	public void setVirtualSize(long width, long height) {
		this.virtualWidth = width;
		this.virtualHeight = height;
		configureScrollbars();
		redraw();
	}

	public long getVirtualHeight() {
		return virtualHeight;
	}

	public void setVirtualHeight(long height) {
		this.virtualHeight = height;
		configureScrollbars();
		redraw();
	}

	public long getVirtualWidth() {
		return virtualWidth;
	}

	public void setVirtualWidth(long width) {
		this.virtualWidth = width;
		configureScrollbars();
		redraw();
	}

	public long getViewportLeft() {
		return viewX;
	}

	/**
	 * Sets viewportLeft.
	 */
	public void scrollHorizontalTo(long x) {
		this.viewX = x;
		adjustScrollbars();
		redraw();
	}

	public long getViewportTop() {
		return viewY;
	}

	/**
	 * Sets viewportTop.
	 */
	public void scrollVerticalTo(long y) {
		this.viewY = y;
		adjustScrollbars();
		redraw();
	}

	/**
	 * Returns the widget width.
	 */
	public int getWidth() {
		return getSize().x;
	}

	/**
	 * Returns the widget height.
	 */
	public int getHeight() {
		return getSize().y;
	}
	
	protected void horizontalBarChanged() {
		viewX = ((long)getHorizontalBar().getSelection()) << hShift;
		redraw();
	}

	protected void verticalBarChanged() {
		viewY = ((long)getVerticalBar().getSelection()) << vShift;
		redraw();
	}
	
	/**
	 * Override this to do your own drawing. 
	 */
	protected void paint(GC graphics) {
		graphics.drawText("x="+viewX+" y="+viewY, 0, 0);
	}

	private void configureScrollbars() {
		hShift = configureScrollbar(getHorizontalBar(), virtualWidth, viewX, getSize().x);
		vShift = configureScrollbar(getVerticalBar(), virtualHeight, viewY, getSize().y);
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
