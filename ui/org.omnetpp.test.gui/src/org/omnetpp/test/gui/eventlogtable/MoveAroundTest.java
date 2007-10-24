package org.omnetpp.test.gui.eventlogtable;

import org.eclipse.swt.SWT;
import org.omnetpp.test.gui.access.EventLogTableAccess;
import org.omnetpp.test.gui.core.EventLogFileTestCase;

public class MoveAroundTest
    extends EventLogFileTestCase
{
    public MoveAroundTest() {
        super("stress.log");
    }
    
    // TODO: scrolling with the wheel
    // TODO: scrolling with the scroll bar
    // TODO: extremely small and large files

    public void testHome() throws Exception {
        EventLogTableAccess eventLogTable = prepareTest();
        eventLogTable.pressKey(SWT.HOME);
        eventLogTable.assertTopVisibleEventNumber(0);
    }

    public void testEnd() throws Exception {
        EventLogTableAccess eventLogTable = prepareTest();
        eventLogTable.pressKey(SWT.END);
        eventLogTable.assertBottomFullyVisibleEventNumber(1215);
    }

    public void testArrowUp() throws Exception {
        EventLogTableAccess eventLogTable = prepareTest();
        eventLogTable.pressKey(SWT.HOME);
        eventLogTable.pressKey(SWT.ARROW_UP);
        eventLogTable.assertTopVisibleEventNumber(0);
        eventLogTable.pressKey(SWT.END);
        eventLogTable.pressKey(SWT.ARROW_UP);
        eventLogTable.assertSelectedEventNumber(1214);
    }

    public void testArrowDown() throws Exception {
        EventLogTableAccess eventLogTable = prepareTest();
        eventLogTable.pressKey(SWT.END);
        eventLogTable.pressKey(SWT.ARROW_DOWN);
        eventLogTable.assertBottomFullyVisibleEventNumber(1215);
        eventLogTable.pressKey(SWT.HOME);
        eventLogTable.pressKey(SWT.ARROW_DOWN);
        eventLogTable.assertSelectedEventNumber(1);
    }

    public void testPageUp() throws Exception {
        EventLogTableAccess eventLogTable = prepareTest();
        eventLogTable.pressKey(SWT.HOME);
        eventLogTable.pressKey(SWT.PAGE_UP);
        eventLogTable.assertTopVisibleEventNumber(0);
        eventLogTable.pressKey(SWT.END);
        eventLogTable.pressKey(SWT.PAGE_UP);
        eventLogTable.assertSelectedEventNumber(1215 - eventLogTable.getPageJumpCount());
    }

    public void testPageDown() throws Exception {
        EventLogTableAccess eventLogTable = prepareTest();
        eventLogTable.pressKey(SWT.END);
        eventLogTable.pressKey(SWT.PAGE_DOWN);
        eventLogTable.assertBottomFullyVisibleEventNumber(1215);
        eventLogTable.pressKey(SWT.HOME);
        eventLogTable.pressKey(SWT.PAGE_DOWN);
        eventLogTable.assertSelectedEventNumber(eventLogTable.getPageJumpCount());
    }
    
    protected EventLogTableAccess prepareTest() {
        openFileFromProjectExplorerViewInEventLogTableEditor();
        selectFilterMode("Events");
        return findEventLogTable();
    }
}
