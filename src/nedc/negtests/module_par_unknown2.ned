//ERROR: module type does not have a parameter named 'a'
simple Gen
    parameters:
endsimple

module Module
   submodules:
      gen: Gen;
          parameters:
              a=10;
endmodule
