//ERROR: parse error
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
              a = 3 * input(5,"prompt");
endmodule
