//ERROR: module type does not have gates
simple Gen
endsimple

module Module
   submodules:
      gen: Gen;
          gatesizes:
            i[10];
endmodule
