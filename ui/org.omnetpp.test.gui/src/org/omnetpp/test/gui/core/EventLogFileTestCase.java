package org.omnetpp.test.gui.core;

import org.omnetpp.eventlogtable.widgets.EventLogTable;
import org.omnetpp.test.gui.access.EventLogTableAccess;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.util.WorkbenchUtils;


public class EventLogFileTestCase
    extends ProjectFileTestCase
{
    public EventLogFileTestCase() {
        this("test.log");
    }
    
    public EventLogFileTestCase(String fileName) {
        super(fileName);
    }
    
    protected void createFileWithOneEvent() throws Exception {
        createFileWithContent(
                "MC id 1 c cModule n module\r\n" + 
        		"BS id 1 tid 1 c cMessage n start\r\n" + 
        		"ES t 0\r\n" + 
        		"\r\n" + 
        		"E # 0 t 0 m 1 msg 1\r\n");
    }

    protected void createFileWithTwoEvents() throws Exception {
        createFileWithContent(
                "MC id 1 c cModule n module\r\n" + 
        		"BS id 1 tid 1 c cMessage n start\r\n" + 
        		"ES t 0\r\n" + 
        		"\r\n" + 
        		"E # 0 t 0 m 1 msg 1\r\n" + 
        		"BS id 2 tid 2 eid 2 etid 2 c cMessage n continue k 1 l 0\r\n" + 
        		"SH sm 1 sg 0\r\n" + 
        		"ES t 1\r\n" + 
        		"\r\n" + 
        		"E # 1 t 1 m 1 msg 2\r\n");
    }

    protected EventLogTableAccess findEventLogTable() {
        EventLogTableAccess eventLogTable = (EventLogTableAccess)Access.createAccess(Access.findDescendantControl(findEditorPart().getComposite().getControl(), EventLogTable.class));
        return eventLogTable;
    }

    protected void selectFilterMode(String text) {
        findEditorPart().getToolBarManager().getToolBar().findToolItemWithTooltip("Filter mode").
            activateDropDownMenu().activateMenuItemWithMouse(text);
    }

    protected void openFileFromProjectExplorerViewInEventLogTableEditor() {
        WorkbenchUtils.findInProjectExplorerView(filePath).reveal().activateContextMenuWithMouseClick().activateMenuItemWithMouse(".*Open With.*").activateMenuItemWithMouse(".*Event Log Table.*");
    }

    protected void openFileFromProjectExplorerViewInSequenceChartEditor() {
        WorkbenchUtils.findInProjectExplorerView(filePath).reveal().activateContextMenuWithMouseClick().activateMenuItemWithMouse(".*Open With.*").activateMenuItemWithMouse(".*Sequence Chart.*");
    }
}
