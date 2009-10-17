/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.test.gui.access;

import org.omnetpp.ned.editor.graph.parts.NedTypeEditPart;

import com.simulcraft.test.gui.access.EditPartAccess;


public class NedTypeEditPartAccess extends EditPartAccess
{
	public NedTypeEditPartAccess(NedTypeEditPart nedTypeEditPart) {
		super(nedTypeEditPart);
	}

	public NedTypeEditPart getNEDTypeEditPart() {
		return (NedTypeEditPart)editPart;
	}
}
