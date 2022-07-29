package org.omnetpp.sequencechart.widgets;

import java.util.ArrayList;
import java.util.Arrays;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.engineext.IMatchableObject;
import org.omnetpp.common.eventlog.EventLogInput;
import org.omnetpp.common.eventlog.ModuleTreeItem;
import org.omnetpp.common.util.Pair;
import org.omnetpp.common.util.BigDecimal;
import org.omnetpp.eventlog.FilteredMessageDependency;
import org.omnetpp.eventlog.IEvent;
import org.omnetpp.eventlog.IEventLog;
import org.omnetpp.eventlog.IMessageDependency;
import org.omnetpp.eventlog.MessageReuseDependency;
import org.omnetpp.eventlog.SequenceChartFacade;
import org.omnetpp.eventlog.entry.ComponentMethodBeginEntry;
import org.omnetpp.eventlog.entry.MessageDescriptionEntry;

public final class SequenceChartStyleProvider implements ISequenceChartStyleProvider {
    class MessageSend implements IMatchableObject {
        private MessageDescriptionEntry entry;

        public MessageSend(MessageDescriptionEntry entry) {
            this.entry = entry;
        }

        @Override
        public String getAsString() {
            return entry.getMessageName();
        }

        @Override
        public String getAsString(String name) {
            return entry.getAsString(name);
        }
    }

    class ComponentMethodCall implements IMatchableObject {
        private ComponentMethodBeginEntry entry;

        public ComponentMethodCall(ComponentMethodBeginEntry entry) {
            this.entry = entry;
        }

        @Override
        public String getAsString() {
            return entry.getMethodName();
        }

        @Override
        public String getAsString(String name) {
            return entry.getAsString(name);
        }
    }

    protected IEventLog eventLog;
    protected EventLogInput eventLogInput;
    protected SequenceChartFacade sequenceChartFacade;
    protected SequenceChartSettings sequenceChartSettings;
    protected MatchExpressionArrayFallback<ModuleTreeItem, Color> axesColorFallback = new MatchExpressionArrayFallback<ModuleTreeItem, Color>(1000, ColorFactory.BLACK);
    protected MatchExpressionArrayFallback<ModuleTreeItem, Color> axesHeaderColorFallback = new MatchExpressionArrayFallback<ModuleTreeItem, Color>(1000, ColorFactory.LIGHT_CYAN);
    protected MatchExpressionArrayFallback<IEvent, Color> eventColorFallback = new MatchExpressionArrayFallback<IEvent, Color>(1000, ColorFactory.RED2);
    protected MatchExpressionArrayFallback<IEvent, Color> selfMessageEventColorFallback = new MatchExpressionArrayFallback<IEvent, Color>(1000, ColorFactory.GREEN2);
    protected MatchExpressionArrayFallback<MessageSend, Color> messageSendColorFallback = new MatchExpressionArrayFallback<MessageSend, Color>(1000, ColorFactory.BLUE);
    protected MatchExpressionArrayFallback<ComponentMethodCall, Color> componentMethodCallColorFallback = new MatchExpressionArrayFallback<ComponentMethodCall, Color>(1000, ColorFactory.ORANGE4);

    @Override
    public final void setEventLogInput(EventLogInput newEventLogInput) {
        if (newEventLogInput == null) {
            eventLog = null;
            eventLogInput = null;
            sequenceChartFacade = null;
        }
        else {
            eventLog = newEventLogInput.getEventLog();
            eventLogInput = newEventLogInput;
            sequenceChartFacade = newEventLogInput.getSequenceChartFacade();
        }
    }

    public void setSequenceChartSettings(SequenceChartSettings sequenceChartSettings) {
        this.sequenceChartSettings = sequenceChartSettings;
        if (sequenceChartSettings.axesColorFallback != null)
            axesColorFallback.setFallback(getColorFallback(sequenceChartSettings.axesColorFallback));
        if (sequenceChartSettings.axesHeaderColorFallback != null)
            axesHeaderColorFallback.setFallback(getColorFallback(sequenceChartSettings.axesHeaderColorFallback));
        if (sequenceChartSettings.eventColorFallback != null)
            eventColorFallback.setFallback(getColorFallback(sequenceChartSettings.eventColorFallback));
        if (sequenceChartSettings.selfMessageEventColorFallback != null)
            selfMessageEventColorFallback.setFallback(getColorFallback(sequenceChartSettings.selfMessageEventColorFallback));
        if (sequenceChartSettings.messageSendColorFallback != null)
            messageSendColorFallback.setFallback(getColorFallback(sequenceChartSettings.messageSendColorFallback));
        if (sequenceChartSettings.componentMethodCallColorFallback != null)
            componentMethodCallColorFallback.setFallback(getColorFallback(sequenceChartSettings.componentMethodCallColorFallback));
    }

