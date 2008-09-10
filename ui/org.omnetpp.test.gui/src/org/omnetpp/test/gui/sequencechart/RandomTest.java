package org.omnetpp.test.gui.sequencechart;

import java.math.BigDecimal;
import java.util.Random;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.common.eventlog.EventLogFilterParameters;
import org.omnetpp.common.eventlog.EventLogInput;
import org.omnetpp.common.util.NullGraphics;
import org.omnetpp.eventlog.engine.FilteredEventLog;
import org.omnetpp.eventlog.engine.IEvent;
import org.omnetpp.eventlog.engine.IEventLog;
import org.omnetpp.eventlog.engine.ModuleCreatedEntryList;
import org.omnetpp.eventlog.engine.PStringVector;
import org.omnetpp.sequencechart.widgets.SequenceChart;
import org.omnetpp.sequencechart.widgets.SequenceChart.AxisOrderingMode;
import org.omnetpp.sequencechart.widgets.SequenceChart.TimelineMode;
import org.omnetpp.test.gui.core.EventLogFileTestCase;

import com.simulcraft.test.gui.core.UIStep;

public class RandomTest
    extends EventLogFileTestCase
{
    private static long SECOND = 1000;
    private static long MINUTE = SECOND * 60;
    private static long HOUR = MINUTE * 60;

    private Random random = new Random(0);
    
    private SequenceChart control;

    public RandomTest() {
        super("stress.log");
    }

    @Override
    protected void setUpInternal() throws Exception {
        super.setUpInternal();
        openFileFromProjectExplorerViewInSequenceChartEditor();
    }
    
    public void testRandom() {
        control = findSequenceChart().getControl();
        initialize();

        long count = 0;
        long begin = System.currentTimeMillis();

        while ((System.currentTimeMillis() - begin) < 1 * MINUTE) {
            System.out.println("At " + count);
            count++;

            // move
            if (random.nextDouble() < 0.9)
                doRandomMove();

            // zoom
            if (random.nextDouble() < 0.2)
                doRandomZoom();

            // zoom
            if (random.nextDouble() < 0.1)
                doRandomAxisOrdering();

            // filter
            if (random.nextDouble() < 0.2)
                setRandomFilter();
            
            // timeline mode
            if (random.nextDouble() < 0.1)
                changeToRandomTimelineMode();

            paint();
        }
    }

    @UIStep
    private void initialize() {
        control.gotoBegin();
        control.defaultZoom();
    }

    @UIStep
    private void paint() {
        Graphics graphics = new NullGraphics();
        graphics.setClip(new Rectangle(0, 0, 1000, 1000));
        control.paintArea(graphics);
    }

    @UIStep
    private void doRandomMove() {
        doRandomOperation(new Object[] {
            0.3, new Runnable() {
                    public void run() {
                        System.out.println("Scrolling right");
                        control.scrollHorizontal(1000);
                    }},
            0.3, new Runnable() {
                    public void run() {
                        System.out.println("Scrolling left");
                        control.scrollHorizontal(-1000);
                    }},
            0.3, new Runnable() {
                    public void run() {
                        if (!control.getEventLog().isEmpty()) {
                            IEvent event = control.getEventLog().getApproximateEventAt(random.nextDouble());
                            System.out.println("Jumping to event #" + event.getEventNumber());
                            control.gotoElement(event);
                        }
                    }},
            0.05, new Runnable() {
                    public void run() {
                        System.out.println("Jumping to begin");
                        control.gotoBegin();
                    }},
            0.05, new Runnable() {
                    public void run() {
                        System.out.println("Jumping to end");
                        control.gotoEnd();
                    }},
            });
    }

    @UIStep
    private void doRandomZoom() {
        doRandomOperation(new Object[] {
            0.45, new Runnable() {
                    public void run() {
                        System.out.println("Zooming in");
                        control.zoomIn();
                    }},
            0.45, new Runnable() {
                    public void run() {
                        System.out.println("Zooming out");
                        control.zoomOut();
                    }},
            0.1, new Runnable() {
                    public void run() {
                        System.out.println("Setting zoom level to default");
                        control.defaultZoom(); 
                    }}
            });
    }

    @UIStep
    private void doRandomAxisOrdering() {
        AxisOrderingMode axisOrderingMode = AxisOrderingMode.values()[random.nextInt(AxisOrderingMode.values().length)];
        System.out.println("Changing ordering mode to " + axisOrderingMode.name());
        control.setAxisOrderingMode(axisOrderingMode);
    }

    @UIStep
    private void setRandomFilter() {
        EventLogInput eventLogInput = control.getInput();

        if (random.nextDouble() < 0.5 && control.getEventLog() instanceof FilteredEventLog) {
            System.out.println("Removing filter");
            eventLogInput.removeFilter();
        }
        else {
            System.out.print("Changing filter to ");
            
            IEventLog eventLog = control.getEventLog();
            if (eventLog instanceof FilteredEventLog)
                eventLog = ((FilteredEventLog)eventLog).getEventLog();

            int count;
            IEvent lastEvent = eventLog.getLastEvent();
            int lastEventNumber = (int)lastEvent.getEventNumber();
            int lastMessageId = lastEvent.getMessageId();
            double lastSimulationTime = lastEvent.getSimulationTime().doubleValue();
            EventLogFilterParameters filterParameters = eventLogInput.getFilterParameters();
            
            // range filter
            filterParameters.enableRangeFilter = random.nextBoolean();
            
            if (filterParameters.enableRangeFilter) {
                boolean enable = random.nextBoolean();

                // event number range
                filterParameters.enableEventNumberFilter = enable;
                filterParameters.lowerEventNumberLimit = random.nextInt(lastEventNumber);
                filterParameters.upperEventNumberLimit = filterParameters.lowerEventNumberLimit + random.nextInt(lastEventNumber - (int)filterParameters.lowerEventNumberLimit);

                // simulation time range
                filterParameters.enableEventNumberFilter = !enable;
                filterParameters.lowerSimulationTimeLimit = new BigDecimal(random.nextDouble() * lastSimulationTime);
                filterParameters.upperSimulationTimeLimit = filterParameters.lowerSimulationTimeLimit.add(new BigDecimal(random.nextDouble() * (lastSimulationTime - filterParameters.lowerSimulationTimeLimit.doubleValue())));
            }

            // module filter
            filterParameters.enableModuleFilter = random.nextBoolean();
            
            if (filterParameters.enableModuleFilter) {
                boolean enable = random.nextBoolean();

                // module id filter
                filterParameters.enableModuleIdFilter = enable;
                count = random.nextInt(eventLog.getNumModuleCreatedEntries()) + 1;
                filterParameters.moduleIds = new int[count];
                for (int i = 0; i < count; i++)
                    filterParameters.moduleIds[i] = random.nextInt(eventLog.getNumModuleCreatedEntries());
                
                // module type filter
                filterParameters.enableModuleNEDTypeNameFilter = !enable;
                count = random.nextInt(5) + 1;
                filterParameters.moduleNEDTypeNames = new String[count];
                for (int i = 0; i < count; i++) {
                    ModuleCreatedEntryList moduleCreatedEntries = eventLog.getModuleCreatedEntries();
                    int index = random.nextInt((int)moduleCreatedEntries.size());
                    filterParameters.moduleNEDTypeNames[i] = moduleCreatedEntries.get(index).getNedTypeName();
                }
            }

            // message filter
            filterParameters.enableMessageFilter = random.nextBoolean();

            if (filterParameters.enableMessageFilter) {
                boolean enable = random.nextBoolean();

                // message encapsulation tree id
                filterParameters.enableMessageEncapsulationTreeIdFilter = enable;
                count = random.nextInt(100) + 1;
                filterParameters.messageEncapsulationTreeIds = new EventLogFilterParameters.EnabledInt[count];
                for (int i = 0; i < count; i++)
                    filterParameters.messageEncapsulationTreeIds[i] = new EventLogFilterParameters.EnabledInt(true, random.nextInt(lastMessageId));
                
                // message class name filter
                filterParameters.enableMessageClassNameFilter = !enable;
                PStringVector names = eventLog.getMessageClassNames().keys();
                count = random.nextInt((int)names.size()) + 1;
                filterParameters.messageClassNames = new String[count];
                for (int i = 0; i < count; i++)
                    filterParameters.messageClassNames[i] = names.get(random.nextInt((int)names.size()));
            }

            System.out.println(filterParameters.toString());
            eventLogInput.filter();
        }
    }

    @UIStep
    private void changeToRandomTimelineMode() {
        TimelineMode timelineMode = TimelineMode.values()[random.nextInt(TimelineMode.values().length)];
        System.out.println("Changing timeline mode to " + timelineMode.name());
        control.setTimelineMode(timelineMode);
    }

    private void doRandomOperation(Object[] parameters) {
        Runnable operation = null;
        double total = 0;
        double variable = random.nextDouble();
        
        for (int i = 0; i < parameters.length; i += 2) {
            double probability = (Double)parameters[i];
            
            if (total <= variable && variable <= total + probability)
                operation = (Runnable)parameters[i + 1];

            if (i % 2 == 0)
                total += probability;
        }
        
        Assert.isTrue(total == 1.0);
        operation.run();
    }
}
