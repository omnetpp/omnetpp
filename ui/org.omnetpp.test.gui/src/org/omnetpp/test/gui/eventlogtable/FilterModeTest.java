package org.omnetpp.test.gui.eventlogtable;

import org.omnetpp.test.gui.core.EventLogFileTestCase;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.ShellAccess;

public class FilterModeTest
    extends EventLogFileTestCase 
{
    public void testAllFilterMode() throws Exception {
        testFilterMode("All");
    }

    public void testEventsMessageSendsAndLogMessagesFilterMode() throws Exception {
        testFilterMode(".*sends.*");
    }

    public void testEventsAndLogMessagesFilterMode() throws Exception {
        testFilterMode("Events and.*");
    }

    public void testEventsFilterMode() throws Exception {
        testFilterMode("Events");
    }

    public void testCustomFilterMode() throws Exception {
        testFilterMode("Custom.*");
        ShellAccess shell = Access.findShellWithTitle("Search.*");
        shell.findTextAfterLabel(".*enter.*").typeIn("ES");
        shell.findButtonWithLabel("OK").click();
    }

    protected void testFilterMode(String text) throws Exception {
        createFileWithTwoEvents();
        openFileFromProjectExplorerViewInEventLogTableEditor();
        selectFilterMode(text);
    }
}
