//
// Test 7: parametrized module and gate vectors, 'for' loop
//

simple Foo
   parameters:
      speed,
      p1,p2,p3,p4;
   gates:
      in: ing[];
      out: outg[];
endsimple Foo

module cmTest7_Main
   parameters:
      timeout, rand, gsiz;
   submodules:
      foo: Foo[2*gsiz]
         parameters:
            speed = 5,
            p1 = timeout>0,
            p2 = timeout!=gsiz,
            p3 = !(timeout==gsiz),
            p4 = timeout==gsiz || 1>3;
         gatesizes:
            ing[ gsiz ], outg[ gsiz ];
   connections:
      for m=0..2*gsiz-1, g=0..2*gsiz-1 do
         foo[m].outg[g] --> foo[(m+1)%(gsiz-1)].ing[g];
         foo[m].outg[g] --> foo[(m+1)%(gsiz-1)].ing[g];
      endfor;
endmodule cmTest7_Main

network nTest7_Network: cmTest7_Main
   parameters:
      timeout = input(100,"Retry timeout?"),
      rand = uniform( 10, 50 ),
      gsiz = 10;
endnetwork

