{{copyright}}


//
// Generates messages with a configurable interarrival time.
//
simple Source
{
    parameters:
        volatile double sendInterval @unit(s) = default(exponential(1s));
        @display("i=block/source");
    gates:
        output out;
}


