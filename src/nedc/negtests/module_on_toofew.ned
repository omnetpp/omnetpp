//ERROR:  too few machines, module type expects 2
module Gen
   machines: a,b;
endmodule

module Module
   machines: a;
   submodules:
      gen: Gen;
          on: a;
endmodule
