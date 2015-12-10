/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.figures.layout;

import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;

/**
 * Submodule layout is using this interface
 * @author andras
 */
public interface ISubmoduleConstraint {
    enum VectorArrangement {none, exact, row, column, matrix, ring}; // names must match IDisplayString.Prop.LAYOUT names

    /**
     * Returns the position that occurs in the display string, or null. For non-vector
     * submodules this should be set as centerLocation; for submodule vectors, centerLocation
     * will be this location plus an offset calculated from vectorArrangement, vectorSize,
     * vectorIndex and vector arrangement parameters.
     */
    public Point getBaseLocation();

    /**
     * Identifies the vector this submodule belongs to, or null if the submodule
     * is not in a vector.
     */
    public Object getVectorIdentifier();

    /**
     * The size of the module vector. Returns 0 if it is not a module vector.
     */
    public int getVectorSize();

    /**
     * The index of the module in a vector (0..getVectorSize()-1)
     */
    public int getVectorIndex();

    /**
     * The type of the vector arrangement (exact, column, etc...)
     */
    public VectorArrangement getVectorArrangement();

    /**
     * Parameters for the vector arrangement.
     */
    public VectorArrangementParameters getVectorArrangementParameters();

    /**
     * Sets the location of a submodule (the center point of its main area icon/shape).
     * This method must store the currently set value which can be later returned by
     * {@link #getCenterLocation()}. Setting it to <code>null</code> is allowed meaning
     * that the submodule location must be determined by the layouting algorithm.
     * In addition this function MUST set the bounds of the figure correctly based on the
     * size of the figure and the currently set center point.
     */
    public void setCenterLocation(Point loc);

    /**
     * The center point of the main area (icon/shape) of a submodule figure.
     * Returns <code>null</code> if setCenterLocation() was not called before. This
     * means that the submodule was added to the parent recently and an auto-layout
     * should be executed to place the submodule inside the compound module.
     */
    public Point getCenterLocation();

    /**
     * The size of the main shape of submodule; the layout algorithm may take this into
     * account. Return null or (0,0) if size is not specified or is to be ignored
     * during layouting.
     */
    public Dimension getShapeSize();
}
