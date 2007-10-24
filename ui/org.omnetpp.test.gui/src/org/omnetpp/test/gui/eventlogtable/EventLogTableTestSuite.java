package org.omnetpp.test.gui.eventlogtable;

import junit.framework.Test;
import junit.framework.TestSuite;

public class EventLogTableTestSuite
    extends TestSuite
{
    public EventLogTableTestSuite() {
        addTestSuite(OpenFileTest.class);
        addTestSuite(MoveAroundTest.class);
        addTestSuite(GotoTest.class);
        addTestSuite(SearchTest.class);
        addTestSuite(BookmarkTest.class);
        addTestSuite(DisplayModeTest.class);
        addTestSuite(FilterModeTest.class);
        addTestSuite(FilterEventLogTest.class);
    }

    public static Test suite() {
        return new EventLogTableTestSuite();
    }        
}
