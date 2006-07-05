package org.omnetpp.figures;

import org.eclipse.draw2d.ChopboxAnchor;
import org.eclipse.draw2d.IFigure;

/**
 * @author rhornig
 * Provides support for Ned Model specific behaviors ie: gatename
 */
public class GateAnchor extends ChopboxAnchor {
	protected String gateName;
	
    public GateAnchor(IFigure owner) {
    	super(owner);
    }
    
    public GateAnchor(IFigure owner, String name) {
    	super(owner);
    	this.gateName = name;
    }

	public String getGateName() {
		return gateName;
	}
    
    
}
