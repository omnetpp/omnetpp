package org.omnetpp.figures.layout;

import org.eclipse.draw2d.geometry.Rectangle;

/**
 * Behaves exactly like the rectangle (ie. can be used with XYLayout, however adds
 * additional info what makes possible additional layouting specific to submodule vectors
 * (ring, mesh etc...)
 * @author rhornig
 */
public class SubmoduleConstraint extends Rectangle {
	static final long serialVersionUID = 1;
	protected String vectorName = null;
	protected int vectorIndex = 0;
	protected int vectorSize = 0;

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

}
