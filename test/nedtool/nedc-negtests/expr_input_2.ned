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
              a = input(input, "prompt");
endmodule
