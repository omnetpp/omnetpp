//ERROR:  too many machines, module type expects only 1
module Gen
endmodule

module Module
   machines: a;
   submodules:
      gen: Gen;
          on: a, b;
endmodule
