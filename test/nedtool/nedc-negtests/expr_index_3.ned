//ERROR: 'index' is not allowed here
simple Gen
endsimple

module Module
   submodules:
      gen: Gen[index];
endmodule
