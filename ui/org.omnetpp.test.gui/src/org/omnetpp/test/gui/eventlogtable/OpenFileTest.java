package org.omnetpp.test.gui.eventlogtable;

import org.omnetpp.test.gui.core.EventLogFileTestCase;

import com.simulcraft.test.gui.util.WorkbenchUtils;

public class OpenFileTest
    extends EventLogFileTestCase
{
    public void testOpenEmptyFile() throws Exception {
        createEmptyFile();
        openFileFromProjectExplorerView();
    }
    
    public void testOpenFileWithOnEvent() throws Exception {
        createFileWithOneEvent();
        openFileFromProjectExplorerView();
    }

    public void testOpenFileWithTwoEvents() throws Exception {
        createFileWithTwoEvents();
        openFileFromProjectExplorerView();
    }

    protected void openFileFromProjectExplorerView() {
        WorkbenchUtils.findInProjectExplorerView(filePath).reveal().activateContextMenuWithMouseClick().activateMenuItemWithMouse(".*Open With.*").activateMenuItemWithMouse(".*Event Log Table.*");
    }
}
