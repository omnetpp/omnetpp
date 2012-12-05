package org.omnetpp.simulation.controller;

import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.simulation.model.cGate;
import org.omnetpp.simulation.model.cMessage;
import org.omnetpp.simulation.model.cModule;

public class Anim {  //TODO temp class; make inner classes toplevel

    public static class Entry {
    }

    public static class ComponentMethodBeginEntry extends Entry {
        public String txt;
        // note: cannot be cModule because module may get deleted while we still hold on to this entry
        public int srcModuleId; //XXX many be 0, see Cmdenv!
        public int destModuleId; //XXX may be 0, see Cmdenv! 

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
        public cModule srcModule;
        public cGate destGate;
        public BigDecimal propagationDelay;  // optional
        public BigDecimal transmissionDelay; // optional

        @Override
        public String toString() {
            return "senddirect";
        }
    }

    public static class MessageSendHopEntry extends Entry {
        public cGate srcGate;
        public BigDecimal propagationDelay;  // optional
        public BigDecimal transmissionDelay; // optional

        @Override
        public String toString() {
            return "sendhop";
        }
    }

    public static class EndSendEntry extends Entry {
        @Override
        public String toString() {
            return "endsend";
        }
    }
}
