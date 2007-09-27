package org.omnetpp.test.gui.access;

import org.eclipse.swt.widgets.Control;

import com.simulcraft.test.gui.access.ControlAccess;
import com.simulcraft.test.gui.access.TreeAccess;

public class InputsPageAccess extends ControlAccess {

	public InputsPageAccess(Control control) {
		super(control);
	}
	
	public TreeAccess getFilesViewTree() {
		return null;
	}
	
	public TreeAccess getLogicalViewTree() {
		return null;
	}
	
	public TreeAccess getFileRunViewTree() {
		return null;
	}
	
	public TreeAccess getRunFileViewTree() {
		return null;
	}
}
