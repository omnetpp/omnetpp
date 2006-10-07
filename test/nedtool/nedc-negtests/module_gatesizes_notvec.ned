//ERROR: gate 'i' is not a vector gate
simple Gen
    gates:
      in: i;
endsimple

module Module
   submodules:
      gen: Gen;
          gatesizes:
            i[10];
endmodule
