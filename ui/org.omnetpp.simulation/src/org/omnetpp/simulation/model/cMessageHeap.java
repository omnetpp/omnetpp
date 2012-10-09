package org.omnetpp.simulation.model;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.omnetpp.simulation.controller.Simulation;

/**
 *
 * @author Andras
 */
public class cMessageHeap extends cObject {
    private cMessage[] messages;

    public cMessageHeap(Simulation simulation, long id) {
        super(simulation, id);
    }

    public cMessage[] getMessages() {
        checkState();
        return messages;
    }

    public boolean isEmpty() {
        checkState();
        return messages.length == 0;
    }

    @Override
    @SuppressWarnings("rawtypes")
    protected void doFillFromJSON(Map jsonObject) {
        super.doFillFromJSON(jsonObject);

        List messagesList = (List) jsonObject.get("messages");
//FIXME: put this simpler code back, once cEvent is introduced
//        messages = new cMessage[messagesList.size()];
//        for (int i = 0; i < messages.length; i++)
//            messages[i] = (cMessage) getSimulation().getObjectByJSONRef((String) messagesList.get(i));

        //FIXME for now just leave out cEvents
        List<cMessage> list = new ArrayList<cMessage>();
        for (Object msgRef : messagesList) {
            Object msg = getSimulation().getObjectByJSONRef((String) msgRef);
            if (msg instanceof cMessage)
                list.add((cMessage)msg);
        }
        messages = list.toArray(new cMessage[]{});
    }

    @Override
    protected void clearReferences() {
        messages = null;
    }
}
