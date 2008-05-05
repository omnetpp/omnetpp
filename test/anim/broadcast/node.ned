module Node
{
    gates:
        input in;
    submodules:
        tx: Tx;
        sink: Sink;
    connections:
        in --> sink.in;
}

