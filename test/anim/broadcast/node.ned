import "sink";
import "tx";

module Node
    gates:
        in: in;
    submodules:
        tx: Tx;
        sink: Sink;
    connections:
        in --> sink.in;
endmodule

