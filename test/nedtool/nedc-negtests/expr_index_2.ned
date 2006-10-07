//ERROR: 'index' may only occur in a submodule vector's definition
simple Gen
    parameters:
      a: numeric;
endsimple

module Module
   submodules:
      gen: Gen;
          parameters:
              a = 1+index;
endmodule
