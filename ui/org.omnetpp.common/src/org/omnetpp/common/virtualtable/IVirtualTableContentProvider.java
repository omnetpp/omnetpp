/**
 * 
 */
package org.omnetpp.common.virtualtable;

import org.eclipse.jface.viewers.IContentProvider;

public interface IVirtualTableContentProvider extends IContentProvider {
	public Object getFirstElement();
	public Object getLastElement();
	
	public long getDistanceToElement(Object sourceElement, Object targetElement, long limit);
	public long getDistanceToFirstElement(Object element, long limit);
	public long getDistanceToLastElement(Object element, long limit);

	public Object getNeighbourElement(Object element, long distance);

	public long getApproximateNumberOfElements();
	public Object getApproximateElementAt(double percentage);
	public double getApproximatePercentageForElement(Object element);
}