package org.omnetpp.test.gui.sequencechart;

import org.omnetpp.test.gui.core.EventLogFileTestCase;

public class AxisSpacingTest
    extends EventLogFileTestCase
{
    public AxisSpacingTest() {
        super("stress.log");
    }
    
    @Override
    protected void setUpInternal() throws Exception {
        super.setUpInternal();
        openFileFromProjectExplorerViewInSequenceChartEditor();
    }

    public void testIncreaseSpacing() throws Exception {
        findToolItemWithToolTip("Increase Spacing").click();
    }

    public void testDecreaseSpacing() throws Exception {
        findToolItemWithToolTip("Decrease Spacing").click();
    }
}
