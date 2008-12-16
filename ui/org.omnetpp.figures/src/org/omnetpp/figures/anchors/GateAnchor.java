/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.figures.anchors;

import org.eclipse.draw2d.ChopboxAnchor;
import org.eclipse.draw2d.IFigure;

/**
 * Provides support for Ned Model specific behaviors ie: gatename
 *
 * @author rhornig
 */
public class GateAnchor extends ChopboxAnchor {
//	protected String gateName;

    public GateAnchor(IFigure owner) {
    	super(owner);
    }

//	public String getGateName() {
//		return gateName;
//	}


}
