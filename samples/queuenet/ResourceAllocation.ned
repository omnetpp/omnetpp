import omnetpp.queueing.ResourcePool;
import omnetpp.queueing.ResourceBasedQueue;
import omnetpp.queueing.Sink;
import omnetpp.queueing.Source;


//
// This example contains a single Resource (token) and two queues 
// e
//
network ResourceAllocation
{
    submodules:
        source1: Source {
            @display("p=42,75");
        }
        resource: ResourcePool {
            @display("p=194,124");
        }
        queue1: ResourceBasedQueue {
            @display("p=196,38");
        }
        sink1: Sink {
            @display("p=357,81");
        }
        source2: Source {
            @display("p=42,168");
        }
        queue2: ResourceBasedQueue {
            @display("p=188,219");
        }
        sink2: Sink {
            @display("p=358,169");
        }
    connections:
        source1.out --> queue1.in++;
        queue1.out --> sink1.in++;
        source2.out --> queue2.in++;
        queue2.out --> sink2.in++;
}
