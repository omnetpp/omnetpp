package org.omnetpp.scave2.editors.ui;

import java.util.ArrayList;
import java.util.Arrays;

import org.eclipse.core.runtime.Assert;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseListener;
import org.eclipse.swt.events.MouseMoveListener;
import org.eclipse.swt.events.PaintEvent;
import org.eclipse.swt.events.PaintListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;

import sun.security.krb5.internal.crypto.t;

/**
 * Holds a grid of equal-sized controls that can be selected and 
 * reordered with the mouse.
 * 
 * @author andras
 */
public class LiveTable extends Composite {
	private static final Color SELECTBORDER_COLOR = new Color(null,255,0,0);
	private static final Color INSERTMARK_COLOR = new Color(null,0,0,0);
 	
	private ArrayList<Control> orderedChildren = new ArrayList<Control>();
	private ArrayList<Control> selection = new ArrayList<Control>();
	private Control insertMark = null;
	
	public LiveTable(Composite parent, int style) {
		super(parent, style);

		GridLayout gridLayout = new GridLayout(4, true);
		gridLayout.horizontalSpacing = 5;
		gridLayout.verticalSpacing = 5;
		setLayout(gridLayout);
		
		addPaintListener(new PaintListener() {
			public void paintControl(PaintEvent e) {
				redrawCanvas(e.gc);
			}
		});
	}
	
	public void setNumColumns(int numColums) {
		GridLayout layout = (GridLayout) getLayout();
		layout.numColumns = numColums;
		redraw();
	}

	public int getNumColumns() {
		GridLayout layout = (GridLayout) getLayout();
		return layout.numColumns;
	}

	/**
	 * Overridden so that we can reorder the children. 
	 */
	@Override
	public Control[] getChildren() {
		// synchronize orderedChildren to actual children; remove disposed ones, add new ones
		Control[] children = super.getChildren();
		for (Control child : orderedChildren)
			if (child.isDisposed())
				orderedChildren.remove(child);
		for (Control child : children)
			if (!orderedChildren.contains(child))
				orderedChildren.add(child);
		return orderedChildren.toArray(new Control[orderedChildren.size()]);
	}
	
	/**
	 * Adds the necessary mouse listeners for dragging a child.
	 */
	public void configureChild(Control control) {
		control.addMouseMoveListener(new MouseMoveListener() {
			public void mouseMove(MouseEvent e) {
				if ((e.stateMask & SWT.BUTTON_MASK) != 0) { // dragging
					Control sender = (Control)e.widget;
					Control c = findControlUnder(sender.toDisplay(e.x,e.y));
					if (c!=null)
						setInsertMark(c);
				}
			}
		});
		control.addMouseListener(new MouseListener() {
			public void mouseDoubleClick(MouseEvent e) {}
			public void mouseDown(MouseEvent e) {
				if (selection.size()<=1 && (e.stateMask & SWT.CTRL)==0)
					setSelection((Control)e.widget);
			}
			public void mouseUp(MouseEvent e) {
				Control sender = (Control)e.widget;
				Control target = findControlUnder(sender.toDisplay(e.x,e.y));
				if (sender!=target && target!=null) {
					// this is a move
					if (selection.isEmpty())
						moveChild(sender, target);
					else
						moveSelectionTo(target);
				}
				if (sender==target) {
					// this is a selection
					if ((e.stateMask & SWT.CTRL)!=0) {
						if (!selection.contains(e.widget))
							select((Control)e.widget);
						else
							deselect((Control)e.widget);
					}
					else {
						setSelection((Control)e.widget);
					}
				}
				setInsertMark(null);
			}
		});
	}

	public void setInsertMark(Control item) {
		insertMark = item;
		redraw();
	}

	private void redrawCanvas(GC gc) {
		for (Control child : selection) {
			Rectangle bounds = child.getBounds();
			bounds.x -= 2;
			bounds.y -= 2;
			bounds.width += 3;
			bounds.height += 3;
			gc.setForeground(SELECTBORDER_COLOR);
			gc.drawRectangle(bounds);
		}
		if (insertMark!=null) {
			Rectangle bounds = insertMark.getBounds();
			bounds.x -= 2;
			bounds.y -= 2;
			bounds.width += 3;
			bounds.height += 3;
			gc.setForeground(INSERTMARK_COLOR);
			gc.drawRectangle(bounds);
		}
	}

	
	public void moveChild(Control item, Control atItem) {
		Assert.isTrue(item.getParent()==this);
		Assert.isTrue(atItem.getParent()==this);
		if (item==atItem)
			return;

		int index = orderedChildren.indexOf(atItem);
		orderedChildren.remove(item);
		orderedChildren.add(index, item);
		layout();
	}

	protected void moveSelectionTo(Control target) {
		int index = orderedChildren.indexOf(target);
		for (Control sel : selection)
			orderedChildren.remove(sel);
		if (index>orderedChildren.size())
			index = orderedChildren.size();
		for (Control sel : selection)
			orderedChildren.add(index++, sel);
		layout();
	}
	
	private void addToSelection(Control control) {
		Assert.isTrue(control.getParent()==this);
		if (control!=null && !selection.contains(control))
			selection.add(control);
	}

	public void select(Control control) {
		addToSelection(control);
		redraw();
	}

	public void deselect(Control control) {
		if (selection.contains(control))
			selection.remove(control);
		redraw();
	}
	
	public void select(Control[] controls) {
		for (Control control : controls)
			addToSelection(control);
		redraw();
	}

	public void setSelection(Control control) {
		selection.clear();
		select(control);
	}

	public void setSelection(Control[] controls) {
		selection.clear();
		select(controls);
	}
	
	public void selectAll() {
		selection.clear();
		for (Control child: getChildren())
			selection.add(child);
	}

	public Control[] getSelection() {
		return selection.toArray(new Control[selection.size()]);
	}
	
	private Control findControlUnder(Point p) {
		for (Control child : getChildren()) {
			Point topLeft = child.toDisplay(0,0);
			Rectangle bounds = child.getBounds();
			bounds.x = topLeft.x;
			bounds.y = topLeft.y;
			if (bounds.contains(p)) {
				return child;
			}
		}
		return null;
	}
}
