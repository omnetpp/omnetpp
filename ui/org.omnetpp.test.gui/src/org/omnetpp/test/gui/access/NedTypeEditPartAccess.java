package org.omnetpp.test.gui.access;

import org.omnetpp.ned.editor.graph.edit.NedTypeEditPart;


public class NedTypeEditPartAccess extends EditPartAccess
{
	public NedTypeEditPartAccess(NedTypeEditPart NEDTypeEditPart) {
		super(NEDTypeEditPart);
	}
	
	public NedTypeEditPart getNEDTypeEditPart() {
		return (NedTypeEditPart)editPart;
	}
}
