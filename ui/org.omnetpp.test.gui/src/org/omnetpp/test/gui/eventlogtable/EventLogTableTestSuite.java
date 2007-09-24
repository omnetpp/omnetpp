package org.omnetpp.test.gui.eventlogtable;

import junit.framework.Test;
import junit.framework.TestSuite;

public class EventLogTableTestSuite
    extends TestSuite
{
    public EventLogTableTestSuite() {
        addTestSuite(OpenFileTest.class);
    }

    public static Test suite() {
        return new EventLogTableTestSuite();
    }        
}
