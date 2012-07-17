package org.omnetpp.simulation.controller;

import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.simulation.model.cGate;
import org.omnetpp.simulation.model.cMessage;

public class Anim {  //TODO temp class; make inner classes toplevel

    public static class Entry {
    }
    
    public static class ComponentMethodBeginEntry extends Entry {
        public String txt;
        public int srcModuleId;
        public int destModuleId; // cannot be cModule because module may get deleted while we still hold on to this entry
        
        @Override
        public String toString() {
            return "call " + txt;
        }
    }

    public static class ComponentMethodEndEntry extends Entry {
        @Override
        public String toString() {
            return "ret";
        }
    }

    public static class BeginSendEntry extends Entry {
        public cMessage msg;
        
        @Override
        public String toString() {
            return "beginsend";
        }
    }

    public static class MessageSendDirectEntry extends Entry {
        public cMessage msg; //XXX redundant
        //TODO fromModule
        public cGate destGate; 
        public BigDecimal propagationDelay;  // optional
        public BigDecimal transmissionDelay; // optional

        @Override
        public String toString() {
            return "senddirect";
        }
    }

    public static class MessageSendHopEntry extends Entry {
        public cMessage msg; //XXX redundant
        public cGate srcGate; 
        public BigDecimal propagationDelay;  // optional 
        public BigDecimal transmissionDelay; // optional

        @Override
        public String toString() {
            return "sendhop";
        }
    }

    public static class EndSendEntry extends Entry {
        public cMessage msg; //XXX redundant

        @Override
        public String toString() {
            return "endsend";
        }
    }
}
