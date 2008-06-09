package org.omnetpp.scave.editors.ui;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Iterator;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.ListenerList;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.ISelectionProvider;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseListener;
import org.eclipse.swt.events.MouseMoveListener;
import org.eclipse.swt.events.MouseTrackAdapter;
import org.eclipse.swt.events.PaintEvent;
import org.eclipse.swt.events.PaintListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;

/**
 * Holds a several controls that can be selected and 
 * reordered with the mouse. Typically the user wants to
 * set a GridLayout on the control, but this is not mandatory.
 *
 * <pre>
 * GridLayout gridLayout = new GridLayout(4, true);
 * gridLayout.horizontalSpacing = gridLayout.verticalSpacing = 5;
 * liveTable.setLayout(gridLayout);
 * </pre>
 * 
 * @author andras
 */
//XXX move to common plug-in
public class LiveTable extends Composite  implements ISelectionProvider {
	private static final Color SELECTBORDER_COLOR = new Color(null,255,0,0);
	private static final Color INSERTMARK_COLOR = new Color(null,0,0,0);
 	
	private ArrayList<Control> orderedChildren = new ArrayList<Control>();
	private ArrayList<Control> selection = new ArrayList<Control>();
	private ListenerList selectionChangedListeners = new ListenerList();
	private ListenerList childOrderChangedListeners = new ListenerList();
	private Control insertMark = null;
	
	/**
	 * Constructor.
	 */
	public LiveTable(Composite parent, int style) {
		super(parent, style);

		addPaintListener(new PaintListener() {
			public void paintControl(PaintEvent e) {
				redrawCanvas(e.gc);
			}
		});
	}
	
	/**
	 * Overridden so that we can reorder the children. 
	 */
	@Override
	public Control[] getChildren() {
		// synchronize orderedChildren to actual children; remove disposed ones, add new ones
		Control[] children = super.getChildren();
		for (Iterator<Control> childIterator = orderedChildren.iterator(); childIterator.hasNext(); )
			if (childIterator.next().isDisposed())
				childIterator.remove();
		for (Control child : children)
			if (!orderedChildren.contains(child))
				orderedChildren.add(child);
		return orderedChildren.toArray(new Control[orderedChildren.size()]);
	}
	
	public void setChildOrder(List<? extends Control> childOrder) {
		// TODO check children
		orderedChildren.clear();
		orderedChildren.addAll(childOrder);
	}
	
	/**
	 * Adds the necessary mouse listeners for dragging and selecting children.
	 */
	public void configureChild(Control control) {
		control.addMouseTrackListener(new MouseTrackAdapter() {
			@Override public void mouseEnter(MouseEvent e) {
				if (e.widget instanceof Control)
					((Control)e.widget).setFocus();
			}
		});
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
				if (selection.size()<=1 && (e.stateMask & SWT.MOD1)==0)
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
					if ((e.stateMask & SWT.MOD1)!=0) {
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

	/**
	 * Returns the child control under the given mouse position, or null.
	 * Expects screen coordinates. 
	 */
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

	/**
	 * Displays the insert mark around the given item.
	 * A null value means no insert mark.
	 */
	public void setInsertMark(Control item) {
		Assert.isTrue(item==null || item.getParent()==this);
		insertMark = item;
		redraw();
	}

	/**
	 * Redraws the canvas, with selection marks and insert mark.
	 */
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

	/**
	 * Changes the order of children so that "item" is moved to 
	 * where the "target" child currently is.
	 */
	public void moveChild(Control item, Control atItem) {
		Assert.isTrue(item.getParent()==this);
		Assert.isTrue(atItem.getParent()==this);
		if (item==atItem)
			return;
		
		moveTo(Collections.singletonList(item), atItem);
		layout();
	}

	/**
	 * Changes the order of children so that the selected items
	 * are moved to where the "target" child currently is.
	 */
	protected void moveSelectionTo(Control target) {
		Assert.isTrue(target.getParent()==this);
		moveTo(selection, target);
		layout();
	}
	
	protected void moveTo(List<Control> controls, Control target) {
		int index = orderedChildren.indexOf(target);
		for (Control control : controls)
			orderedChildren.remove(control);
		if (index>orderedChildren.size())
			index = orderedChildren.size();
		for (Control control : controls)
			orderedChildren.add(index++, control);
		
		fireChildOrderChanged();
	}
	
	private void addToSelection(Control control) {
		Assert.isTrue(control == null || control.getParent()==this);
		if (control!=null && !selection.contains(control))
			selection.add(control);
	}

	/**
	 * Extends the selection with the given control.
	 */
	public void select(Control control) {
		addToSelection(control);
		redraw();
		fireSelectionChanged();
	}

	/**
	 * Removes the given control from the selection.
	 */
	public void deselect(Control control) {
		if (selection.contains(control))
			selection.remove(control);
		redraw();
		fireSelectionChanged();
	}
	
	/**
	 * Extends the selection with the given controls.
	 */
	public void select(Control[] controls) {
		for (Control control : controls)
			addToSelection(control);
		redraw();
		fireSelectionChanged();
	}

	/**
	 * Sets the selection to the given control.
	 */
	public void setSelection(Control control) {
		selection.clear();
		select(control);
	}

	/**
	 * Sets the selection to the given controls.
	 */
	public void setSelection(Control[] controls) {
		selection.clear();
		select(controls);
	}
	
	/**
	 * Selects all controls.
	 */
	public void selectAll() {
		selection.clear();
		for (Control child: getChildren())
			selection.add(child);
	}

	/**
	 * Returns the selection.
	 */
	public ISelection getSelection() {
		return new StructuredSelection(selection);
	}

    /**
     * Sets the current selection for this selection provider.
     */
	public void setSelection(ISelection selection) {
		List<Control> controls = new ArrayList<Control>();
		if (selection instanceof IStructuredSelection) {
			for (Iterator<?> iterator = ((IStructuredSelection)selection).iterator(); iterator.hasNext();) {
				Object element = iterator.next();
				if (element instanceof Control)
					controls.add((Control)element);
			}
		}
		setSelection(controls.toArray(new Control[controls.size()]));
	}

	/**
     * Adds a listener for selection changes in this selection provider.
	 */
	public void addSelectionChangedListener(ISelectionChangedListener listener) {
		selectionChangedListeners.add(listener);
	}

	/**
     * Removes the given selection change listener from this selection provider.
	 */
	public void removeSelectionChangedListener(ISelectionChangedListener listener) {
		selectionChangedListeners.remove(listener);
	}
	
	/**
	 * Notifies the selection change listeners about a selection change event.
	 */
	protected void fireSelectionChanged() {
		SelectionChangedEvent event = new SelectionChangedEvent(this, getSelection());
		for (Object listener : selectionChangedListeners.getListeners())
			((ISelectionChangedListener)listener).selectionChanged(event);
	}
	
	public void addChildOrderChangedListener(IChildOrderChangedListener listener) {
		childOrderChangedListeners.add(listener);
	}
	
	public void removeChildOrderChangedListener(IChildOrderChangedListener listener) {
		childOrderChangedListeners.remove(listener);
	}
	
	protected void fireChildOrderChanged() {
		for (Object listener : childOrderChangedListeners.getListeners())
			((IChildOrderChangedListener)listener).childOrderChanged(this);
	}
	
	public static interface IChildOrderChangedListener {
		void childOrderChanged(LiveTable table);
	}
}
