package org.omnetpp.scave.dialogs;

import org.eclipse.jface.preference.PreferencePage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;

public class LegendPreferencePage extends PreferencePage {

	public LegendPreferencePage(String title)
	{
		super(title);
	}
	
	@Override
	protected Control createContents(Composite parent) {
		return new LegendPreferencePanel(parent, SWT.NONE);
	}

}
