package org.omnetpp.sequencechart.widgets;

import java.io.Serializable;

import org.eclipse.swt.graphics.Color;
import org.omnetpp.common.util.Pair;
import org.omnetpp.eventlog.TimelineMode;

/**
 * Used to persistently store the sequence chart settings.
 */
class SequenceChartSettings implements Serializable {

    /**
     * Used to persistently store the per axis sequence chart settings.
     */
    public static class AxisState implements Serializable {
        private static final long serialVersionUID = 1L;

        // identifies the axis module
        public String moduleFullPath;

        // identifies the vector
        public String vectorFileName;
        public String vectorRunName;
        public String vectorModuleFullPath;
        public String vectorName;
    }

    private static final long serialVersionUID = 1L;
    public int viewportTop;
    public long fixPointEventNumber;
    public long fixPointViewportCoordinate;
    public double pixelPerTimelineCoordinate;
    public AxisState[] axisStates;
    public TimelineMode timelineMode;
    public double axisSpacing;
    public SequenceChart.AxisSpacingMode axisSpacingMode;
    public SequenceChart.AxisOrderingMode axisOrderingMode;
    public String[] moduleFullPathesManualAxisOrder;
    public Boolean showArrowHeads;
    public Boolean showAxes;
    public Boolean showAxisHeaders;
    public Boolean showAxisInfo;
    public Boolean showAxisLabels;
    public Boolean showAxisVectorData;
    public Boolean showComponentMethodCalls;
    public Boolean showEmptyAxes;
    public Boolean showEventLogInfo;
    public Boolean showEventMarks;
    public Boolean showEventNumbers;
    public Boolean showHairlines;
    public Boolean showInitializationEvent;
    public Boolean showMessageNames;
    public Boolean showMessageSends;
    public Boolean showMessageReuses;
    public Boolean showMethodNames;
    public Boolean showMixedMessageDependencies;
    public Boolean showMixedSelfMessageDependencies;
    public Boolean showPositionAndRange;
    public Boolean showSelfMessageSends;
    public Boolean showSelfMessageReuses;
    public Boolean showTimeDifferences;
    public Boolean showTransmissionDurations;
    public Boolean showZeroSimulationTimeRegions;
    public String fontName;
    public int fontHeight;
    public Pair<String, String[]>[] axesColorFallback;
    public Pair<String, String[]>[] axesHeaderColorFallback;
    public Pair<String, String[]>[] eventColorFallback;
    public Pair<String, String[]>[] selfMessageEventColorFallback;
    public Pair<String, String[]>[] messageSendColorFallback;
    public Pair<String, String[]>[] componentMethodCallColorFallback;
    public Boolean enableColoring;
    public Boolean enableAxesColoring;
    public Boolean enableAxesHeaderColoring;
    public Boolean enableEventColoring;
    public Boolean enableSelfMessageEventColoring;
    public Boolean enableMessageSendColoring;
    public Boolean enableComponentMethodCallColoring;
}
