/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.test.gui.sequencechart;

import org.omnetpp.test.gui.core.EventLogFileTestCase;

public class ZoomTest
    extends EventLogFileTestCase
{
    public ZoomTest() {
        super("stress.log");
    }

    @Override
    protected void setUpInternal() throws Exception {
        super.setUpInternal();
        openFileFromProjectExplorerViewInSequenceChartEditor();
    }

    public void testZoomIn() throws Exception {
        findToolItemWithToolTip("Zoom In").click();
    }

    public void testZoomOut() throws Exception {
        findToolItemWithToolTip("Zoom Out").click();
    }
}
