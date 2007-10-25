package org.omnetpp.test.gui.eventlogtable;

import org.omnetpp.test.gui.core.EventLogFileTestCase;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.ButtonAccess;
import com.simulcraft.test.gui.access.ShellAccess;

public class FilterEventLogTest
    extends EventLogFileTestCase
{
    public FilterEventLogTest() {
        super("stress.log");
    }
    
    @Override
    protected void setUpInternal() throws Exception {
        super.setUpInternal();
    }
    
    public void testNoFilter() throws Exception {
        openFileFromProjectExplorerViewInEventLogTableEditor();
        findToolItemWithToolTip("Event filter").activateDropDownMenu().activateMenuItemWithMouse("Filter...");
        
        ShellAccess shell = Access.findShellWithTitle("Filter event log");
        ButtonAccess button = shell.findButtonWithLabel("Filter for events.*selected modules");
        button.selectWithMouseClick();
        shell.findButtonWithLabel("OK").selectWithMouseClick();

        findToolItemWithToolTip("Event filter").activateDropDownMenu().activateMenuItemWithMouse("Show All");
    }

    public void _testFilterForModule() throws Exception {
        // TODO:
    }
}
