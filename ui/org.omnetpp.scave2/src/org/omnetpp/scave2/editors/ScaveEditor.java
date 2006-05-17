package org.omnetpp.scave2.editors;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.ui.part.MultiPageEditorPart;

public class ScaveEditor extends MultiPageEditorPart {
	
	/**
	 * The constructor.
	 */
	public ScaveEditor() {
	}

	protected void createPages() {
	}

	@Override
	protected void pageChange(int newPageIndex) {
		super.pageChange(newPageIndex);
	}

	@Override
	public void doSave(IProgressMonitor monitor) {
		// TODO Auto-generated method stub

	}
	@Override
	public void doSaveAs() {
		// TODO Auto-generated method stub

	}

	@Override
	public boolean isDirty() {
		// TODO Auto-generated method stub
		return false;
	}
	@Override
	public boolean isSaveAsAllowed() {
		// TODO Auto-generated method stub
		return false;
	}
}


