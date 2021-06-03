module SinkNode
{
    gates:
        input in[];
    submodules:
        sink: Sink {
            gates:
                in[sizeof(parent.in)];
        };
    connections:
        for i=0..sizeof(in)-1 {
            in[i] --> sink.in[i];
        };
}

