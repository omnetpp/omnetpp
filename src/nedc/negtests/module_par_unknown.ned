//ERROR: module type does not have parameters
simple Gen
endsimple

module Module
   submodules:
      gen: Gen;
          parameters:
              a=10;
endmodule
