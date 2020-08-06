module Parent
{
    gates:
        output out;
    submodules:
        src: Src;
        sink: Sink;
    connections:
        src.out1 --> sink.in;
        src.out2 --> out;
}

