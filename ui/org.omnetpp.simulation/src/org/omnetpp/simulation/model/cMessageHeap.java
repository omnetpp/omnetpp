package org.omnetpp.simulation.model;

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
        messages = new cMessage[messagesList.size()];
        for (int i = 0; i < messages.length; i++)
            messages[i] = (cMessage) getSimulation().getObjectByJSONRef((String) messagesList.get(i));
    }

    @Override
    protected void clearReferences() {
        messages = null;
    }
}
