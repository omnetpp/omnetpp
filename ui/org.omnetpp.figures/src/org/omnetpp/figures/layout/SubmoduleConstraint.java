package org.omnetpp.figures.layout;

import org.eclipse.draw2d.geometry.Dimension;
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
	protected boolean moveable = false;

	public SubmoduleConstraint(IDisplayString dps, Float scale) {
		super();
		// copy the location and size from the display string
		Point loc = dps.getLocation(scale);
        // special case:
        // Integer.MIN_VALUE signals that the coordinate is unpinned and the
        // layouter can move it freely (if no location is specified in the display string)
        if (loc == null) {
        	loc = new Point(Integer.MIN_VALUE, Integer.MIN_VALUE);
        	moveable = true;
        }
        setLocation(loc);

        // set the size of the submodule
        // special handling required if either width or height is not present or
        // the whole B tag is missing. In this case the corresponding size should be set to -1
        // to signal the request of automatic resizing
        Dimension size = dps.getSize(scale);
        // the tag is missing totally
        if (!dps.containsTag(IDisplayString.Tag.b)) {
            size.width = -1;
            size.height = -1;
        }
        else {
            // only one of them is missing
            boolean widthExist = dps.containsProperty(IDisplayString.Prop.WIDTH);
            boolean heightExist = dps.containsProperty(IDisplayString.Prop.HEIGHT);
            if (!widthExist && heightExist)
                size.width = -1;
            if (widthExist && !heightExist)
                size.height = -1;
        }

        setSize(size);
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
	 * @return whether the node is freely movable
	 */
	public boolean isMoveable() {
	    return moveable;
	}

}
