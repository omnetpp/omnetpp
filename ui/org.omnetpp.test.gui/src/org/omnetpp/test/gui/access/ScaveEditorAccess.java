package org.omnetpp.test.gui.access;

import org.eclipse.ui.part.MultiPageEditorPart;

import com.simulcraft.test.gui.access.MultiPageEditorPartAccess;

public class ScaveEditorAccess extends MultiPageEditorPartAccess {

	public ScaveEditorAccess(MultiPageEditorPart scaveEditor) {
		super(scaveEditor);
	}
	
	public InputsPageAccess ensureInputsPageActive() {
		return (InputsPageAccess)ensureActivePage("Inputs");
	}

}
