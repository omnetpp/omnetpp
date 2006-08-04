package org.omnetpp.figures.layout;

import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.common.displaymodel.IDisplayString;

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

	public SubmoduleConstraint(IDisplayString dps) {
		super();
		// copy the location and size from the displaystring
		Point loc = dps.getLocation();
        // special case:
        // Integer.MIN_VALUE signals that the coordinate is unpinned and the
        // layouter can move it freely (if no location is specified in the displaystring)
        if (loc == null) 
        	loc = new Point(Integer.MIN_VALUE, Integer.MIN_VALUE);

        setLocation(loc);
        setSize(dps.getSize());
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

}
