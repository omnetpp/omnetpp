//ERROR: 'index' may only occur in a submodule vector's definition
simple Gen
endsimple

module Module
   submodules:
      gen: Gen[index];
endmodule
