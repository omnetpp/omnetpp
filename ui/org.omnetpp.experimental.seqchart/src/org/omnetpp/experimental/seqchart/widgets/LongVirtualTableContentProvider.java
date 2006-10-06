package org.omnetpp.experimental.seqchart.widgets;

import org.eclipse.jface.viewers.Viewer;

/**
 * This class serves debugging purposes.
 */
public class LongVirtualTableContentProvider implements IVirtualTableContentProvider {
	protected static boolean debug = true;

	protected long maxValue;

	public Object getFirstElement() {
		if (debug)
			System.out.println("Virtual table content provider getFirstElement");

		return 0L;
	}

	public Object getLastElement() {
		if (debug)
			System.out.println("Virtual table content provider getLastElement");

		return maxValue;
	}

	public long getDistanceToElement(Object sourceElement, Object targetElement, long limit)
	{
		if (debug)
			System.out.println("Virtual table content provider getDistanceToElement sourceElement: " + sourceElement + " targetElement: " + targetElement + " limit: " + limit);

		try {
			Thread.sleep(limit);
		}
		catch (Exception e) {			
		}

		long sourceValue = (Long)sourceElement;
		long targetValue = (Long)targetElement;
		long delta = Math.abs(targetValue - sourceValue);
		
		if (delta > limit)
			return limit;
		else
			return delta;
	}

	public long getDistanceToFirstElement(Object element, long limit) {
		if (debug)
			System.out.println("Virtual table content provider getDistanceToFirstElement element: " + element + " limit: " + limit);

		long value = (Long)element;
		
		try {
			Thread.sleep(limit);
		}
		catch (Exception e) {			
		}

		if (value > limit)
			return limit;
		else
			return value;
	}

	public long getDistanceToLastElement(Object element, long limit) {
		if (debug)
			System.out.println("Virtual table content provider getDistanceToLastElement element: " + element + " limit: " + limit);

		long value = (Long)element;
		
		try {
			Thread.sleep(limit);
		}
		catch (Exception e) {			
		}

		if (maxValue - value > limit)
			return limit;
		else
			return maxValue - value;
	}

	public Object getNeighbourElement(Object element, long distance) {
		if (debug)
			System.out.println("Virtual table content provider getNeighbourElement element: " + element + " distance: " + distance);

		long value = (Long)element;
		
		try {
			Thread.sleep(distance);
		}
		catch (Exception e) {			
		}
		
		if ((value + distance < 0) || (value + distance > maxValue))
			return null;
		else
			return value + distance;
	}

	public double getApproximatePercentageForElement(Object element) {
		if (debug)
			System.out.println("Virtual table content provider getApproximatePercentageForElement element: " + element);

		return (double)(Long)element / maxValue;
	}

	public Object getApproximateElementAt(double percentage) {
		if (debug)
			System.out.println("Virtual table content provider getApproximateElementAt percentage: " + percentage);

		return (long)(maxValue * percentage);
	}

	public long getApproximateNumberOfElements() {
		if (debug)
			System.out.println("Virtual table content provider getApproximateNumberOfElements");

		return maxValue + 1;
	}

	public void dispose() {
	}

	public void inputChanged(Viewer viewer, Object oldInput, Object newInput) {
		maxValue = (Long)newInput;
	}
}