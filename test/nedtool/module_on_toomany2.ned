//ERROR:  too many machines, module type expects only 2
module Gen
   machines: a,b;
endmodule

module Module
   machines: a,b,c;
   submodules:
      gen: Gen;
          on: a,b,c;
endmodule
