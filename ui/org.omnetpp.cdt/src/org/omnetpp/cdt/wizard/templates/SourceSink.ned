{{copyright}}


//
// Sample network, consisting of a source and a sink.
//
network {{ProjectName}}
{
    parameters:
        source: Source;
        sink: Sink;
    connections:
        source.out --> sink.in;
}
