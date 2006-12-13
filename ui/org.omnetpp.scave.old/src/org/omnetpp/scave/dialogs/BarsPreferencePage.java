package org.omnetpp.scave.dialogs;

import org.eclipse.jface.preference.PreferencePage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;

public class BarsPreferencePage extends PreferencePage {

	public BarsPreferencePage(String title)
	{
		super(title);
	}
	
	@Override
	protected Control createContents(Composite parent) {
		return new BarsPreferencePanel(parent, SWT.NONE);
	}

}
