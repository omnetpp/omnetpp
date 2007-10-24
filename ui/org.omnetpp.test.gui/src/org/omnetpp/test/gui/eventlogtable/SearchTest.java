package org.omnetpp.test.gui.eventlogtable;

import org.eclipse.swt.SWT;
import org.omnetpp.test.gui.access.EventLogTableAccess;
import org.omnetpp.test.gui.core.EventLogFileTestCase;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.ShellAccess;

public class SearchTest
    extends EventLogFileTestCase
{
    public SearchTest() {
        super("stress.log");
    }
    
    @Override
    protected void setUpInternal() throws Exception {
        super.setUpInternal();
        openFileFromProjectExplorerViewInEventLogTableEditor();
    }
    
    public void testSearchNotFound() throws Exception {
        EventLogTableAccess eventLogTable = findEventLogTable();
        searchFor(eventLogTable, "Csenge");
        Access.findShellWithTitle("Search raw text").findButtonWithLabel("OK").selectWithMouseClick();
        eventLogTable.assertTopVisibleEventNumber(0);
    }

    public void testSearchFound() throws Exception {
        EventLogTableAccess eventLogTable = findEventLogTable();
        searchFor(eventLogTable, "Immediately");
        eventLogTable.assertTopVisibleEventNumber(23);
    }
    
    protected void searchFor(EventLogTableAccess eventLogTable, String text) {
        eventLogTable.pressKey(SWT.HOME);
        eventLogTable.activateContextMenuWithMouseClick(0).findMenuItemByLabel("Search.*").activateWithMouseClick();

        ShellAccess shell = Access.findShellWithTitle("Search raw text");
        shell.findTextAfterLabel("Please enter the search text").typeIn(text);
        shell.findButtonWithLabel("OK").selectWithMouseClick();
    }
}
