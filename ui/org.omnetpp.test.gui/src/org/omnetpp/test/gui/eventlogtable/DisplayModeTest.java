/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.test.gui.eventlogtable;

import org.omnetpp.test.gui.core.EventLogFileTestCase;

public class DisplayModeTest
    extends EventLogFileTestCase
{
    protected void selectDisplayMode(String text) throws Exception {
        createFileWithTwoEvents();
        openFileFromProjectExplorerViewInEventLogTableEditor();

        findEditorPart().getToolBarManager().getToolBar().findToolItemWithTooltip("Display mode").
            activateDropDownMenu().activateMenuItemWithMouse(text);
    }

    public void testDescriptiveDisplayMode() throws Exception {
        selectDisplayMode("Descriptive");
    }

    public void testRawDisplayMode() throws Exception {
        selectDisplayMode("Raw");
    }
}
