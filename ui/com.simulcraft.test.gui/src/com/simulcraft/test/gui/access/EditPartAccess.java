package com.simulcraft.test.gui.access;

import org.eclipse.gef.EditPart;

public class EditPartAccess
	extends Access
{
	protected EditPart editPart;
	
	public EditPartAccess(EditPart editPart) {
		this.editPart = editPart;
	}
	
	public EditPart getEditPart() {
		return editPart;
	}
}
