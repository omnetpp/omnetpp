package org.omnetpp.common.virtualtable;

import org.eclipse.jface.viewers.Viewer;
import org.omnetpp.common.Debug;

/**
 * This class serves debugging purposes.
 */
public class LongVirtualTableContentProvider implements IVirtualTableContentProvider<Long> {
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

	public Long getFirstElement() {
		if (debug)
			Debug.println("Virtual table content provider getFirstElement");

		return 0L;
	}

	public Long getLastElement() {
		if (debug)
			Debug.println("Virtual table content provider getLastElement");

		return maxValue;
	}

	public long getDistanceToElement(Long sourceElement, Long targetElement, long limit)
	{
		if (debug)
			Debug.println("Virtual table content provider getDistanceToElement sourceElement: " + sourceElement + " targetElement: " + targetElement + " limit: " + limit);

		long sourceValue = sourceElement;
		long targetValue = targetElement;
		long delta = targetValue - sourceValue;

		if (Math.abs(delta) > Math.abs(limit) || Math.signum(delta) != Math.signum(limit))
			return sleep(limit);
		else
			return sleep(delta);
	}

	public long getDistanceToFirstElement(Long element, long limit) {
		if (debug)
			Debug.println("Virtual table content provider getDistanceToFirstElement element: " + element + " limit: " + limit);

		long value = element;
		
		if (value > limit)
			return sleep(limit);
		else
			return sleep(value);
	}

	public long getDistanceToLastElement(Long element, long limit) {
		if (debug)
			Debug.println("Virtual table content provider getDistanceToLastElement element: " + element + " limit: " + limit);

		long value = element;
		
		if (maxValue - value > limit)
			return sleep(limit);
		else
			return sleep(maxValue - value);
	}

	public Long getNeighbourElement(Long element, long distance) {
		if (debug)
			Debug.println("Virtual table content provider getNeighbourElement element: " + element + " distance: " + distance);

		long value = element;
		
		if (value + distance < 0)
			distance = -value;
		
		if (value + distance > maxValue)
			distance = maxValue - value;
		
		sleep(distance);

		return value + distance;
	}

	public Long getClosestElement(Long element) {
		if (debug)
			Debug.println("Virtual table content provider getClosestElement element: " + element);

		return element;
	}

	public double getApproximatePercentageForElement(Long element) {
		if (debug)
			Debug.println("Virtual table content provider getApproximatePercentageForElement element: " + element);

		return (double)element / maxValue;
	}

	public Long getApproximateElementAt(double percentage) {
		if (debug)
			Debug.println("Virtual table content provider getApproximateElementAt percentage: " + percentage);

		return (long)(maxValue * percentage);
	}

	public long getApproximateNumberOfElements() {
		if (debug)
			Debug.println("Virtual table content provider getApproximateNumberOfElements");

		return maxValue + 1;
	}

	public void dispose() {
	}

	public void inputChanged(Viewer viewer, Object oldInput, Object newInput) {
		if (newInput != null)
			maxValue = (Long)newInput;
	}
}