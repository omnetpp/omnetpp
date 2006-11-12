import "sink";

module SinkNode
    gates:
        in: in[];
    submodules:
        sink: Sink;
          gatesizes:
              in[sizeof(in)];
    connections:
        for i=0..sizeof(in)-1 do
            in[i] --> sink.in[i];
        endfor
endmodule

