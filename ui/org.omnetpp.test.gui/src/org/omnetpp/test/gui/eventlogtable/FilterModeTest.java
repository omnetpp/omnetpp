package org.omnetpp.test.gui.eventlogtable;

import org.omnetpp.test.gui.core.EventLogFileTestCase;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.ShellAccess;

public class FilterModeTest
    extends EventLogFileTestCase 
{
    protected void selectFilterMode(String text) throws Exception {
        createFileWithTwoEvents();
        openFileFromProjectExplorerViewInEventLogTableEditor();
        
        findEditorPart().getToolBarManager().getToolBar().findToolItemWithToolTip("Filter mode").
            activateDropDownMenu().activateMenuItemWithMouse(text);
    }

    public void testAllFilterMode() throws Exception {
        selectFilterMode("All");
    }

    public void testEventsMessageSendsAndLogMessagesFilterMode() throws Exception {
        selectFilterMode(".*sends.*");
    }

    public void testEventsAndLogMessagesFilterMode() throws Exception {
        selectFilterMode("Events and.*");
    }

    public void testEventsFilterMode() throws Exception {
        selectFilterMode("Events");
    }

    public void testCustomFilterMode() throws Exception {
        selectFilterMode("Custom.*");
        ShellAccess shell = Access.findShellWithTitle("Search.*");
        shell.findTextAfterLabel(".*enter.*").typeIn("ES");
        shell.findButtonWithLabel("OK").click();
    }
}
