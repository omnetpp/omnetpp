//ERROR: unary minus not accepted
simple Gen
    parameters:
      a: numeric;
endsimple

module Module
   submodules:
      gen: Gen;
          parameters:
              a = -5s310ms;
endmodule
