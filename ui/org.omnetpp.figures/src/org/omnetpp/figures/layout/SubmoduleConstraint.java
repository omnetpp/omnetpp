/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.figures.layout;

import org.eclipse.draw2d.geometry.Rectangle;

/**
 * Behaves exactly like the rectangle (i.e. can be used with XYLayout, however, it adds
 * additional info what makes possible additional layouting specific to submodule vectors
 * (ring, mesh etc...)
 *
 * @author rhornig
 */
public class SubmoduleConstraint extends Rectangle {
	static final long serialVersionUID = 1;
	private boolean pinned = true;
	protected String vectorName = null;
	protected int vectorIndex = 0;
	protected int vectorSize = 0;

	public SubmoduleConstraint() {
		x = Integer.MIN_VALUE;
		y = Integer.MIN_VALUE;
		width = -1;
		height = -1;
	}
	
	public void setVectorName(String name) {
		this.vectorName = name;
	}

	public void setVectorSize(int size) {
		vectorSize = size;
	}

	public void setVectorIndex(int index) {
		vectorIndex = index;
	}

	public int getVectorIndex() {
		return vectorIndex;
	}

	public String getVectorName() {
		return vectorName;
	}

	public int getVectorSize() {
		return vectorSize;
	}

	/**
	 * Set whether the module should be treated as fixed by a full layout alg.
	 * If it is not pinned then the coordinates in this constraint should be treated
	 * as initial values for the layouting .
	 */
	public void setPinned(boolean pinned) {
		this.pinned = pinned;
	}

	public boolean isPinned() {
		return pinned;
	}

	/**
	 * If a constraint is unspecified (default) the layouter should assign a coordinate
	 * at the next full or incremental layout. Accessing x,y,width,height when the constraint
	 * is undefined results in unspecified value.  
	 * If it is specified the module can be either unpinned (full layout can move it) 
	 * or pinned (always fixed) 
	 */
	public boolean isUnspecified() {
		return x == Integer.MIN_VALUE || y == Integer.MIN_VALUE;
	}

}
