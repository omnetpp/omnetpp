package org.omnetpp.test.gui.sequencechart;

import org.eclipse.core.runtime.Assert;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Point;
import org.omnetpp.common.util.ReflectionUtils;
import org.omnetpp.eventlog.engine.IEvent;
import org.omnetpp.sequencechart.widgets.SequenceChart;
import org.omnetpp.test.gui.access.SequenceChartAccess;
import org.omnetpp.test.gui.core.EventLogFileTestCase;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.core.UIStep;

public class MoveAroundTest
    extends EventLogFileTestCase
{
    public MoveAroundTest() {
        super("stress.log");
    }

    @Override
    protected void setUpInternal() throws Exception {
        super.setUpInternal();
        openFileFromProjectExplorerViewInSequenceChartEditor();
    }
    
    public void testMoveFromBeginToEndUsingArrowKeys() {
        moveTroughUsingArrowKeys(true);
    }

    public void testMoveFromEndToBeginUsingArrowKeys() {
        moveTroughUsingArrowKeys(false);
    }

    public void moveTroughUsingArrowKeys(boolean forward) {
        SequenceChartAccess sequenceChart = findSequenceChart();
        SequenceChart control = sequenceChart.getControl();

        sequenceChart.pressKey(forward ? SWT.HOME : SWT.END);
        assertSelectionEvent(sequenceChart);
        
        setKeyboardTypeDelay(0);

        IEvent limit = forward ? control.getEventLog().getLastEvent() : control.getEventLog().getFirstEvent();
        while (control.getSelectionEvent().getEventNumber() != limit.getEventNumber())
        {
            assertPaintHasBeenFinishedWithoutErrors(sequenceChart);
            sequenceChart.pressKey(forward ? SWT.ARROW_RIGHT : SWT.ARROW_LEFT);
        }
    }
    
    public void testMoveFromBeginToEndUsingMouse() {
        moveThroughUsingMouse(true);
    }

    public void testMoveFromEndToBeginUsingMouse() {
        moveThroughUsingMouse(false);
    }
    
    public void moveThroughUsingMouse(boolean forward) {
        SequenceChartAccess sequenceChart = findSequenceChart();
        SequenceChart control = sequenceChart.getControl();

        sequenceChart.pressKey(forward ? SWT.HOME : SWT.END);
        assertSelectionEvent(sequenceChart);

        setMouseMoveDuration(0);
        setMouseClickDuration(0);
        
        IEvent limit = forward ? control.getEventLog().getLastEvent() : control.getEventLog().getFirstEvent();
        while (!control.getViewportCenterSimulationTime().equals(limit.getSimulationTime()))
        {
            assertPaintHasBeenFinishedWithoutErrors(sequenceChart);
            dragOnePageHorizontally(sequenceChart, forward);
        }
    }

    @UIStep
    private void dragOnePageHorizontally(SequenceChartAccess sequenceChart, boolean forward) {
        SequenceChart control = sequenceChart.getControl();
        Point size = control.getSize();
        int y = size.y / 2;
        Point p1 = control.toDisplay(size.x - 5, y);
        Point p2 = control.toDisplay(5, y);
        sequenceChart.dragMouseAbsolute(Access.LEFT_MOUSE_BUTTON, forward ? p1 : p2, forward ? p2 : p1);
    }

    private void assertPaintHasBeenFinishedWithoutErrors(SequenceChartAccess sequenceChart) {
        SequenceChart control = sequenceChart.getControl();
        Assert.isTrue(!(Boolean)ReflectionUtils.getFieldValue(control, "internalErrorHappenedDuringPaint"));
        Assert.isTrue((Boolean)ReflectionUtils.getFieldValue(control, "paintHasBeenFinished"));
    }
    
    @UIStep
    private void assertSelectionEvent(SequenceChartAccess sequenceChart) {
        Assert.isTrue(sequenceChart.getControl().getSelectionEvent() != null);
    }
}
