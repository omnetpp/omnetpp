//ERROR: module type 'Gen' has no parameter named 'a'
simple Gen
    parameters:
      b: numeric;
endsimple

module Module
   submodules:
      gen: Gen;
          parameters:
              a=10;
endmodule
