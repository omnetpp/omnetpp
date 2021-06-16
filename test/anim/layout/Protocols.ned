network Protocols1 extends Base
{
    gates:
        input in;
        output out;
    submodules:
        mac: Node {
            parameters:
                @display("i=queue");
            gates:
                in[2];
                out[2];
        };
        gen: Node {
            parameters:
                @display("i=block/source");
            gates:
                in[0];
                out[1];
        };
        sink: Node {
            parameters:
                @display("i=sink");
            gates:
                in[1];
                out[0];
        };
    connections allowunconnected:
        in --> mac.in[0];
        out <-- mac.out[0];
        mac.out[1] --> sink.in[0];
        mac.in[1] <-- gen.out[0];
}

