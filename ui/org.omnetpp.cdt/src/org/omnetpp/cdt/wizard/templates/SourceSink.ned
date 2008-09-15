{{copyright}}

{{simulationspackage:}}package {{simulationspackage}};
{{rootpackage:}}import {{rootpackage}}.Source;
{{rootpackage:}}import {{rootpackage}}.Sink;

//
// Sample network, consisting of a source and a sink.
//
network SourceSink
{
    submodules:
        source: Source;
        sink: Sink;
    connections:
        source.out --> sink.in;
}


