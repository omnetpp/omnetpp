//ERROR: compound module has no parameter named 'p'
simple Gen
endsimple

module Module
   submodules:
      gen: p like Gen;
endmodule
