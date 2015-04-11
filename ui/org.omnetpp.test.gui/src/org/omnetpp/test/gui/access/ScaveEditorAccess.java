/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.test.gui.access;

import org.eclipse.swt.SWT;
import org.eclipse.ui.part.MultiPageEditorPart;

import com.simulcraft.test.gui.access.CTabItemAccess;
import com.simulcraft.test.gui.access.MultiPageEditorPartAccess;
import com.simulcraft.test.gui.core.UIStep;

public class ScaveEditorAccess extends MultiPageEditorPartAccess {

    public ScaveEditorAccess(MultiPageEditorPart scaveEditor) {
        super(scaveEditor);
    }

    public InputsPageAccess ensureInputsPageActive() {
        return (InputsPageAccess)ensureActivePage("Inputs");
    }

    public BrowseDataPageAccess ensureBrowseDataPageActive() {
        return (BrowseDataPageAccess)ensureActivePage("Browse Data");
    }

    public DatasetsAndChartsPageAccess ensureDatasetsPageActive() {
        return (DatasetsAndChartsPageAccess)ensureActivePage("Datasets");
    }

    @UIStep
    public void closePage(String label) {
        CTabItemAccess item = getCTabItem(label);
        item.clickOnCloseIcon();
    }

    public void executeUndo() {
        assertActivated();
        pressKey('z', SWT.CTRL);
    }
}
