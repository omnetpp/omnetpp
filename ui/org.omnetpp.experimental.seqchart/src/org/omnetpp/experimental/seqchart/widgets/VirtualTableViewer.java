package org.omnetpp.experimental.seqchart.widgets;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.viewers.ContentViewer;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.KeyEvent;
import org.eclipse.swt.events.KeyListener;
import org.eclipse.swt.events.PaintEvent;
import org.eclipse.swt.events.PaintListener;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableItem;

public class VirtualTableViewer extends ContentViewer {
	/**
	 * The underlying table widget.
	 */
	protected Table table;
	
	/**
	 * This is an element close to the visible area.
	 */
	protected Object fixPointElement;
	
	/**
	 * The index of the fix point element in the real table.
	 */
	protected int fixPointTableIndex;

	/**
	 * A flag used to avoid recursively call into fix table.
	 */
	protected boolean alreadyInFixTable;

	/**
	 * The top index of the read table when the table was last fixed.
	 */
	protected int oldTopIndex;

	protected boolean followSelection = true;
	
	public VirtualTableViewer(Table table) {
		this.table = table;
		
		table.addListener(SWT.SetData, new Listener() {
			public void handleEvent(Event e) {
				int indexDelta = e.index - fixPointTableIndex;
				if (Math.abs(indexDelta) <= getMaximumLinearMove()) {
					Object element = getVirtualTableContentProvider().getNeighbourElement(fixPointElement, e.index - fixPointTableIndex);
					getVirtualTableItemProvider().fillTableItem((TableItem)e.item, element);
				}
			}
		});
		table.addPaintListener(new PaintListener() {
			public void paintControl(PaintEvent e) {
				if (processTablePositionChange())
					fixTablePosition();
			}
		});
		table.addKeyListener(new KeyListener() {
			public void keyPressed(KeyEvent e) {
			}

			public void keyReleased(KeyEvent e) {
				if (e.keyCode == SWT.F5) {
					VirtualTableViewer.this.table.setTopIndex(fixPointTableIndex);
					redrawTable();
				}
			}
		});
	}

	@Override
	public Control getControl() {
		return table;
	}

	@Override
	public ISelection getSelection() {
		return new VirtualTableSelection(getInput(), getSelectionElements());
	}

	@Override
	public void refresh() {
	}

	@Override
	public void setSelection(ISelection selection, boolean reveal) {
		System.out.println("VirtualTableViewer got selection: " + selection);
		
		if (followSelection) {
			// act as a view: display the element which comes in the selection, 
			// or display nothing if the selection is not a virtual table selection. 
			if (!(selection instanceof IVirtualTableSelection))
				setInput(null);
			else {
				IVirtualTableSelection virtualTableSelection = (IVirtualTableSelection)selection;
				if (virtualTableSelection.getInput() != getInput())
					setInput(virtualTableSelection.getInput());
	
				//FIXME only notify if selection actually changed
				setSelectionElements(virtualTableSelection.getElements());
			}
		}
		else {
			// act as an editor: stick to the input, and ignore selections 
			// that are about something else 
			if (selection instanceof IVirtualTableSelection) {
				IVirtualTableSelection virtualTableSelection = (IVirtualTableSelection)selection;
				
				if (virtualTableSelection.getInput() == getInput()) {
					//FIXME only notify if selection actually changed
					setSelectionElements(virtualTableSelection.getElements());
				}
			}
		}
	}
	
	@Override
	public void setInput(Object input) {
		super.setInput(input);
		ensureFixPoint();
	}

	/**
	 * Returns the current selection.
	 */
	public Object getSelectionElement() {
		return null;
// FIXME: this might cost quite a bit if the selection is far away from the fixpoint		return getElementAt(table.getSelectionIndex());
	}

	/**
	 * Returns the current selection.
	 */
	public List getSelectionElements() {
		int[] indices = table.getSelectionIndices();
		ArrayList<Object> list = new ArrayList<Object>();
		for (int index : indices)
			; // FIXME: this might cost quite a bit list.add(getElementAt(index));
		return list;
	}

	/**
	 * Selects the given elements, and goes to the first one.
	 */
	public void setSelectionElements(List elements) {
		if (elements.size() > 0)
			gotoElement(elements.get(0));

		/* TODO: we should delay index calculation
		int visibleElementCount = getVisibleElementCount();
		int[] indices = new int[elements.size()];

		for (int i = 0; i < elements.size(); i++) {
			// TODO: what if the element is far away?
			int distance = (int)getVirtualTableContentProvider().getDistanceToElement(fixPointElement, elements.get(i), visibleElementCount);

			if (distance < visibleElementCount)
				indices[i] = fixPointTableIndex + distance;
		}

		table.setSelection(indices);
		*/
	}

