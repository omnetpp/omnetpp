package org.omnetpp.simulation.ui;

import java.util.ArrayList;
import java.util.List;

import org.apache.commons.lang3.ObjectUtils;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.GC;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.util.IPredicate;
import org.omnetpp.simulation.controller.CommunicationException;
import org.omnetpp.simulation.controller.Simulation;
import org.omnetpp.simulation.model.cMessage;
import org.omnetpp.simulation.model.cMessageHeap;

/**
 * An timeline content and label provider that uses cMessageHeap as input. Messages can be filtered.
 *
 * @author Andras
 */
public class TimelineContentProvider implements ITimelineContentProvider, ITimelineLabelProvider {
    protected Simulation simulation;
    protected IPredicate filter;
    protected boolean colorByMessageKind = true;

    protected Color defaultMessageFillColor = ColorFactory.RED;
    protected Color defaultMessageBorderColor = ColorFactory.RED4;

    protected Color[] msgKindFillColors = new Color[] {
            ColorFactory.RED, ColorFactory.GREEN, ColorFactory.BLUE, ColorFactory.WHITE,
            ColorFactory.YELLOW, ColorFactory.CYAN, ColorFactory.MAGENTA, ColorFactory.BLACK
    };
    protected Color[] msgKindBorderColors = new Color[] {
            ColorFactory.RED3, ColorFactory.GREEN3, ColorFactory.BLUE3, ColorFactory.GREY80,
            ColorFactory.YELLOW3, ColorFactory.CYAN3, ColorFactory.MAGENTA3, ColorFactory.BLACK
    };

    public TimelineContentProvider(Simulation simulation) {
        this.simulation = simulation;
    }

    public void setFilter(IPredicate filter) {
        if (!ObjectUtils.equals(this.filter, filter)) {
            this.filter = filter;
        }
    }

    public IPredicate getFilter() {
        return filter;
    }

    @Override
    public BigDecimal getBaseTime() {
        return simulation.getNextEventSimulationTimeGuess();
    }

    @Override
    public Object[] getMessages() {
        try {
            if (!simulation.hasRootObjects())
                return new Object[0];
            cMessageHeap fes = (cMessageHeap)simulation.getRootObject(Simulation.ROOTOBJ_FES);
            fes.loadIfUnfilled();
            cMessage[] messages = fes.getMessages();
            simulation.loadUnfilledObjects(messages);

            if (filter == null)
                return messages;
            else {
                List<cMessage> list = new ArrayList<cMessage>();
                for (cMessage msg : messages)
                    if (filter.matches(msg))
                        list.add(msg);
                return list.toArray();
            }
        }
        catch (CommunicationException e) {
            return new Object[0];
        }
    }

    @Override
    public BigDecimal getMessageTime(Object message) {
        return ((cMessage)message).getArrivalTime();
    }

    @Override
    public String getMessageLabel(Object message) {
        return ((cMessage)message).getName(); //XXX and/or class name, or something (to be made customizable)
    }

    @Override
    public void drawMessageSymbol(GC gc, Object message, boolean active, int x, int y) {
        if (colorByMessageKind) {
            int colorIndex = ((cMessage)message).getKind() & 0x07; // not "x % 8" because it yields negative numbers for negative msgkind!
            gc.setBackground(msgKindFillColors[colorIndex]);
            gc.setForeground(msgKindBorderColors[colorIndex]);
        }
        else {
            gc.setBackground(defaultMessageFillColor);
            gc.setForeground(defaultMessageBorderColor);
        }
        gc.setAlpha(active ? 255 : 40);
        gc.fillOval(x-2, y-4, 5, 9);
        gc.drawOval(x-2, y-4, 5, 9);
    }

}
