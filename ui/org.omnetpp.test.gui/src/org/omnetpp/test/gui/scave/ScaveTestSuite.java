package org.omnetpp.test.gui.scave;

import junit.framework.Test;
import junit.framework.TestSuite;

public class ScaveTestSuite
    extends TestSuite
{
    public ScaveTestSuite() {
        addTestSuite(OpenFileTest.class);
    }

    public static Test suite() {
        return new ScaveTestSuite();
    }        
}
