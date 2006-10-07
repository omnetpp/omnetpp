//ERROR: second argument to 'input()' must be a string literal (prompt text)
simple Gen
    parameters:
      a: numeric;
endsimple

module Module
   submodules:
      gen: Gen;
          parameters:
              a = input(2,2);
endmodule
