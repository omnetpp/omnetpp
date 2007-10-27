package org.omnetpp.test.gui.access;

import org.eclipse.swt.SWT;
import org.eclipse.ui.part.MultiPageEditorPart;

import com.simulcraft.test.gui.access.CTabItemAccess;
import com.simulcraft.test.gui.access.MultiPageEditorPartAccess;
import com.simulcraft.test.gui.core.InUIThread;

public class ScaveEditorAccess extends MultiPageEditorPartAccess {

	public ScaveEditorAccess(MultiPageEditorPart scaveEditor) {
		super(scaveEditor);
	}
	
	public InputsPageAccess ensureInputsPageActive() {
		return (InputsPageAccess)ensureActivePage("Inputs");
	}
	
	public BrowseDataPageAccess ensureBrowseDataPageActive() {
		return (BrowseDataPageAccess)ensureActivePage("Browse data");
	}
	
	public DatasetsAndChartsPageAccess ensureDatasetsPageActive() {
		return (DatasetsAndChartsPageAccess)ensureActivePage("Datasets");
	}
	
	@InUIThread
	public void closePage(String label) {
		CTabItemAccess item = getCTabItem(label);
		item.clickOnCloseIcon();
	}
	
	public void executeUndo() {
		assertActivated();
        pressKey('z', SWT.CTRL);
	}
}
