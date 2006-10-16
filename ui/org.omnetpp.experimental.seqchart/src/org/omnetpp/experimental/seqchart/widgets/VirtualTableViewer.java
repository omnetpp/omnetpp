package org.omnetpp.experimental.seqchart.widgets;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.viewers.ContentViewer;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.SWT;
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
				if (Math.abs(indexDelta) < getVisibleItemCount() * 2) {
					Object element = getVirtualTableContentProvider().getNeighbourElement(fixPointElement, e.index - fixPointTableIndex);
					getVirtualTableItemProvider().fillTableItem((TableItem)e.item, element);
				}
			}
		});
		table.addPaintListener(new PaintListener() {
			public void paintControl(PaintEvent e) {
				fixTable();
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
// FIXME: this might cost quite a bit		return getElementAt(table.getSelectionIndex());
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

		int visibleItemCount = getVisibleItemCount();
		int[] indices = new int[elements.size()];
		for (int i = 0; i < elements.size(); i++) {
			int distance = (int)getVirtualTableContentProvider().getDistanceToElement(fixPointElement, elements.get(i), visibleItemCount);
			if (distance < visibleItemCount)
				indices[i] = fixPointTableIndex + distance;
		}

		table.setSelection(indices);
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
// TODO: resurrect
//			if (!eventLog.isIncludedInLog(event.getEventNumber()))
//				event = eventLog.getEventForEventNumber(event.getEventNumber(), MatchKind.FIRST);

			if (element != null) {
				double percentage = getVirtualTableContentProvider().getApproximatePercentageForElement(element);
				int itemCount = getItemCount();
				int visibleItemCount = getVisibleItemCount();
				int reserveAboveTop = visibleItemCount;
				int reserveBelowBottom = visibleItemCount;
				int oldTopIndex = getTopIndex();
				int newTopIndex = (int)(reserveAboveTop + ((itemCount - reserveAboveTop - reserveBelowBottom - visibleItemCount) * percentage));

				fixTablePosition(oldTopIndex, newTopIndex, element, newTopIndex + visibleItemCount / 2);

				table.clearAll();
				table.redraw();
			}
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
			setApproximateItemCount();
			relocateFixPoint(getVirtualTableContentProvider().getFirstElement(), 0);
		}
	}

	protected void setApproximateItemCount() {
		long itemCount = getVirtualTableContentProvider().getApproximateNumberOfElements();
		System.out.println("Approximate virtual table size: " + itemCount);
		itemCount = itemCount < 10000 ? itemCount : 10000;
		itemCount = Math.max(itemCount, getItemCountLowerBound());
		System.out.println("Real table size: " + itemCount);
		table.setItemCount((int)itemCount);
	}

	protected long getItemCountLowerBound() {
		Object element = getVirtualTableContentProvider().getFirstElement();

		if (element == null)
			return 0;
		else
			return getVirtualTableContentProvider().getDistanceToLastElement(element, getVisibleItemCount() * 2);
	}
	
	protected void relocateFixPoint(Object element, int index) {
		fixPointElement = element;
		fixPointTableIndex = index;
	}

	protected void fixTable() {
		int currentTopIndex = getTopIndex();
		
		if (alreadyInFixTable || currentTopIndex == oldTopIndex)
			return;
		else
			alreadyInFixTable = true;

		int itemCount = getItemCount();
		int currentBottomIndex = getBottomIndex();
		int visibleItemCount = getVisibleItemCount();
		int newTopIndex = -1;
		int newFixPointTableIndex = -1;
		Object newFixPointElement = null;
		IVirtualTableContentProvider virtualTableContentProvider = getVirtualTableContentProvider();
		int indexDelta = currentTopIndex - oldTopIndex;
		boolean forward = indexDelta > 0;

		// step 1. set fix point and top index to reflect the new virtual table position
		System.out.println("Index delta: " + indexDelta);
		
		if (Math.abs(indexDelta) < visibleItemCount) {
			// scroll virtual table linearly according to the small scroll in the real table and move real table back to its old position
			newTopIndex = oldTopIndex;
			newFixPointElement = fixPointElement;
			newFixPointTableIndex = fixPointTableIndex - indexDelta;
		}
		else {
			// jump to approximate element in virtual table based on real the table position and leave real table position as it is
			double percentage = (double)currentTopIndex / (itemCount - visibleItemCount);
			newTopIndex = currentTopIndex;
			newFixPointElement = virtualTableContentProvider.getApproximateElementAt(percentage);
			newFixPointTableIndex = forward ? currentBottomIndex : currentTopIndex;
		}

		// step 2. move table and selection and set new fix point
		fixTablePosition(currentTopIndex, newTopIndex, newFixPointElement, newFixPointTableIndex);

		// step 3. if we are near the beginning or the end make sure that there are as many real table lines as virtual table lines below and/or above the table
		int reserveAboveTop = visibleItemCount;
		int reserveBelowTop = visibleItemCount;
		int firstElementDistance = (int)virtualTableContentProvider.getDistanceToFirstElement(getTopElement(), reserveAboveTop);
		int lastElementDistance = (int)virtualTableContentProvider.getDistanceToLastElement(getBottomElement(), reserveBelowTop);
		boolean nearFirstElement = firstElementDistance < reserveAboveTop;
		boolean nearLastElement = lastElementDistance < reserveBelowTop;

		if (nearFirstElement && nearLastElement) {
			fixTablePosition(newTopIndex, firstElementDistance, virtualTableContentProvider.getFirstElement(), 0);
			table.setItemCount(1 + (int)virtualTableContentProvider.getDistanceToLastElement(virtualTableContentProvider.getFirstElement(), Long.MAX_VALUE));
		}
		else if (nearFirstElement)
			fixTablePosition(newTopIndex, firstElementDistance, virtualTableContentProvider.getFirstElement(), 0);
		else if (nearLastElement)
			fixTablePosition(newTopIndex, itemCount - visibleItemCount - lastElementDistance, virtualTableContentProvider.getLastElement(), itemCount - 1);
		else {
			double percentage = virtualTableContentProvider.getApproximatePercentageForElement(getTopElement());
			int modifiedTopIndex = visibleItemCount + (int)((itemCount - visibleItemCount * 3) * percentage);
			fixTablePosition(newTopIndex, modifiedTopIndex, fixPointElement, fixPointTableIndex - newTopIndex + modifiedTopIndex);
		}

		// step 4. relocated fix point to the top of the visible area, so that it's close to what is visible
		relocateFixPoint(getTopElement(), getTopIndex());
		
		// step 5. remember the last set top index for the next call to fixTable
		oldTopIndex = getTopIndex();

		table.clearAll();
		table.redraw();
		System.out.println("Top element after fixTable: " + getTopElement() + " new top index: " + getTopIndex());
		
		alreadyInFixTable = false;
	}
	
	protected void fixTablePosition(int oldTopIndex, int newTopIndex, Object fixPointElement, int fixPointTableIndex) {		
		int deltaTopIndex = newTopIndex - oldTopIndex;
		relocateFixPoint(fixPointElement, fixPointTableIndex);

		if (oldTopIndex != newTopIndex) {
			int selectedIndex = table.getSelectionIndex();

			if (selectedIndex != -1)
				table.setSelection(selectedIndex + deltaTopIndex);

			table.setTopIndex(newTopIndex);
		}
	}

	protected int getItemCount() {
		return table.getItemCount();
	}
	
	protected int getTopIndex() {
		return table.getTopIndex();
	}
	
	protected int getBottomIndex() {
		return table.getTopIndex() + getVisibleItemCount() - 1;
	}

	private Object getElementAt(int index) {
		int elementIndexDelta = index - fixPointTableIndex;
		return getVirtualTableContentProvider().getNeighbourElement(fixPointElement, elementIndexDelta);
	}
	
	protected Object getTopElement() {
		Object element = getElementAt(getTopIndex());
		
		if (element == null)
			throw new IllegalStateException();

		return element;
	}

	protected Object getBottomElement() {
		Object element = getElementAt(getBottomIndex());
		
		if (element == null)
			throw new IllegalStateException();

		return element;
	}
		
	public boolean isFirstElement(Object element) {
		return getVirtualTableContentProvider().getDistanceToFirstElement(element, 1) == 0;
	}

	public boolean isLastElement(Object element) {
		return getVirtualTableContentProvider().getDistanceToLastElement(element, 1) == 0;
	}

	protected int getVisibleItemCount() {
		int itemHeight = table.getItemHeight();
		int headerHeight = table.getHeaderHeight();
		int height = table.getSize().y;
		
		return Math.max(0, (height - headerHeight) / itemHeight - 1);
	}
}
