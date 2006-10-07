//ERROR: 'index' may only occur in a submodule vector's definition
simple Gen
    parameters:
      a: numeric;
endsimple

module Module
   parameters:
      x: numeric;
   submodules:
      gen: Gen;
          parameters:
              a = index;
endmodule