	/**
	 * See setFollowSelection().
	 */
	public boolean getFollowSelection() {
		return followSelection;
	}

	/**
	 * Sets whether this widget should always switch to the element which comes in
	 * the selection (=true), or stick to the input set with setInput() (=false).
	 * The proper setting typically depends on whether the widget is used in an
	 * editor (false) or in a view (true).
	 *
	 * Default is true.
	 */
	public void setFollowSelection(boolean followSelection) {
		this.followSelection = followSelection;
	}

	/**
	 * Position the table selection to the given element or nearby.
	 */
	public void gotoElement(Object element) {
		if (element != null) {
			int topIndex = getTopVisibleElementIndex();

			changeTablePosition(topIndex, topIndex, element, topIndex);
			table.setSelection(topIndex);

			fixTablePosition();
		}
	}

	protected IVirtualTableContentProvider getVirtualTableContentProvider() {
		return (IVirtualTableContentProvider)getContentProvider();
	}
	
	protected IVirtualTableItemProvider getVirtualTableItemProvider() {
		return (IVirtualTableItemProvider)getLabelProvider();
	}
	
	protected void ensureFixPoint() {
		if (fixPointElement == null) {
			setTableElementCount();
			relocateFixPoint(getVirtualTableContentProvider().getFirstElement(), 0);
		}
	}

	protected void setTableElementCount() {
		long elementCount = getVirtualTableContentProvider().getApproximateNumberOfElements();
		System.out.println("Approximate virtual table size: " + elementCount);
		elementCount = elementCount < 10000 ? elementCount : 10000;
		elementCount = Math.max(elementCount, getElementCountLowerBound());
		System.out.println("Real table size: " + elementCount);
		table.setItemCount((int)elementCount);
	}

	protected long getElementCountLowerBound() {
		Object element = getVirtualTableContentProvider().getFirstElement();

		if (element == null)
			return 0;
		else
			return getVirtualTableContentProvider().getDistanceToLastElement(element, getMaximumLinearMove() * 2);
	}
	
	protected void relocateFixPoint(Object element, int index) {
		fixPointElement = element;
		fixPointTableIndex = index;
	}

	protected boolean processTablePositionChange() {
		int currentTopIndex = getTopVisibleElementIndex();
		
		if (alreadyInFixTable || currentTopIndex == oldTopIndex)
			return false;

		try {
			alreadyInFixTable = true;
	
			int tableElementCount = getTableElementCount();
			int currentBottomIndex = getBottomVisibleElementIndex();
			int visibleElementCount = getVisibleElementCount();
			int newTopIndex = -1;
			int newFixPointTableIndex = -1;
			Object newFixPointElement = null;
			IVirtualTableContentProvider virtualTableContentProvider = getVirtualTableContentProvider();
			int indexDelta = currentTopIndex - oldTopIndex;
			int maximumLinearMove = getMaximumLinearMove();
			boolean forward = indexDelta > 0;
	
			// step 1. set fix point and top index to reflect the new virtual table position
			System.out.println("Index delta: " + indexDelta);
			
			if (Math.abs(indexDelta) < maximumLinearMove) {
				// scroll virtual table linearly according to the small scroll in the real table and move real table back to its old position
				newTopIndex = oldTopIndex;
				newFixPointElement = fixPointElement;
				newFixPointTableIndex = fixPointTableIndex - indexDelta;
			}
			else {
				// jump to approximate element in virtual table based on real the table position and leave real table position as it is
				double percentage = Math.max(0, Math.min(1, (double)currentTopIndex / (tableElementCount - visibleElementCount)));
				newTopIndex = currentTopIndex;
				newFixPointElement = virtualTableContentProvider.getApproximateElementAt(percentage);
				newFixPointTableIndex = forward ? currentTopIndex + (int)virtualTableContentProvider.getDistanceToFirstElement(newFixPointElement, currentBottomIndex - currentTopIndex) : currentTopIndex;
			}
	
			// step 2. move table and selection and set new fix point
			changeTablePosition(currentTopIndex, newTopIndex, newFixPointElement, newFixPointTableIndex);
			
			return true;
		}
		finally {
			alreadyInFixTable = false;
		}
	}

