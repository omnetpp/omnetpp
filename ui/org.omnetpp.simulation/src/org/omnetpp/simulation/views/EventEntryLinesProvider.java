package org.omnetpp.simulation.views;

import org.eclipse.core.runtime.Assert;
import org.eclipse.swt.graphics.Color;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.simulation.controller.Anim;
import org.omnetpp.simulation.controller.EventEntry;

/**
 *
 * @author Andras
 */
public class EventEntryLinesProvider implements IEventEntryLinesProvider {

    @Override
    public int getNumLines(EventEntry entry) {
        int count = entry.isEvent() ? 1 /* the banner line */ : 0;
        for (Object item : entry.logItems)
            if (item instanceof String || item instanceof Anim.BeginSendEntry)
                count++;
        return count;
    }

    @Override
    public String getLineText(EventEntry entry, int lineIndex) {
        if (entry.isEvent()) {
            if (lineIndex == 0)
                return getBannerLine(entry);
            else
                lineIndex--;
        }
        for (Object item : entry.logItems) {
            if (item instanceof String || item instanceof Anim.BeginSendEntry)
                if (lineIndex-- == 0)
                    return getTextForLogItem(item);
        }
        throw new RuntimeException("log entry line index out of bounds");
    }

    @Override
    public Color getLineColor(EventEntry entry, int lineIndex) {
        if (entry.isEvent()) {
            if (lineIndex == 0)
                return ColorFactory.BLUE;
            else
                lineIndex--;
        }
        for (Object item : entry.logItems) {
            if (item instanceof String || item instanceof Anim.BeginSendEntry)
                if (lineIndex-- == 0)
                    return getColorForLogItem(item);
        }
        throw new RuntimeException("log entry line index out of bounds");
    }

    protected String getBannerLine(EventEntry entry) {
        Assert.isTrue(entry.isEvent());
        return "Event #" + entry.eventNumber + " t=" + entry.simulationTime.toString() + " at " + entry.moduleFullPath + " on " + entry.messageName + " (" + entry.messageClassName + ")\n";
    }

    protected String getTextForLogItem(Object item) {
        if (item instanceof String)
            return (String)item;
        else if (item instanceof Anim.BeginSendEntry) {
            Anim.BeginSendEntry entry = (Anim.BeginSendEntry)item;
            return "sends some message";  //TODO message name!!!
        }
        else
            return item.toString();
    }

    protected Color getColorForLogItem(Object item) {
        return item instanceof String ? null : ColorFactory.GREY50;
    }


}
