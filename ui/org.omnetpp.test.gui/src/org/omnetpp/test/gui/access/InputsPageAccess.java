package org.omnetpp.test.gui.access;

import static org.omnetpp.scave.TestSupport.FILE_RUN_VIEW_TREE_ID;
import static org.omnetpp.scave.TestSupport.INPUT_FILES_TREE;
import static org.omnetpp.scave.TestSupport.LOGICAL_VIEW_TREE_ID;
import static org.omnetpp.scave.TestSupport.RUN_FILE_VIEW_TREE_ID;

import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;

import com.simulcraft.test.gui.access.ControlAccess;
import com.simulcraft.test.gui.access.TreeAccess;
import com.simulcraft.test.gui.util.Predicate;

public class InputsPageAccess extends ControlAccess {

	public InputsPageAccess(Control control) {
		super(control);
	}
	
	public Composite getInputsPage() {
		return (Composite)widget;
	}
	
	public TreeAccess getInputFilesViewTree() {
		return (TreeAccess)createAccess(
					findDescendantControl(
							getInputsPage(),
							Predicate.hasID(INPUT_FILES_TREE)));
	}
	
	public TreeAccess getFileRunViewTree() {
		return (TreeAccess)createAccess(
				findDescendantControl(
						getInputsPage(),
						Predicate.hasID(FILE_RUN_VIEW_TREE_ID)));
	}
	
	public TreeAccess getRunFileViewTree() {
		return (TreeAccess)createAccess(
				findDescendantControl(
						getInputsPage(),
						Predicate.hasID(RUN_FILE_VIEW_TREE_ID)));
	}

	public TreeAccess getLogicalViewTree() {
		return (TreeAccess)createAccess(
				findDescendantControl(
						getInputsPage(),
						Predicate.hasID(LOGICAL_VIEW_TREE_ID)));
	}
}
