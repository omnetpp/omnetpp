package org.omnetpp.test.unit.common;

import junit.framework.Test;
import junit.framework.TestSuite;

public class AllStringUtilsTests {

    public static Test suite() {
        TestSuite suite = new TestSuite("Test for org.omnetpp.test.unit.common");
        //$JUnit-BEGIN$
        suite.addTestSuite(StringUtils_ToInstanceName.class);
        //$JUnit-END$
        return suite;
    }

}
