package org.omnetpp.test.gui.core;

import junit.framework.Test;
import junit.framework.TestSuite;

import org.omnetpp.test.gui.eventlogtable.EventLogTableTestSuite;
import org.omnetpp.test.gui.inifileeditor.IniFileEditorTestSuite;
import org.omnetpp.test.gui.nededitor.NedEditorTestSuite;
import org.omnetpp.test.gui.sequencechart.SequenceChartTestSuite;

public class IDETestSuite
	extends TestSuite
{
    public IDETestSuite() {
        addTest(IniFileEditorTestSuite.suite());
        addTest(NedEditorTestSuite.suite());
        addTest(EventLogTableTestSuite.suite());
        addTest(SequenceChartTestSuite.suite());
    }

    public static Test suite() {
		return new IDETestSuite();
	}		 
}
