/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
 *--------------------------------------------------------------*/

package org.omnetpp.figures.layout;


import org.apache.commons.lang3.ObjectUtils;
import org.omnetpp.common.displaymodel.DimensionF;
import org.omnetpp.common.displaymodel.PointF;

/**
 * The default submodule constraint (for layouting).
 *
 * @author andras
 */
public class SubmoduleConstraint implements ISubmoduleConstraint {
    // input of layouting
    protected PointF baseLoc;
    protected DimensionF shapeSize;
    protected Object vectorIdentifier;
    protected int vectorSize;
    protected int vectorIndex;
    protected String vectorArrangementPar1;
    protected String vectorArrangementPar2;
    protected String vectorArrangementPar3;
    protected VectorArrangement vectorArrangement;
    protected VectorArrangementParameters vectorArrangementParams;

    // result of layouting
    protected PointF centerLoc;

    /**
     * Sets the base location. The return value indicates whether the value has
     * actually changed from its previous value.
     */
    public boolean setBaseLocation(PointF baseLoc) {
        if (ObjectUtils.equals(this.baseLoc, baseLoc))
            return false;
        this.baseLoc = baseLoc;
        return true;
    }

    @Override
    public PointF getBaseLocation() {
        return baseLoc;
    }

    /**
     * Sets the shape size. The return value indicates whether the value has
     * actually changed from its previous value.
     */
    public boolean setShapeSize(DimensionF shapeSize) {
        if (ObjectUtils.equals(this.shapeSize, shapeSize))
            return false;
        this.shapeSize = shapeSize;
        return true;
    }

    @Override
    public DimensionF getShapeSize() {
        return shapeSize;
    }

    /**
     * Sets the submodule vector arrangement. The return value indicates whether
     * the value has actually changed from its previous value.
     */
    public boolean setVectorArragement(VectorArrangement vectorArrangement, VectorArrangementParameters vectorArrangementParams) {
        if (this.vectorArrangement == vectorArrangement && ObjectUtils.equals(this.vectorArrangementParams, vectorArrangementParams))
            return false;
        this.vectorArrangement = vectorArrangement;
        this.vectorArrangementParams = vectorArrangementParams;
        return true;
    }

    @Override
    public VectorArrangement getVectorArrangement() {
        return vectorArrangement;
    }

    @Override
    public VectorArrangementParameters getVectorArrangementParameters() {
        return vectorArrangementParams;
    }

    /**
     * Sets the submodule vector index. The return value indicates whether
     * the value has actually changed from its previous value.
     */
    public boolean setSubmoduleVectorIndex(Object vectorIdentifier, int vectorSize, int vectorIndex) {
        if (ObjectUtils.equals(this.vectorIdentifier, vectorIdentifier) && this.vectorSize == vectorSize && this.vectorIndex == vectorIndex)
            return false;
        this.vectorIdentifier = vectorIdentifier;
        this.vectorSize = vectorSize;
        this.vectorIndex = vectorIndex;
        return true;
    }

    @Override
    public Object getVectorIdentifier() {
        return vectorIdentifier;
    }

    @Override
    public int getVectorIndex() {
        return vectorIndex;
    }

    @Override
    public int getVectorSize() {
        return vectorSize;
    }

    @Override
    public void setLayoutedLocation(PointF loc) {
        centerLoc = loc;
    }

    @Override
    public PointF getLayoutedLocation() {
        return centerLoc;
    }

}
