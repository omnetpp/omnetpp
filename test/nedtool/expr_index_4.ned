//ERROR: 'index' may only occur in a submodule vector's definition
simple Gen
    gates: in: in; out: out;
endsimple

module Module
   submodules:
      a: Gen[1];
      b: Gen[1];
   connections:
      a[index].out --> b[0].in,
      a[index].in <-- b[0].out;
endmodule
