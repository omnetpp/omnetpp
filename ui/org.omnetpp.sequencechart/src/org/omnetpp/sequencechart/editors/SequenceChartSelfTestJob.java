package org.omnetpp.sequencechart.editors;

import java.util.ArrayList;

import org.eclipse.core.runtime.ILog;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.util.BigDecimal;
import org.omnetpp.eventlog.IEvent;
import org.omnetpp.eventlog.TimelineMode;
import org.omnetpp.sequencechart.SequenceChartPlugin;
import org.omnetpp.sequencechart.widgets.SequenceChart;
import org.omnetpp.sequencechart.widgets.SequenceChart.AxisOrderingMode;

public class SequenceChartSelfTestJob extends Job {
    private SequenceChart sequenceChart;
    private RuntimeException exception;
    private ArrayList<TestCase> testCases = new ArrayList<TestCase>();

    public SequenceChartSelfTestJob(SequenceChart sequenceChart) {
        super("Sequence Chart Self Test");
        this.sequenceChart = sequenceChart;
        addAllTests();
    }

    @Override
    protected IStatus run(IProgressMonitor monitor) {
        ILog log = SequenceChartPlugin.getDefault().getLog();
        for (TestCase testCase : testCases) {
            long startTime = System.currentTimeMillis();
            try {
                // run test from the UI thread and wait for completion (including paint)
                log.info("SequenceChart testing started, test case = " + testCase.title);
                Display.getDefault().syncExec(() -> {
                    testCase.runnable.run();
                    while (Display.getCurrent().readAndDispatch())
                        ; // finish painting
                });
                long elapsedTime = System.currentTimeMillis() - startTime;
                log.info("SequenceChart testing finished, test case = " + testCase.title + ", duration = " + elapsedTime + "ms");
                if (elapsedTime > 3000)
                    exception = new RuntimeException("Test case run too long: " + elapsedTime + "ms");
                else if (sequenceChart.getInput().isCanceled())
                    exception = new RuntimeException("Test case canceled");
                else
                    exception = sequenceChart.getInternalError();
                // TODO: check number of read bytes
                // TODO: check memory consumption
            }
            catch (RuntimeException e) {
                exception = e;
            }
            if (exception != null)
                return new Status(Status.ERROR, SequenceChartPlugin.PLUGIN_ID, "Test case: " + testCase.title, exception);
        }
        return new Status(Status.OK, SequenceChartPlugin.PLUGIN_ID, "OK");
    }

    private void addAllTests() {
//        addNavigationTests();
//        addTimelineModeTests();
//        addAxisOrderingModeTests();
//        addShowTests();
    }

    private void addShowTests() {
        addTest("Hide all content", () -> sequenceChart.setShowAll(false));
        addNavigationTests();
        addTest("Show all content", () -> sequenceChart.setShowAll(true));
        addNavigationTests();
        // TODO: add show default, show network communcation
    }

    private void addTimelineModeTests() {
        for (TimelineMode timelineMode : TimelineMode.values()) {
            addTest("Set timeline mode", () -> sequenceChart.setTimelineMode(timelineMode));
            addNavigationTests();
        }
    }

    private void addAxisOrderingModeTests() {
        for (AxisOrderingMode axisOrderingMode : AxisOrderingMode.values()) {
            addTest("Set axis ordering mode", () -> sequenceChart.setAxisOrderingMode(axisOrderingMode));
            addNavigationTests();
        }
    }

    private void addNavigationTests() {
        addTest("Goto begin", () -> sequenceChart.gotoBegin());
        addTest("Goto end", () -> sequenceChart.gotoEnd());
        addTest("Goto first event", () -> {
            IEvent firstEvent = sequenceChart.getEventLog().getFirstEvent();
            if (firstEvent != null)
                sequenceChart.gotoElement(firstEvent);
        });
        addTest("Goto last event", () -> {
            IEvent lastEvent = sequenceChart.getEventLog().getLastEvent();
            if (lastEvent != null)
                sequenceChart.gotoElement(lastEvent);
        });
        addTest("Goto 0 simulation time", () -> sequenceChart.gotoSimulationTime(BigDecimal.ZERO));
        addTest("Goto first simulation time", () -> {
            BigDecimal simulationTime = sequenceChart.getEventLog().getFirstSimulationTime();
            if (simulationTime.greaterOrEqual(BigDecimal.ZERO))
                sequenceChart.gotoSimulationTime(simulationTime);
        });
        addTest("Goto last simulation time", () -> {
            BigDecimal simulationTime = sequenceChart.getEventLog().getLastSimulationTime();
            if (simulationTime.greaterOrEqual(BigDecimal.ZERO))
                sequenceChart.gotoSimulationTime(simulationTime);
        });
        // TODO: more
    }

    private void addTest(String title, Runnable runnable) {
        testCases.add(new TestCase(title, runnable));
    }

    private class TestCase
    {
        public String title;
        public Runnable runnable;

        public TestCase(String title, Runnable runnable) {
            this.title = title;
            this.runnable = runnable;
        }
    }
}
