//
// Test 3: compound modules
//

simple Generator
        parameters:
          num_messages, ia_time;
        gates:
          out: out;
endsimple

simple Sink
        gates:
          in: in;
endsimple

module Nouse
        parameters:
          ia_time;

        submodules:
          generator: Generator
            parameters:
              num_messages = 1e6,
              ia_time = ia_time;
          sink: Sink;

        connections:
          generator.out --> sink.in;
endmodule

network nouse: Nouse
endnetwork
