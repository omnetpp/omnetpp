@COPYRIGHT_HEADER@


//
// Consumes received messages and collects statistics
//
simple Sink
{
    parameters:
        @display("i=block/sink");
    gates:
        input in;
}

