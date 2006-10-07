//ERROR: compound module has no parameter named 'p'
simple Gen
endsimple

module Module
   parameters:
      dummy: numeric;
   submodules:
      gen: p like Gen;
endmodule
