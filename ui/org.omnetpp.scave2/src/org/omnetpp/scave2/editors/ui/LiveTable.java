package org.omnetpp.scave2.editors.ui;

import java.util.ArrayList;

import org.eclipse.core.runtime.Assert;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseListener;
import org.eclipse.swt.events.MouseMoveListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;

/**
 * Holds a grid of equal-sized controls that can be selected and 
 * reordered with the mouse.
 * 
 * @author andras
 */
public class LiveTable extends Composite {

	private int itemWidth = 200;
	private int itemHeight = 150;
	private Color BORDER_COLOR = new Color(null,255,255,255);
	private Color SELECTBORDER_COLOR = new Color(null,255,0,0);
	private Color INSERTMARK_COLOR = new Color(null,0,0,0);
	
	private ArrayList<Control> selection = new ArrayList<Control>();
	
	public LiveTable(Composite parent, int style) {
		super(parent, style);

		GridLayout gridLayout = new GridLayout(4, true);
		gridLayout.horizontalSpacing = 1;
		gridLayout.verticalSpacing = 1;
		setLayout(gridLayout);
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
	 * Adds a control to the table. The control can be created with any parent,
	 * because this method will explicitly overwrite it using setParent().
	 */
	public void addChild(Control control) {
		Canvas frame = new Canvas(this, SWT.NONE);
		frame.setBackground(BORDER_COLOR);
		frame.setLayoutData(new GridData(itemWidth,itemHeight));
		FillLayout fillLayout = new FillLayout();
		fillLayout.marginHeight = 3;
		fillLayout.marginWidth = 3;
		frame.setLayout(fillLayout);
		control.setParent(frame);
		
		control.addMouseMoveListener(new MouseMoveListener() {
			public void mouseMove(MouseEvent e) {
				if ((e.stateMask & SWT.BUTTON_MASK) != 0) {
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
				setSelection((Control)e.widget);
			}
			public void mouseUp(MouseEvent e) {
				if ((e.stateMask & SWT.BUTTON_MASK) != 0) {
					Control sender = (Control)e.widget;
					Control target = findControlUnder(sender.toDisplay(e.x,e.y));
					if (target!=null) {
						moveChild(sender, target);
						setInsertMark(null);
						setSelection(target);
					}
				}
			}
		});
	}

	public void removeChild(Control control) {
		for (Control f : getChildren()) {
			Canvas frame = (Canvas)f;
			if (frame.getChildren()[0]==control) {
				frame.dispose();
				break;
			}
		}
	}
	
	public void setInsertMark(Control item) {
		redrawSelectionMarks();
		if (item!=null) {
			Assert.isTrue(item.getParent().getParent()==this);
			Canvas frame = (Canvas) item.getParent();
			frame.setBackground(INSERTMARK_COLOR);
		}
	}

	public void moveChild(Control item, Control beforeItem) {
		Assert.isTrue(item.getParent().getParent()==this);
		Assert.isTrue(beforeItem.getParent().getParent()==this);
		if (item==beforeItem)
			return;

		Control[] frames = getChildren();
		ArrayList<Control> children = new ArrayList<Control>();;
		for (int i=0; i<frames.length; i++)
			children.add(((Canvas)frames[i]).getChildren()[0]);
		children.remove(item);
		int index = children.indexOf(beforeItem);
		children.add(index, item);

		for (int i=0; i<frames.length; i++)
			children.get(i).setParent((Canvas)frames[i]);
	}
	
	private void addToSelection(Control control) {
		Assert.isTrue(control.getParent().getParent()==this);
		if (control!=null && !selection.contains(control))
			selection.add(control);
	}

	public void select(Control control) {
		addToSelection(control);
	}

	public void select(Control[] controls) {
		for (Control control : controls)
			addToSelection(control);
		redrawSelectionMarks();
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
		for (Control frame : getChildren())
			selection.add(((Canvas)frame).getChildren()[0]);
	}

	public Control[] getSelection() {
		return selection.toArray(new Control[selection.size()]);
	}
	
	private void redrawSelectionMarks() {
		for (Control frame : getChildren()) {
			frame.setBackground(BORDER_COLOR);
		}
		for (Control control : selection) {
			Canvas frame = (Canvas) control.getParent();
			frame.setBackground(SELECTBORDER_COLOR);
		}
	}

	private Control findControlUnder(Point p) {
		for (Control child : getChildren()) {
			Point topLeft = child.toDisplay(0,0);
			Rectangle bounds = child.getBounds();
			bounds.x = topLeft.x;
			bounds.y = topLeft.y;
			if (bounds.contains(p)) {
				Canvas frame = (Canvas)child;
				return frame.getChildren()[0];
			}
		}
		return null;
	}
}
