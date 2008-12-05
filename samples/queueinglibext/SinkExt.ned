import org.omnetpp.queueing.Sink;


//
// A Sink extension to demonstrate the subclassing of a simple
// module implemented in a different project. This module extends
// the Sink module to display the number of incoming jobs.
//
simple SinkExt extends Sink
{
    @class(SinkExt);
}
