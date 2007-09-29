package org.omnetpp.test.gui.core;

import junit.framework.Test;
import junit.framework.TestSuite;

import org.omnetpp.test.gui.eventlogtable.EventLogTableTestSuite;
import org.omnetpp.test.gui.inifileeditor.InifileEditorTestSuite;
import org.omnetpp.test.gui.launcher.LauncherTestSuite;
import org.omnetpp.test.gui.nededitor.NedEditorTestSuite;
import org.omnetpp.test.gui.scave.ScaveTestSuite;
import org.omnetpp.test.gui.sequencechart.SequenceChartTestSuite;

public class IDETestSuite
	extends TestSuite
{
    public IDETestSuite() {
        addTest(NedEditorTestSuite.suite());
        addTest(InifileEditorTestSuite.suite());
        addTest(LauncherTestSuite.suite());
        addTest(EventLogTableTestSuite.suite());
        addTest(SequenceChartTestSuite.suite());
        addTest(ScaveTestSuite.suite());
    }

    public static Test suite() {
		return new IDETestSuite();
	}		 
}
