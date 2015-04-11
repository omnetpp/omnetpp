/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.test.gui.eventlogtable;

import org.omnetpp.test.gui.core.EventLogFileTestCase;

public class OpenFileTest
    extends EventLogFileTestCase
{
    public void testOpenEmptyFile() throws Exception {
        createEmptyFile();
        openFileFromProjectExplorerViewInEventLogTableEditor();
    }

    public void testOpenFileWithOnEvent() throws Exception {
        createFileWithOneEvent();
        openFileFromProjectExplorerViewInEventLogTableEditor();
    }

    public void testOpenFileWithTwoEvents() throws Exception {
        createFileWithTwoEvents();
        openFileFromProjectExplorerViewInEventLogTableEditor();
    }
}
