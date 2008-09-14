@COPYRIGHT_HEADER@


//
// Sample network, consisting of a source and a sink.
//
network @NETWORKNAME@
{
    parameters:
        source: Source;
        sink: Sink;
    connections:
        source.out --> sink.in;
}
