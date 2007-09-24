package org.omnetpp.test.gui.sequencechart;

import junit.framework.Test;
import junit.framework.TestSuite;

public class SequenceChartTestSuite
    extends TestSuite
{
    public SequenceChartTestSuite() {
        addTestSuite(OpenFileTest.class);
    }

    public static Test suite() {
        return new SequenceChartTestSuite();
    }        
}
