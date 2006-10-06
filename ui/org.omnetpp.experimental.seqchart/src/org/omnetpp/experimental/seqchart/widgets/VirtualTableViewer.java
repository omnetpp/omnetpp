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
	protected Table table;
	
	protected Object fixPointElement;
	
	protected int fixPointTableIndex;

	protected boolean followSelection = true;
	
	public VirtualTableViewer(Table table) {
		this.table = table;
		
		table.addListener(SWT.SetData, new Listener() {
			public void handleEvent(Event e) {
				TableItem item = (TableItem)e.item;
				Object element = getVirtualTableContentProvider().getNeighbourElement(fixPointElement, e.index - fixPointTableIndex);
				getVirtualTableItemProvider().fillTableItem(item, element);
			}
		});
		table.addPaintListener(new PaintListener() {
			public void paintControl(PaintEvent e) {
				fixTable2();
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

	boolean alreadyInFixTable;
	
	int oldTopIndex;
	int oldBottomIndex;
	
	protected void fixTable2() {
		int itemCount = getItemCount();
		int newTopIndex = getTopIndex();
		int newBottomIndex = getBottomIndex();
		
		if (newTopIndex == oldTopIndex)
			return;
		
		int indexDelta = newTopIndex - oldTopIndex;
		boolean forward = indexDelta > 0;
		int visibleItemCount = getVisibleItemCount();
		IVirtualTableContentProvider virtualTableContentProvider = getVirtualTableContentProvider();

		// step 1. set fix point and top index to reflect the new virtual table position
		System.out.println("Index delta: " + indexDelta);
		
		if (indexDelta <= visibleItemCount) {
			// scroll virtual table linearly according to the small scroll in the real table
			double percentage = virtualTableContentProvider.getApproximatePercentageForElement(getTopElement());
			int xxxTopIndex = (int)((itemCount - visibleItemCount) * percentage);
			table.setTopIndex(xxxTopIndex);
			relocateFixPoint(fixPointElement, fixPointTableIndex - newTopIndex + xxxTopIndex);
		}
		else {
			// jump to approximate element in virtual table based on real the table position
			double percentage = (double)newTopIndex / (itemCount - visibleItemCount);
			Object element = virtualTableContentProvider.getApproximateElementAt(percentage);
			relocateFixPoint(element, forward ? newBottomIndex : newTopIndex);
		}

		// step 2. if we are near the beginning or the end make sure that there are as many real table lines as virtual table lines below and/or above the table
		int reserveAboveTop = visibleItemCount;
		int reserveBelowBottom = visibleItemCount;
		int firstElementDistance = (int)virtualTableContentProvider.getDistanceToFirstElement(getTopElement(), reserveAboveTop);
		int lastElementDistance = (int)virtualTableContentProvider.getDistanceToLastElement(getBottomElement(), reserveBelowBottom);
		boolean nearFirstElement = firstElementDistance < reserveAboveTop;
		boolean nearLastElement = lastElementDistance < reserveBelowBottom;

		if (nearFirstElement && nearLastElement) {
			// TODO: how will it be less than visibleItemCount?
			fixTablePosition(oldTopIndex, firstElementDistance, virtualTableContentProvider.getFirstElement(), 0);
			table.setItemCount(firstElementDistance + lastElementDistance + visibleItemCount);
		}
		else if (nearFirstElement)
			fixTablePosition(oldTopIndex, firstElementDistance, virtualTableContentProvider.getFirstElement(), 0);
		else if (nearLastElement)
			fixTablePosition(oldTopIndex, itemCount - lastElementDistance - visibleItemCount, virtualTableContentProvider.getLastElement(), itemCount - 1);

		// step 3. move fix point to the top of the visible area
		relocateFixPoint(getTopElement(), getTopIndex());
		
		oldTopIndex = getTopIndex();
		oldBottomIndex = getBottomIndex();
	}
	
	protected void fixTable() {
		if (alreadyInFixTable)
			return;
		else
			alreadyInFixTable = true;
		
		System.out.println("fixTable");

		int itemCount = getItemCount();
		int oldTopIndex = getTopIndex();
		int oldBottomIndex = getBottomIndex();
		Object topElement = getTopElement();
		Object bottomElement = getBottomElement();
		int visibleItemCount = getVisibleItemCount();
		int reserveAboveTop = visibleItemCount;
		int reserveBelowBottom = visibleItemCount;
		IVirtualTableContentProvider virtualTableContentProvider = getVirtualTableContentProvider();
		int firstElementDistance = (int)virtualTableContentProvider.getDistanceToFirstElement(topElement, reserveAboveTop);
		int lastElementDistance = (int)virtualTableContentProvider.getDistanceToLastElement(bottomElement, reserveBelowBottom);
		
		//System.out.println("Top element before fix table: " + topElement + " top index: " + oldTopIndex);

		if (firstElementDistance < reserveAboveTop) {
			if (!isFirstElement(fixPointElement) || fixPointTableIndex != 0)
				fixTablePosition(oldTopIndex, firstElementDistance, virtualTableContentProvider.getFirstElement(), 0);
		}
		else if (lastElementDistance  < reserveBelowBottom) {
			if (!isLastElement(fixPointElement) || fixPointTableIndex != itemCount - 1)
				fixTablePosition(oldTopIndex, itemCount - lastElementDistance - visibleItemCount, virtualTableContentProvider.getLastElement(), itemCount - 1);
		}
		else {
			double percentage = virtualTableContentProvider.getApproximatePercentageForElement(topElement);
			int newTopIndex = (int)(reserveAboveTop + ((itemCount - reserveAboveTop - reserveBelowBottom - visibleItemCount) * percentage));
			
			if (oldTopIndex != newTopIndex) {
				int deltaTopIndex = newTopIndex - oldTopIndex;
	
				if (Math.abs(deltaTopIndex) > visibleItemCount * 2) {
					percentage = (double)oldTopIndex / (itemCount - visibleItemCount);
					Object element = virtualTableContentProvider.getApproximateElementAt(percentage);					
					fixTablePosition(oldTopIndex, oldTopIndex, element, deltaTopIndex > 0 ? oldTopIndex : oldBottomIndex);
				}
				else
					fixTablePosition(oldTopIndex, newTopIndex, topElement, newTopIndex);
			}
		}
		
		alreadyInFixTable = false;
	}
	
	protected void fixTablePosition(int oldTopIndex, int newTopIndex, Object fixPointElement, int fixPointTableIndex) {		
		int deltaTopIndex = newTopIndex - oldTopIndex;
		relocateFixPoint(fixPointElement, fixPointTableIndex);

		if (oldTopIndex != newTopIndex) {
			table.setSelection(table.getSelectionIndex() + deltaTopIndex);
			table.setTopIndex(newTopIndex);
		}

		table.clearAll();
		table.redraw();

		System.out.println("Top element after relocate: " + getTopElement() + " new top index: " + newTopIndex + " delta top index: " + deltaTopIndex);
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
