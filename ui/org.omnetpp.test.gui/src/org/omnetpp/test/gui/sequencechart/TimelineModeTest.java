/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.test.gui.sequencechart;

import org.omnetpp.test.gui.core.EventLogFileTestCase;

public class TimelineModeTest
    extends EventLogFileTestCase
{
    public TimelineModeTest() {
        super("stress.log");
    }

    @Override
    protected void setUpInternal() throws Exception {
        super.setUpInternal();
        openFileFromProjectExplorerViewInSequenceChartEditor();
    }

    public void testLinear() throws Exception {
        findToolItemWithToolTip("Timeline Mode").activateDropDownMenu().activateMenuItemWithMouse("Linear");
    }

    // TODO: other
}