    public SequenceChartSettings getSequenceChartSettings() {
        return sequenceChartSettings;
    }

    @Override
    public final Color getBackgroundColor() {
        return ColorFactory.WHITE;
    }

    @Override
    public final Color getSelectionColor() {
        return ColorFactory.RED;
    }

    @Override
    public final Color getHighlightColor() {
        return ColorFactory.DARK_RED;
    }

    @Override
    public final Color getBookmarkColor() {
        return ColorFactory.GREEN3;
    }

    @Override
    public final Color getZeroSimulationTimeRegionColor() {
        return ColorFactory.GREY94;
    }

    @Override
    public final Color getLongArrowheadColor() {
        return ColorFactory.WHITE;
    }

    @Override
    public final Color getInfoBackgroundColor() {
        return ColorFactory.LIGHT_CYAN;
    }

    @Override
    public final Color getInfoLabelColor() {
        return ColorFactory.BLACK;
    }

    @Override
    public final Color getGutterBackgroundColor() {
        return new Color(255, 255, 160);
    }

    @Override
    public final Color getGutterBorderColor() {
        return ColorFactory.BLACK;
    }

    @Override
    public final Color getTickLineColor() {
        return ColorFactory.DARK_GREY;
    }

    @Override
    public final Color getMouseTickLineColor() {
        return ColorFactory.BLACK;
    }

    @Override
    public Color getAxisHeaderColor(ModuleTreeItem axisModule) {
        return axesHeaderColorFallback.resolveArrayFallback(axisModule);
    }

    @Override
    public Color getAxisColor(ModuleTreeItem axisModule) {
        return axesColorFallback.resolveArrayFallback(axisModule);
    }

    @Override
    public final Font getAxisLabelFont(ModuleTreeItem axisModule) {
        return null;
    }

    @Override
    public final Color getAxisLabelColor(ModuleTreeItem axisModule) {
        return axesColorFallback.resolveArrayFallback(axisModule);
    }

    @Override
    public final Font getTickLabelFont(BigDecimal tick) {
        return null;
    }

    @Override
    public final Color getTickLabelColor(BigDecimal tick) {
        return ColorFactory.BLACK;
    }

    @Override
    public final Font getEventLabelFont(IEvent event) {
        return null;
    }

    @Override
    public final Color getEventLabelColor(IEvent event) {
        return getEventFillColor(event);
    }

    @Override
    public final Color getEventStrokeColor(IEvent event) {
        Color color = getEventFillColor(event);
        if (color.equals(ColorFactory.RED2))
            return ColorFactory.RED4;
        else if (color.equals(ColorFactory.GREEN2))
            return ColorFactory.GREEN4;
        else
            return ColorFactory.getChangedHsb(color, 1.0f, 1.0f, 0.7f);
    }

    @Override
    public final Color getEventFillColor(IEvent event) {
        if (isInitializationEvent(event))
            return ColorFactory.WHITE;
        else if (event.isSelfMessageProcessingEvent())
            return selfMessageEventColorFallback.resolveArrayFallback(event);
        else
            return eventColorFallback.resolveArrayFallback(event);
    }

    @Override
    public final Font getMessageDependencyLabelFont(IMessageDependency messageDependency) {
        return null;
    }

    @Override
    public final Color getMessageDependencyColor(IMessageDependency messageDependency) {
        Color messageSendColor = ColorFactory.BLUE;
        Color messageReuseColor = ColorFactory.GREEN4;

        if (messageDependency instanceof FilteredMessageDependency) {
            switch (((FilteredMessageDependency) messageDependency).getKind()) {
                case SENDS:
                    return messageSendColor;
                case REUSES:
                    return messageReuseColor;
                case MIXED:
                    return ColorFactory.CYAN4;
                default:
                    throw new RuntimeException("Unknown kind");
            }
        }
        else {
            if (messageDependency instanceof MessageReuseDependency)
                return messageReuseColor;
            else
                return messageSendColorFallback.resolveArrayFallback(new MessageSend(messageDependency.getBeginMessageDescriptionEntry()));
        }
    }

