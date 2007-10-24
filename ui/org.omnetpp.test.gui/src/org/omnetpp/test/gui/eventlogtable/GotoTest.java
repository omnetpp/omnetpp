package org.omnetpp.test.gui.eventlogtable;

import org.eclipse.swt.SWT;
import org.omnetpp.test.gui.access.EventLogTableAccess;
import org.omnetpp.test.gui.core.EventLogFileTestCase;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.ShellAccess;

public class GotoTest
    extends EventLogFileTestCase 
{
    public GotoTest() {
        super("stress.log");
    }
    
    @Override
    protected void setUpInternal() throws Exception {
        super.setUpInternal();
        openFileFromProjectExplorerViewInEventLogTableEditor();
    }
    
    public void testGotoEventNumber() throws Exception {
        EventLogTableAccess eventLogTable = findEventLogTable();
        eventLogTable.gotoEventNumber(100);
        eventLogTable.assertSelectedEventNumber(100);
    }

    public void testGotoSimulationTime() throws Exception {
        EventLogTableAccess eventLogTable = findEventLogTable();
        eventLogTable.activateContextMenuWithMouseClick().activateMenuItemWithMouse("Goto simulation time.*");

        ShellAccess shell = Access.findShellWithTitle("Goto simulation time");
        shell.findTextAfterLabel("Please enter.*").typeIn(String.valueOf("10"));
        shell.findButtonWithLabel("OK").selectWithMouseClick();
        
        eventLogTable.assertSelectedEventNumber(860);
    }

    public void testGotoEventCause() throws Exception {
        EventLogTableAccess eventLogTable = findEventLogTable();
        eventLogTable.activateContextMenuWithMouseClick(500).activateMenuItemWithMouse("Goto event cause");
        eventLogTable.assertSelectedEventNumber(496);
    }

    public void testGotoMessageArrival() throws Exception {
        selectFilterMode("All");
        EventLogTableAccess eventLogTable = findEventLogTable();
        eventLogTable.gotoEventNumber(500);

        for (int i = 0; i < 4; i++)
            eventLogTable.pressKey(SWT.ARROW_DOWN);
        
        eventLogTable.activateContextMenuWithMouseClickAtSelectedElement().activateMenuItemWithMouse("Goto message arrival");
        eventLogTable.assertSelectedEventNumber(514);
    }

    public void testGotoMessageOrigin() throws Exception {
        EventLogTableAccess eventLogTable = findEventLogTable();
        eventLogTable.gotoEventNumber(676);

        for (int i = 0; i < 5; i++)
            eventLogTable.pressKey(SWT.ARROW_DOWN);
    
        eventLogTable.activateContextMenuWithMouseClickAtSelectedElement().activateMenuItemWithMouse("Goto message origin");
        eventLogTable.assertSelectedEventNumber(590);
    }

    public void testGotoMessageReuse() throws Exception {
        EventLogTableAccess eventLogTable = findEventLogTable();
        eventLogTable.activateContextMenuWithMouseClick(590).activateMenuItemWithMouse("Goto message reuse");
        eventLogTable.assertSelectedEventNumber(676);
    }
}
