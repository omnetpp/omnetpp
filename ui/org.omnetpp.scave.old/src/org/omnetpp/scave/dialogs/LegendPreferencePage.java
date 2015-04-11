/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

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
