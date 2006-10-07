//ERROR: unary minus not accepted
simple Gen
    parameters:
      a: bool;
endsimple

module Module
   submodules:
      gen: Gen;
          parameters:
              a = -true;
endmodule
