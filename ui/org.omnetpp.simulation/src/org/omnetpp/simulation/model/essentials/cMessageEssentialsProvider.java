package org.omnetpp.simulation.model.essentials;

import java.util.ArrayList;
import java.util.List;

import org.omnetpp.simulation.model.cMessage;
import org.omnetpp.simulation.model.cObject;
import org.omnetpp.simulation.ui.IEssentialsProvider;

import static org.omnetpp.simulation.model.FieldNames.*;

/**
 *
 * @author Andras
 */
public class cMessageEssentialsProvider implements IEssentialsProvider {

    @Override
    public boolean supports(cObject object) {
        return object instanceof cMessage; //XXX handle cPackets as well? or not?
    }

    @Override
    public int getScore(cObject object) {
        return 10;
    }

    public boolean hasChildren(cObject object) {
        return true;
    }

    @Override
    public Object[] getChildren(cObject object) {
        //TODO add paramList and controlInfo, if present!!! also custom fields added via inheritance
        cMessage msg = (cMessage) object;
        List<Object> result = new ArrayList<Object>();
        result.add(object.getField(FLD_COBJECT_FULLNAME));
        result.add(object.getField(FLD_CMESSAGE_KIND));
        if (msg.isSelfMessage()) {
            if (msg.isScheduled()) {
                result.add("scheduled self-message");
                result.add(object.getField(FLD_CMESSAGE_ARRIVALTIME));
                result.add(object.getField(FLD_CMESSAGE_ARRIVALMODULE));
            }
            else {
                result.add("inactive self-message");
            }
        }
        else {
            //TODO if encapsulated: "message encapsulated in ...."
            if (msg.isScheduled()) {
                result.add("message in transit"); //TODO zero delay or not
                result.add(object.getField(FLD_CMESSAGE_SENDERMODULE));
                result.add(object.getField(FLD_CMESSAGE_ARRIVALMODULE));
                if (!msg.getSendingTime().equals(msg.getArrivalTime()))
                    result.add(object.getField(FLD_CMESSAGE_SENDINGTIME));
                result.add(object.getField(FLD_CMESSAGE_ARRIVALTIME));
                //TODO print propagation delay explicitly?
            }
            else {
                if (msg.getSenderModule() == null)
                    result.add("new message");
            }

        }
        return result.toArray();
    }

}