    @Override
    public final int getMessageDependencyLineStyle(IMessageDependency messageDependency) {
        if (messageDependency instanceof FilteredMessageDependency) {
            switch (((FilteredMessageDependency) messageDependency).getKind()) {
                case SENDS:
                    return SWT.LINE_SOLID;
                case REUSES:
                    return SWT.LINE_CUSTOM;
                case MIXED:
                    return SWT.LINE_CUSTOM;
                default:
                    throw new RuntimeException("Unknown kind");
            }
        }
        else {
            if (messageDependency instanceof MessageReuseDependency)
                return SWT.LINE_CUSTOM;
            else
                return SWT.LINE_SOLID;
        }
    }

    @Override
    public final int[] getMessageDependencyLineDash(IMessageDependency messageDependency) {
        int[] dottedLinePattern = new int[] {2, 2}; // 2px black, 2px gap
        int[] dashedLinePattern = new int[] {4, 4}; // 4px black, 4px gap

        if (messageDependency instanceof FilteredMessageDependency) {
            switch (((FilteredMessageDependency) messageDependency).getKind()) {
                case SENDS:
                    return null;
                case REUSES:
                    return dottedLinePattern; // SWT.LINE_DOT style is not what we want
                case MIXED:
                    return dashedLinePattern;
                default:
                    throw new RuntimeException("Unknown kind");
            }
        }
        else {
            if (messageDependency instanceof MessageReuseDependency)
                return dottedLinePattern; // SWT.LINE_DOT style is not what we want
            else
                return null;
        }
    }

    @Override
    public final Color getComponentMethodCallColor(ComponentMethodBeginEntry componentMethodCall) {
        return componentMethodCallColorFallback.resolveArrayFallback(new ComponentMethodCall(componentMethodCall));
    }

    @Override
    public final int getComponentMethodCallLineStyle(ComponentMethodBeginEntry componentMethodCall) {
        return SWT.LINE_SOLID;
    }

    @Override
    public final int[] getComponentMethodCallLineDash(ComponentMethodBeginEntry componentMethodCall) {
        return null;
    }

    @Override
    public final int getComponentMethodCallReturnLineStyle(ComponentMethodBeginEntry componentMethodCall) {
        return SWT.LINE_DOT;
    }

    @Override
    public final int[] getComponentMethodCallReturnLineDash(ComponentMethodBeginEntry componentMethodCall) {
        return null;
    }

    @Override
    public final int getTextSpacing() {
        return 2;
    }

    @Override
    public final int getTickSpacing() {
        return 100;
    }

    @Override
    public final int getAxisOffset() {
        return 20;
    }

    @Override
    public final int getArrowheadLength() {
        return 10;
    }

    @Override
    public final int getArrowheadWidth() {
        return 7;
    }

    @Override
    public final int getMinimumHalfEllipseHeight() {
        return 15;
    }

    @Override
    public final int getLongMessageArrowWidth() {
        return 80;
    }

    @Override
    public final int getEventRadius() {
        return 3;
    }

    @Override
    public final int getEventSelectionRadius() {
        return 10;
    }

    private boolean isInitializationEvent(IEvent event) {
        return event.getEventNumber() == 0;
    }

    @SuppressWarnings("unchecked")
    private Pair<String, Color[]>[] getColorFallback(Pair<String, String[]>[] colorSettings) {
        Pair<String, Color[]>[] fallback = new Pair[colorSettings.length];
        for (int i = 0; i < colorSettings.length; i++) {
            Pair<String, String[]> element = colorSettings[i];
            fallback[i] = new Pair<String, Color[]>(null, null);
            fallback[i].first = element.first;
            String[] colorNames = element.second;
            ArrayList<Color> colors = new ArrayList<Color>();
            for (int j = 0; j < colorNames.length; j++) {
                String colorName = colorNames[j];
                if ("light".equals(colorName))
                    colors.addAll(Arrays.asList(ColorFactory.getGoodLightColors()));
                else if ("dark".equals(colorName))
                    colors.addAll(Arrays.asList(ColorFactory.getGoodDarkColors()));
                else {
                    Color color = ColorFactory.asColor(colorName);
                    if (color != null)
                        colors.add(color);
                }
            }
            fallback[i].second = colors.toArray(new Color[0]);
        }
        return fallback;
    }
}
