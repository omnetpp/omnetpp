package org.omnetpp.test.gui.sequencechart;

import org.omnetpp.test.gui.core.EventLogFileTestCase;

public class OpenFileTest
    extends EventLogFileTestCase
{
    public void testOpenEmptyFile() throws Exception {
        createEmptyFile();
        openFileFromProjectExplorerViewInSequenceChartEditor();
    }
    
    public void testOpenFileWithOnEvent() throws Exception {
        createFileWithOneEvent();
        openFileFromProjectExplorerViewInSequenceChartEditor();
    }

    public void testOpenFileWithTwoEvents() throws Exception {
        createFileWithTwoEvents();
        openFileFromProjectExplorerViewInSequenceChartEditor();
    }
}
