/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.figures.layout;

import org.omnetpp.common.displaymodel.DimensionF;
import org.omnetpp.common.displaymodel.PointF;

/**
 * Submodule layout is using this interface
 * @author andras
 */
public interface ISubmoduleConstraint {
    /**
     * Vector arrangement. Names must match IDisplayString.Prop.LAYOUT names.
     */
    public enum VectorArrangement {none, exact, row, column, matrix, ring};

    /**
     * Returns the position that occurs in the display string, or null. For non-vector
     * submodules this should be set as centerLocation; for submodule vectors, centerLocation
     * will be this location plus an offset calculated from vectorArrangement, vectorSize,
     * vectorIndex and vector arrangement parameters.
     */
    public PointF getBaseLocation();

    /**
     * The size of the main shape of submodule; the layout algorithm may take this into
     * account. Return null or (0,0) if size is not specified or is to be ignored
     * during layouting.
     */
    public DimensionF getShapeSize();

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
     * Sets the layouted location of a submodule (the center point of its
     * main area icon/shape). Pass null to invalidate the currrent location;
     * this will cause the submodule to be layouted next time the layouter
     * is invoked.
     */
    public void setLayoutedLocation(PointF loc);

    /**
     * Returns the layouted location of the submodule, or null if it was not yet layouted.
     */
    public PointF getLayoutedLocation();
}
