//
// Test 4: parameter types
//

simple ParTypes
   parameters:
      n1,
      n2: numeric,
      const c1,
      c2: const,
      c3: const numeric,
      c4: numeric const,
      s: string,
      b: bool,
      a: anytype;
endsimple

module cmTest4_Main
   submodules:
      partypes: ParTypes
         parameters:
           n1=1,
           n2=1,
           c1=1,
           c2=1,
           c3=1,
           c4=1,
           s="some string",
           b=true,
           a=1;
endmodule cmTest4_Main
