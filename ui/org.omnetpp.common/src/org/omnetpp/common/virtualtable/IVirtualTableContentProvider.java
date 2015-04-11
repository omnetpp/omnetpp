/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.virtualtable;

import java.util.Comparator;

import org.eclipse.jface.viewers.IContentProvider;

/**
 * The virtual table retrives the displayed elements by using an instance of this interface.
 */
public interface IVirtualTableContentProvider<T> extends IContentProvider, Comparator<T> {
    /**
     * Returns the first element of the virtual table.
     */
    public T getFirstElement();

    /**
     * Returns the last element of the virtual table.
     */
    public T getLastElement();

    /**
     * Returns the distance between source and target.
     * Limit is a signed value which tells in what direction should the distance be measured.
     * If the distance would be greater than the absolute value of limit then measurement
     * should stop and return with the absolute value of limit.
     */
    public long getDistanceToElement(T sourceElement, T targetElement, long limit);

    /**
     * Same as getDistanceToElement but it measures against the first element.
     */
    public long getDistanceToFirstElement(T element, long limit);

    /**
     * Same as getDistanceToElement but it measures against the last element.
     */
    public long getDistanceToLastElement(T element, long limit);

    /**
     * Returns the neighbour of element or null in the given signed distance.
     */
    public T getNeighbourElement(T element, long distance);

    /**
     * Returns the closest element in both directions that should be displayed in the virtual table.
     * This is used when the content is filtered and the filter changes.
     */
    public T getClosestElement(T element);

    /**
     * Returns an approximation of the number of elements. This value should be precise when
     * the number of elements is a reasonably small value.
     */
    public long getApproximateNumberOfElements();

    /**
     * Returns an approximate element at the given percentage [0.0 1.0].
     * The elements returned must follow the order of their percentage values.
     */
    public T getApproximateElementAt(double percentage);

    /**
     * Returns an approximation of the element's position in the table.
     * The values returned must follow the order of their elements.
     */
    public double getApproximatePercentageForElement(T element);
}