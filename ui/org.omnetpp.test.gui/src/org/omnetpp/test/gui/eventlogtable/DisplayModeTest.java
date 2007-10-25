package org.omnetpp.test.gui.eventlogtable;

import org.omnetpp.test.gui.core.EventLogFileTestCase;

public class DisplayModeTest
    extends EventLogFileTestCase
{
    protected void selectDisplayMode(String text) throws Exception {
        createFileWithTwoEvents();
        openFileFromProjectExplorerViewInEventLogTableEditor();
        
        findEditorPart().getToolBarManager().getToolBar().findToolItemWithToolTip("Display mode").
            activateDropDownMenu().activateMenuItemWithMouse(text);
    }

    public void testDescriptiveDisplayMode() throws Exception {
        selectDisplayMode("Descriptive");
    }

    public void testRawDisplayMode() throws Exception {
        selectDisplayMode("Raw");
    }
}
