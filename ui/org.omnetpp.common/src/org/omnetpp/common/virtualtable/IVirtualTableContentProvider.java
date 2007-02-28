package org.omnetpp.common.virtualtable;

import org.eclipse.jface.viewers.IContentProvider;

public interface IVirtualTableContentProvider<T> extends IContentProvider {
	public T getFirstElement();
	public T getLastElement();
	
	public long getDistanceToElement(T sourceElement, T targetElement, long limit);
	public long getDistanceToFirstElement(T element, long limit);
	public long getDistanceToLastElement(T element, long limit);

	public T getNeighbourElement(T element, long distance);
	public T getClosestElement(T element);

	public long getApproximateNumberOfElements();
	public T getApproximateElementAt(double percentage);
	public double getApproximatePercentageForElement(T element);
}