//ERROR: unary minus not accepted
simple Gen
    parameters:
      a: string;
endsimple

module Module
   submodules:
      gen: Gen;
          parameters:
              a = -"one";
endmodule