	protected void fixTablePosition() {
		if (alreadyInFixTable)
			return;
		
		try {
			alreadyInFixTable = true;
	
			int tableElementCount = getTableElementCount();
			int visibleElementCount = getVisibleElementCount();
			int topIndex = getTopVisibleElementIndex();
			IVirtualTableContentProvider virtualTableContentProvider = getVirtualTableContentProvider();
	
			// step 1. if we are near the beginning or the end make sure that there are as many real table lines as virtual table lines below and/or above the table
			int reservedElementCount = getMaximumLinearMove() + 1;
			int	 firstElementDistance = (int)virtualTableContentProvider.getDistanceToFirstElement(getTopVisibleElement(), reservedElementCount);
			int lastElementDistance = (int)virtualTableContentProvider.getDistanceToLastElement(getBottomVisibleElement(), reservedElementCount);
			boolean nearFirstElement = firstElementDistance < reservedElementCount;
			boolean nearLastElement = lastElementDistance < reservedElementCount;
	
			if (nearFirstElement && nearLastElement) {
				changeTablePosition(topIndex, firstElementDistance, virtualTableContentProvider.getFirstElement(), 0);
				table.setItemCount(1 + (int)virtualTableContentProvider.getDistanceToLastElement(virtualTableContentProvider.getFirstElement(), Long.MAX_VALUE));
			}
			else if (nearFirstElement)
				changeTablePosition(topIndex, firstElementDistance, virtualTableContentProvider.getFirstElement(), 0);
			else if (nearLastElement)
				changeTablePosition(topIndex, tableElementCount - visibleElementCount - lastElementDistance, virtualTableContentProvider.getLastElement(), tableElementCount - 1);
			else {
				double percentage = virtualTableContentProvider.getApproximatePercentageForElement(getTopVisibleElement());
				int modifiedTopIndex = reservedElementCount + (int)((tableElementCount - visibleElementCount - 2 * reservedElementCount) * percentage);
				changeTablePosition(topIndex, modifiedTopIndex, fixPointElement, fixPointTableIndex - topIndex + modifiedTopIndex);
			}
	
			// step 2. relocated fix point to the top of the visible area, so that it's close to what is visible
			relocateFixPoint(getTopVisibleElement(), getTopVisibleElementIndex());
			
			// step 3. remember the last set top index for the next call to fixTable
			oldTopIndex = getTopVisibleElementIndex();
			
			// step 4. assert invariants
			Assert.isTrue(virtualTableContentProvider.getDistanceToFirstElement(getTopVisibleElement(), reservedElementCount) <= getTopVisibleElementIndex(), "Not enough lines above top element in real table");
			Assert.isTrue(virtualTableContentProvider.getDistanceToLastElement(getBottomVisibleElement(), reservedElementCount) <= tableElementCount - getBottomVisibleElementIndex(), "Not enough lines below bottom element in real table");
	
			redrawTable();
			System.out.println("Top element after fixTable: " + getTopVisibleElement() + " new top index: " + getTopVisibleElementIndex());
		}
		finally {
			alreadyInFixTable = false;
		}
	}
	
	protected void changeTablePosition(int oldTopIndex, int newTopIndex, Object fixPointElement, int fixPointTableIndex) {		
		int deltaTopIndex = newTopIndex - oldTopIndex;
		relocateFixPoint(fixPointElement, fixPointTableIndex);

		if (oldTopIndex != newTopIndex) {
			int selectedIndex = table.getSelectionIndex();

			if (selectedIndex != -1)
				table.setSelection(selectedIndex + deltaTopIndex);

			table.setTopIndex(newTopIndex);
		}
	}

	protected void redrawTable() {
		table.clearAll();
		table.redraw();
	}

	protected int getTableElementCount() {
		return table.getItemCount();
	}
	
	protected int getTopVisibleElementIndex() {
		return table.getTopIndex();
	}
	
	protected int getBottomVisibleElementIndex() {
		return Math.min(table.getItemCount() - 1, table.getTopIndex() + getVisibleElementCount() - 1);
	}

	private Object getElementAtIndex(int index) {
		int elementIndexDelta = index - fixPointTableIndex;
		return getVirtualTableContentProvider().getNeighbourElement(fixPointElement, elementIndexDelta);
	}
	
	/**
	 * Returns the top visible element.
	 */
	protected Object getTopVisibleElement() {
		Object element = getElementAtIndex(getTopVisibleElementIndex());
		
		if (element == null)
			throw new IllegalStateException();

		return element;
	}

	/**
	 * Returns the bottom visible element even if it is not totally visible.
	 */
	protected Object getBottomVisibleElement() {
		Object element = getElementAtIndex(getBottomVisibleElementIndex());
		
		if (element == null)
			throw new IllegalStateException();

		return element;
	}
	
	/**
	 * Returns the maximum distance in the real table that will be considered a linear move in the virtual table.
	 */
	protected int getMaximumLinearMove() {
		return getVisibleElementCount() + 1;
	}

	/**
	 * Returns the number of visible elements including the one which is not totally visible.
	 */
	protected int getVisibleElementCount() {
		int itemHeight = table.getItemHeight();
		int headerHeight = table.getHeaderHeight();
		// height seems to be offset by approximately one item (itemHeight: 14, headerHeight: 20, height: 40 for a less than one visible row table)
		int height = table.getSize().y - itemHeight;
		
		return Math.max(0, (int)Math.ceil((double)(height - headerHeight) / itemHeight));
	}
}
