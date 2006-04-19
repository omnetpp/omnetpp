package org.omnetpp.scave.dialogs;

import org.eclipse.jface.preference.PreferencePage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;

public class AxesPreferencePage extends PreferencePage {
	
	public AxesPreferencePage(String title)
	{
		super(title);
	}

	@Override
	protected Control createContents(Composite parent) {
		return new AxesPreferencePanel(parent, SWT.NONE);
	}
}
