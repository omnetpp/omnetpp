//
// Test 8: conditional sections and conditional connections
//

simple Foo
   parameters:
      speed,
      p1,p2,p3,p4;
   gates:
      in: ing[];
      out: outg[];
endsimple Foo

module cmTest8_Main
   parameters:
      timeout, rand, gsiz;
   submodules:
      foo: Foo[2*gsiz]
         parameters if index>gsiz:
            speed = 5,
            p1 = timeout > 0,
            p2 = timeout!=gsiz,
            p3 = !(timeout==gsiz),
            p4 = timeout==gsiz || 1>3;
         parameters if index<=gsiz:
            speed = 5,
            p1 = timeout > 0,
            p2 = timeout!=gsiz,
            p3 = !(timeout==gsiz),
            p4 = timeout==gsiz || 1>3;
         gatesizes:
            ing[ gsiz ], outg[ gsiz ];
   connections:
      for m=0..2*gsiz-1, g=0..2*gsiz-1 do
         foo[m].outg[g] --> foo[(m+1)%(gsiz-1)].ing[g] if m>g;
         foo[m].outg[g] --> foo[(m+1)%(gsiz-1)].ing[g] if m<=g;
      endfor;
endmodule cmTest8_Main

network nTest8_Network: cmTest8_Main
   parameters:
      timeout = input(100,"Retry timeout?"),
      rand = uniform( 10, 50 ),
      gsiz = 10;
endnetwork

