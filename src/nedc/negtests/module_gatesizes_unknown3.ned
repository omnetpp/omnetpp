//ERROR: module type does not have a gate named 'i'
simple Gen
    gates:
      in: j[];
endsimple

module Module
   submodules:
      gen: Gen;
          gatesizes:
            i[10];
endmodule
