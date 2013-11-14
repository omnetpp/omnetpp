package org.omnetpp.simulation.model;

import org.apache.commons.lang3.ArrayUtils;
import org.omnetpp.simulation.controller.CommunicationException;
import org.omnetpp.simulation.controller.Simulation;

/**
 *
 * @author Andras
 */
public class FieldNames {
    // to refresh this list use code at bottom
    public static final String FLD_COBJECT_CLASSNAME = "className";
    public static final String FLD_COBJECT_FULLNAME = "fullName";
    public static final String FLD_COBJECT_FULLPATH = "fullPath";
    public static final String FLD_COBJECT_INFO = "info";
    public static final String FLD_COBJECT_DETAILEDINFO = "detailedInfo";
    public static final String FLD_CNAMEDOBJECT_NAME = "name";
    public static final String FLD_COWNEDOBJECT_OWNER = "owner";
    public static final String FLD_CDEFAULTLIST_DEFAULTLIST = "defaultList";
    public static final String FLD_CDISPLAYSTRING_VALUE = "value";
    public static final String FLD_CPROPERTY_ISIMPLICIT = "isImplicit";
    public static final String FLD_CPROPERTY_INDEX = "index";
    public static final String FLD_CPROPERTY_VALUE = "value";
    public static final String FLD_CPROPERTIES_CONTENTS = "contents";
    public static final String FLD_CCOMPONENT_ISMODULE = "isModule";
    public static final String FLD_CCOMPONENT_COMPONENTTYPE = "componentType";
    public static final String FLD_CCOMPONENT_PAR = "par";
    public static final String FLD_CCOMPONENT_PROPS = "props";
    public static final String FLD_CCHANNEL_DISPLAYSTRING = "displayString";
    public static final String FLD_CDATARATECHANNEL_ISBUSY = "isBusy";
    public static final String FLD_CDATARATECHANNEL_TRANSMISSIONFINISHTIME = "transmissionFinishTime";
    public static final String FLD_CMODULE_ID = "id";
    public static final String FLD_CMODULE_ISSIMPLE = "isSimple";
    public static final String FLD_CMODULE_ISVECTOR = "isVector";
    public static final String FLD_CMODULE_INDEX = "index";
    public static final String FLD_CMODULE_SIZE = "size";
    public static final String FLD_CMODULE_DISPLAYSTRING = "displayString";
    public static final String FLD_CMODULE_GATE = "gate";
    public static final String FLD_CSIMPLEMODULE_ISTERMINATED = "isTerminated";
    public static final String FLD_CSIMPLEMODULE_USESACTIVITY = "usesActivity";
    public static final String FLD_CSIMPLEMODULE_STACKSIZE = "stackSize";
    public static final String FLD_CSIMPLEMODULE_STACKUSAGE = "stackUsage";
    public static final String FLD_CMODULETYPE_ISNETWORK = "isNetwork";
    public static final String FLD_CREGISTRATIONLIST_CONTENTS = "contents";
    public static final String FLD_CGATE_TYPE = "type";
    public static final String FLD_CGATE_ID = "id";
    public static final String FLD_CGATE_ISVECTOR = "isVector";
    public static final String FLD_CGATE_INDEX = "index";
    public static final String FLD_CGATE_DELIVERONRECEPTIONSTART = "deliverOnReceptionStart";
    public static final String FLD_CGATE_CHANNEL = "channel";
    public static final String FLD_CGATE_PREVIOUSGATE = "previousGate";
    public static final String FLD_CGATE_NEXTGATE = "nextGate";
    public static final String FLD_CGATE_PATHSTARTGATE = "pathStartGate";
    public static final String FLD_CGATE_PATHENDGATE = "pathEndGate";
    public static final String FLD_CGATE_ISCONNECTEDOUTSIDE = "isConnectedOutside";
    public static final String FLD_CGATE_ISCONNECTEDINSIDE = "isConnectedInside";
    public static final String FLD_CGATE_ISCONNECTED = "isConnected";
    public static final String FLD_CGATE_ISPATHOK = "isPathOK";
    public static final String FLD_CARRAY_TAKEOWNERSHIP = "takeOwnership";
    public static final String FLD_CARRAY_CONTENTS = "contents";
    public static final String FLD_CMESSAGE_ID = "id";
    public static final String FLD_CMESSAGE_TREEID = "treeId";
    public static final String FLD_CMESSAGE_KIND = "kind";
    public static final String FLD_CMESSAGE_SCHEDULINGPRIORITY = "schedulingPriority";
    public static final String FLD_CMESSAGE_DISPLAYSTRING = "displayString";
    public static final String FLD_CMESSAGE_CONTROLINFO = "controlInfo";
    public static final String FLD_CMESSAGE_PARLIST = "parList";
    public static final String FLD_CMESSAGE_ISSCHEDULED = "isScheduled";
    public static final String FLD_CMESSAGE_ISSELFMESSAGE = "isSelfMessage";
    public static final String FLD_CMESSAGE_CREATIONTIME = "creationTime";
    public static final String FLD_CMESSAGE_SENDINGTIME = "sendingTime";
    public static final String FLD_CMESSAGE_ARRIVALTIME = "arrivalTime";
    public static final String FLD_CMESSAGE_TIMESTAMP = "timestamp";
    public static final String FLD_CMESSAGE_SENDERMODULEID = "senderModuleId";
    public static final String FLD_CMESSAGE_SENDERGATEID = "senderGateId";
    public static final String FLD_CMESSAGE_SENDERMODULE = "senderModule";
    public static final String FLD_CMESSAGE_SENDERGATE = "senderGate";
    public static final String FLD_CMESSAGE_ARRIVALMODULEID = "arrivalModuleId";
    public static final String FLD_CMESSAGE_ARRIVALGATEID = "arrivalGateId";
    public static final String FLD_CMESSAGE_ARRIVALMODULE = "arrivalModule";
    public static final String FLD_CMESSAGE_ARRIVALGATE = "arrivalGate";
    public static final String FLD_CPACKET_BITLENGTH = "bitLength";
    public static final String FLD_CPACKET_BYTELENGTH = "byteLength";
    public static final String FLD_CPACKET_HASBITERROR = "hasBitError";
    public static final String FLD_CPACKET_ENCAPSULATEDPACKET = "encapsulatedPacket";
    public static final String FLD_CPACKET_ISRECEPTIONSTART = "isReceptionStart";
    public static final String FLD_CPACKET_DURATION = "duration";
    public static final String FLD_CSTATISTIC_COUNT = "count";
    public static final String FLD_CSTATISTIC_WEIGHTS = "weights";
    public static final String FLD_CSTATISTIC_SUM = "sum";
    public static final String FLD_CSTATISTIC_SQRSUM = "sqrSum";
    public static final String FLD_CSTATISTIC_MIN = "min";
    public static final String FLD_CSTATISTIC_MAX = "max";
    public static final String FLD_CSTATISTIC_MEAN = "mean";
    public static final String FLD_CSTATISTIC_STDDEV = "stddev";
    public static final String FLD_CSTATISTIC_VARIANCE = "variance";
    public static final String FLD_CDENSITYESTBASE_ISTRANSFORMED = "isTransformed";
    public static final String FLD_CDENSITYESTBASE_UNDERFLOWCELL = "underflowCell";
    public static final String FLD_CDENSITYESTBASE_OVERFLOWCELL = "overflowCell";
    public static final String FLD_CDENSITYESTBASE_NUMCELLS = "numCells";
    public static final String FLD_CDENSITYESTBASE_CELLINFO = "cellInfo";
    public static final String FLD_CEXPRESSION_STR = "str";
    public static final String FLD_CFSM_STATE = "state";
    public static final String FLD_CFSM_STATENAME = "stateName";
    public static final String FLD_CNEDMATHFUNCTION_CATEGORY = "category";
    public static final String FLD_CNEDMATHFUNCTION_DESCRIPTION = "description";
    public static final String FLD_CNEDMATHFUNCTION_NUMARGS = "numArgs";
    public static final String FLD_CNEDFUNCTION_SIGNATURE = "signature";
    public static final String FLD_CNEDFUNCTION_CATEGORY = "category";
    public static final String FLD_CNEDFUNCTION_DESCRIPTION = "description";
    public static final String FLD_CNEDFUNCTION_RETURNTYPE = "returnType";
    public static final String FLD_CNEDFUNCTION_ARGTYPE = "argType";
    public static final String FLD_CNEDFUNCTION_MINARGS = "minArgs";
    public static final String FLD_CNEDFUNCTION_MAXARGS = "maxArgs";
    public static final String FLD_COUTVECTOR_ISENABLED = "isEnabled";
    public static final String FLD_COUTVECTOR_VALUESRECEIVED = "valuesReceived";
    public static final String FLD_COUTVECTOR_VALUESSTORED = "valuesStored";
    public static final String FLD_CPAR_VALUE = "value";
    public static final String FLD_CPAR_TYPE = "type";
    public static final String FLD_CPAR_UNIT = "unit";
    public static final String FLD_CPAR_ISNUMERIC = "isNumeric";
    public static final String FLD_CPAR_ISVOLATILE = "isVolatile";
    public static final String FLD_CPAR_ISEXPRESSION = "isExpression";
    public static final String FLD_CPAR_ISSHARED = "isShared";
    public static final String FLD_CMSGPAR_VALUE = "value";
    public static final String FLD_CMSGPAR_TYPE = "type";
    public static final String FLD_CMSGPAR_ISNUMERIC = "isNumeric";
    public static final String FLD_CMSGPAR_ISCONSTANT = "isConstant";
    public static final String FLD_CLINKEDLIST_LENGTH = "length";
    public static final String FLD_CMESSAGEHEAP_LENGTH = "length";
    public static final String FLD_CMESSAGEHEAP_MESSAGES = "messages";
    public static final String FLD_CQUEUE_TAKEOWNERSHIP = "takeOwnership";
    public static final String FLD_CQUEUE_LENGTH = "length";
    public static final String FLD_CQUEUE_CONTENTS = "contents";
    public static final String FLD_CPACKETQUEUE_BITLENGTH = "bitLength";
    public static final String FLD_CPACKETQUEUE_BYTELENGTH = "byteLength";
    public static final String FLD_CHASHER_HASH = "hash";
    public static final String FLD_CSIMULATION_SIMTIME = "simTime";
    public static final String FLD_CSIMULATION_EVENTNUMBER = "eventNumber";
    public static final String FLD_CSIMULATION_MESSAGEQUEUE = "messageQueue";
    public static final String FLD_CSIMULATION_LASTMODULEID = "lastModuleId";
    public static final String FLD_CSIMULATION_SYSTEMMODULE = "systemModule";
    public static final String FLD_CSIMULATION_MODULE = "module";
    public static final String FLD_CSIMULATION_HASHER = "hasher";
    public static final String FLD_CTOPOLOGY_NODE_MODULEID = "moduleId";
    public static final String FLD_CTOPOLOGY_NODE_MODULE = "module";
    public static final String FLD_CTOPOLOGY_NODE_WEIGHT = "weight";
    public static final String FLD_CTOPOLOGY_NODE_ENABLED = "enabled";
    public static final String FLD_CTOPOLOGY_NODE_INLINK = "inLink";
    public static final String FLD_CTOPOLOGY_NODE_OUTLINK = "outLink";
    public static final String FLD_CTOPOLOGY_NODE_DISTANCETOTARGET = "distanceToTarget";
    public static final String FLD_CTOPOLOGY_NODE_PATH = "path";
    public static final String FLD_CTOPOLOGY_LINK_WEIGHT = "weight";
    public static final String FLD_CTOPOLOGY_LINK_ENABLED = "enabled";
    public static final String FLD_CTOPOLOGY_LINKIN_REMOTENODE = "remoteNode";
    public static final String FLD_CTOPOLOGY_LINKIN_REMOTEGATE = "remoteGate";
    public static final String FLD_CTOPOLOGY_LINKIN_LOCALGATE = "localGate";
    public static final String FLD_CTOPOLOGY_LINKOUT_REMOTENODE = "remoteNode";
    public static final String FLD_CTOPOLOGY_LINKOUT_REMOTEGATE = "remoteGate";
    public static final String FLD_CTOPOLOGY_LINKOUT_LOCALGATE = "localGate";
    public static final String FLD_CTOPOLOGY_NUMNODES = "numNodes";
    public static final String FLD_CTOPOLOGY_NODE = "node";
    public static final String FLD_CXMLELEMENT_TAGNAME = "tagName";
    public static final String FLD_CXMLELEMENT_NODEVALUE = "nodeValue";
    public static final String FLD_CXMLELEMENT_SOURCELOCATION = "sourceLocation";
    public static final String FLD_CXMLELEMENT_HASATTRIBUTES = "hasAttributes";
    public static final String FLD_CXMLELEMENT_HASCHILDREN = "hasChildren";
    public static final String FLD_CXMLELEMENT_DETAILEDINFO = "detailedInfo";

    /**
     * The above declarations were generated with this function, so it can also
     * be used to update them if they ever get out of sync. The method prints
     * the declarations on stdout for you to copy/paste them into the code.
     * Just call it while any simulation is running; e.g. temporarily add it
     * into the body of ProcessInfoAction and hit the Process Info button during
     * simulation.
     */
    public static void generateConstants(Simulation simulation) throws CommunicationException {
        cObject classDescriptorsList = simulation.getRootObject(Simulation.ROOTOBJ_CLASSDESCRIPTORS);
        classDescriptorsList.load();
        cObject[] childObjects = classDescriptorsList.getChildObjects();
        simulation.loadUnfilledObjects(childObjects);
        ArrayUtils.reverse(childObjects);
        for (cObject o : childObjects) {
            cClassDescriptor desc = (cClassDescriptor)o;
            for (FieldDescriptor f : desc.getFieldDescriptors()) {
                if (f.getDeclaredOn().equals(desc.getName())) {
                    String className = desc.getName();
                    String fieldName = f.getName();
                    String constantName = "FLD_" + className.toUpperCase() + "_" + fieldName.toUpperCase();
                    System.out.println("public static final String " + constantName + " = \"" + fieldName + "\";");
                }
            }
        }
    }
}
