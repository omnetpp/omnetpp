package org.omnetpp.experimental.simkernel.swig;

import org.omnetpp.experimental.simkernel.IEnvirCallback;



public class EnvirCallbackProxy {
	private IEnvirCallback callback;
	
	public EnvirCallbackProxy(IEnvirCallback cb) {
		callback = cb;
	}
	
    void objectDeleted(long cObject_object) {
    	callback.objectDeleted(new cObject(cObject_object, false));
    }

    void messageSent(long cMessage_msg, long cGate_directToGate) {
    	callback.messageSent(new cMessage(cMessage_msg, false), new cGate(cGate_directToGate, false));
    }

    void messageDelivered(long cMessage_msg) {
    	callback.messageDelivered(new cMessage(cMessage_msg, false));
    }

    void breakpointHit(String label, long cSimpleModule_mod) {
    	callback.breakpointHit(label, new cSimpleModule(cSimpleModule_mod, false));
    }

    void moduleMethodCalled(long cModule_from, long cModule_to, String method) {
    	callback.moduleMethodCalled(new cModule(cModule_from, false), new cModule(cModule_to, false), method);
    }
    
    void moduleCreated(long cModule_newmodule) {
    	callback.moduleCreated(new cModule(cModule_newmodule, false));
    }
    
    void moduleDeleted(long cModule_module) {
    	callback.moduleDeleted(new cModule(cModule_module, false));
    }

    void moduleReparented(long cModule_module, long cModule_oldparent) {
    	callback.moduleReparented(new cModule(cModule_module, false), new cModule(cModule_oldparent, false));
    }

    void connectionCreated(long cGate_srcgate) {
    	callback.connectionCreated(new cGate(cGate_srcgate, false));
    }

    void connectionRemoved(long cGate_srcgate) {
    	callback.connectionRemoved(new cGate(cGate_srcgate, false));
    }

    void gateDisplayStringChanged(long cGate_gate) {
    	callback.displayStringChanged(new cGate(cGate_gate, false));
    }

    void moduleDisplayStringChanged(long cModule_submodule) {
    	callback.displayStringChanged(new cModule(cModule_submodule, false));
    }

    void backgroundDisplayStringChanged(long cModule_parentmodule) {
    	callback.backgroundDisplayStringChanged(new cModule(cModule_parentmodule, false));
    }
}
