package org.omnetpp.test.gui.access;

import static org.omnetpp.scave.TestSupport.CHARTSHEETS_TREE_ID;
import static org.omnetpp.scave.TestSupport.DATASETS_TREE_ID;

import org.eclipse.swt.widgets.Composite;

import com.simulcraft.test.gui.access.CompositeAccess;
import com.simulcraft.test.gui.access.TreeAccess;
import com.simulcraft.test.gui.util.Predicate;

public class DatasetsAndChartsPageAccess extends CompositeAccess {

	public DatasetsAndChartsPageAccess(Composite composite) {
		super(composite);
	}

	public TreeAccess getDatasetsTree() {
		return (TreeAccess)createAccess(
				findDescendantControl(
						getControl(),
						Predicate.hasID(DATASETS_TREE_ID)));
	}
	
	public TreeAccess getChartsheetsTree() {
		return (TreeAccess)createAccess(
				findDescendantControl(
						getControl(),
						Predicate.hasID(CHARTSHEETS_TREE_ID)));
	}
}
