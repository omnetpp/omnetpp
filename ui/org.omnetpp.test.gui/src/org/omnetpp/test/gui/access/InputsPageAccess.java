package org.omnetpp.test.gui.access;

import static org.omnetpp.scave.TestSupport.FILE_RUN_VIEW_TREE_ID;
import static org.omnetpp.scave.TestSupport.INPUT_FILES_TREE;
import static org.omnetpp.scave.TestSupport.LOGICAL_VIEW_TREE_ID;
import static org.omnetpp.scave.TestSupport.RUN_FILE_VIEW_TREE_ID;

import org.eclipse.swt.custom.CTabItem;
import org.eclipse.swt.widgets.Composite;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.CTabItemAccess;
import com.simulcraft.test.gui.access.CompositeAccess;
import com.simulcraft.test.gui.access.TreeAccess;
import com.simulcraft.test.gui.core.InUIThread;
import com.simulcraft.test.gui.util.Predicate;

public class InputsPageAccess extends CompositeAccess {

	public InputsPageAccess(Composite control) {
		super(control);
	}
	
	public TreeAccess getInputFilesViewTree() {
		return (TreeAccess)createAccess(
					findDescendantControl(
							getControl(),
							Predicate.hasID(INPUT_FILES_TREE)));
	}
	
	public TreeAccess getFileRunViewTree() {
		return (TreeAccess)createAccess(
				findDescendantControl(
						getControl(),
						Predicate.hasID(FILE_RUN_VIEW_TREE_ID)));
	}
	
	public TreeAccess getRunFileViewTree() {
		return (TreeAccess)createAccess(
				findDescendantControl(
						getControl(),
						Predicate.hasID(RUN_FILE_VIEW_TREE_ID)));
	}

	public TreeAccess getLogicalViewTree() {
		return (TreeAccess)createAccess(
				findDescendantControl(
						getControl(),
						Predicate.hasID(LOGICAL_VIEW_TREE_ID)));
	}
	
	@InUIThread
	public CTabItem findTab(String label) {
		return Access.findDescendantCTabItemByLabel(getControl(), label);
	}
	
	@InUIThread
	public boolean isTabSelected(String label) {
		CTabItem item = findTab(label);
		return item.getParent().getSelection() == item;
	}
	
	public void selectTab(String label) {
		CTabItemAccess item = (CTabItemAccess)createAccess(findTab(label));
		item.click();
	}
	
	public void ensureTabSelected(String label) {
		if (!isTabSelected(label))
			selectTab(label);
	}
	
	public void ensureFileRunViewVisible() {
		ensureTabSelected(".*file.*run.*");
	}
	
	public void ensureRunFileViewVisible() {
		ensureTabSelected(".*run.*file.*");
	}
	
	public void ensureLogicalViewVisible() {
		ensureTabSelected(".*[lL]ogical.*");
	}
}
