//ERROR: name 'gen' not unique
simple Gen
endsimple

module Module
   submodules:
      gen: Gen;
      gen: Gen;
endmodule
