//
// XXX Experimental property descriptors for the simkernel classes.
// Usage: opp_msgc -Xnc sim_std.msg
//

class cNoncopyableObject extends cObject
{
};

class cDefaultList extends cNoncopyableObject
{
    properties:
        omitGetVerb = true;
    fields:
        int defaultListItems;
};

class cComponent extends cDefaultList
{
    properties:
        omitGetVerb = true;
    fields:
        int numInitStages;
        bool isModule;
        int params;
};

class cChannel extends cComponent
{
};

class cBasicChannel extends cChannel
{
    properties:
        omitGetVerb = true;
    fields:
        double delay;
        double error;
        double datarate;
        bool disabled;
        bool isBusy;
        simtime_t transmissionFinishes;
};

class cModule extends cComponent
{
    properties:
        omitGetVerb = true;
    fields:
        string fullPath;
        bool isSimple;
        bool isModule;
        int id;
        bool isVector;
        int index;
        int size;
        int gates;
        bool checkInternalConnections;
};

class cSimpleModule extends cModule
{
    properties:
        omitGetVerb = true;
    fields:
        bool usesActivity;
        bool isTerminated;
        simtime_t simTime;
        bool stackOverflow;
        unsigned int stackSize;
        unsigned int stackUsage;
};

class cCompoundModule extends cModule
{
};

class cArray extends cObject
{
    properties:
        omitGetVerb = true;
    fields:
        int items;
        bool takeOwnership;
};


class cComponentType extends cNoncopyableObject
{
};

class cModuleType extends cComponentType
{
    properties:
        omitGetVerb = true;
    fields:
        bool isNetwork;
};

class cChannelType extends cComponentType
{
};

//----

class cGate extends cNoncopyableObject
{
    properties:
        omitGetVerb = true;
    fields:
        char type;
        int id;
        bool isVector;
        int index;
        bool isBusy;
        simtime_t transmissionFinishes;
        bool isConnectedOutside;
        bool isConnectedInside;
        bool isConnected;
        bool isRouteOK;
};

class cMessage extends cObject
{
    properties:
        omitGetVerb = true;
    fields:
        int kind;
        int priority;
        long length;
        long byteLength;
        bool hasBitError;
        simtime_t timestamp;
        bool isSelfMessage;
        bool isScheduled;
        int senderModuleId;
        int senderGateId;
        int arrivalModuleId;
        int arrivalGateId;
        simtime_t creationTime;
};

class cMessagePar extends cObject
{
    properties:
        omitGetVerb = true;
    fields:
        bool takeOwnership;
        char type;
        bool isNumeric;
        bool isConstant;
        bool isInput;
};

class cPacket extends cMessage
{
    properties:
        omitGetVerb = true;
    fields:
        short protocol;
        short pdu;
};

//----

class cStatistic extends cObject
{
    properties:
        omitGetVerb = true;
    fields:
        long samples;
        double weights;
        double sum;
        double sqrSum;
        double min;
        double max;
        double mean;
        double stddev;
        double variance;
};

class cStdDev extends cStatistic
{
    properties:
        omitGetVerb = true;
    fields:
};

class cWeightedStdDev extends cStdDev
{
};

class cDensityEstBase extends cStdDev
{
    properties:
        omitGetVerb = true;
    fields:
        string detailedInfo;
        bool transformed;
        int cells;
        unsigned long underflowCell;
        unsigned long overflowCell;
};

class cHistogramBase extends cDensityEstBase
{
    properties:
        omitGetVerb = true;
    fields:
        int cells;
};

class cEqdHistogramBase extends cHistogramBase
{
};

class cVarHistogram extends cHistogramBase
{
};

class cLongHistogram extends cEqdHistogramBase
{
};

class cDoubleHistogram extends cEqdHistogramBase
{
};

class cKSplit extends cDensityEstBase
{
};

class cPSquare extends cDensityEstBase
{
};

class cTransientDetection extends cObject
{
};

class cAccuracyDetection extends cObject
{
};

class cTDExpandingWindows extends cTransientDetection
{
};

class cADByStddev extends cAccuracyDetection
{
};

//----

class cExpression extends cPolymorphic
{
    properties:
        omitGetVerb = true;
    fields:
        string toString;
};

class cFSM extends cObject
{
    properties:
        omitGetVerb = true;
    fields:
        int state;
        int inTransientState;
};

class cMathFunction extends cNoncopyableObject
{
};

class cNEDFunction extends cNoncopyableObject
{
};

class cOutVector extends cNoncopyableObject
{
};

class cPar extends cObject
{
    properties:
        omitGetVerb = true;
    fields:
        char type;
        bool isNumeric;
        bool isVolatile;
        bool isConstant;
        bool isSet;
        bool hasDefaultValue;
};

class cProperties extends cPolymorphic
{
};

class cProperty extends cPolymorphic
{
    properties:
        omitGetVerb = true;
    fields:
        string fullPath;
        bool isImplicit;
        short index;
        //XXX const std::vector<const char *>& keys;
};

class cLinkedList extends cObject
{
    properties:
        omitGetVerb = true;
    fields:
        int length;
        bool empty;
};

class cMessageHeap extends cObject
{
    properties:
        omitGetVerb = true;
    fields:
        int length;
        bool empty;
};

class cQueue extends cObject
{
    properties:
        omitGetVerb = true;
    fields:
        int length;
        bool empty;
        bool takeOwnership;
};

class cSimulation extends cNoncopyableObject
{
    properties:
        omitGetVerb = true;
    fields:
        string fullPath;
        int lastModuleId;
        int runNumber;
        simtime_t simTime;
        long eventNumber;
        int contextType;
};

class cScheduler extends cPolymorphic
{
};

class cSequentialScheduler extends cScheduler
{
};

class cRealTimeScheduler extends cScheduler
{
};

class cTopology extends cObject
{
    properties:
        omitGetVerb = true;
    fields:
        int nodes;
};

class cXMLElement
{
    properties:
        omitGetVerb = true;
    fields:
        bool hasAttributes;
        //XXX const cXMLAttributeMap& getAttributes;
        bool hasChildren;
        //XXX cXMLElementList getChildren;
};

