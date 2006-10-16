package org.omnetpp.experimental.seqchart.widgets;

import org.eclipse.jface.viewers.Viewer;

/**
 * This class serves debugging purposes.
 */
public class LongVirtualTableContentProvider implements IVirtualTableContentProvider {
	protected static boolean debug = true;

	protected double sleepScale = 0.1;

	protected long maxValue;
	
	protected long sleep(long value) {
		try {
			Thread.sleep((int)(value * sleepScale));
		}
		catch (Exception e) {			
		}
		
		return value;
	}

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

		long sourceValue = (Long)sourceElement;
		long targetValue = (Long)targetElement;
		long delta = Math.abs(targetValue - sourceValue);
		
		if (delta > limit)
			return sleep(limit);
		else
			return sleep(delta);
	}

	public long getDistanceToFirstElement(Object element, long limit) {
		if (debug)
			System.out.println("Virtual table content provider getDistanceToFirstElement element: " + element + " limit: " + limit);

		long value = (Long)element;
		
		if (value > limit)
			return sleep(limit);
		else
			return sleep(value);
	}

	public long getDistanceToLastElement(Object element, long limit) {
		if (debug)
			System.out.println("Virtual table content provider getDistanceToLastElement element: " + element + " limit: " + limit);

		long value = (Long)element;
		
		if (maxValue - value > limit)
			return sleep(limit);
		else
			return sleep(maxValue - value);
	}

	public Object getNeighbourElement(Object element, long distance) {
		if (debug)
			System.out.println("Virtual table content provider getNeighbourElement element: " + element + " distance: " + distance);

		long value = (Long)element;
		
		if (value + distance < 0)
			distance = -value;
		
		if (value + distance > maxValue)
			distance = maxValue - value;
		
		sleep(distance);

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