//
// Test 9: channels I
//

channel microwave
        error 1e-5;
        delay normal( 1e-3, 1e-4 );
endchannel

simple SSSS
  gates:
         out: o, oo;
         in: i, ii;
         out: ooo;
         in: iii;
endsimple

module cccc
  submodules:
         s1 : SSSS;
         s2 : SSSS;
  connections:
         s1.o --> s2.i;
         s1.i <-- microwave <-- s2.o;

         s1.oo -->microwave--> s2.ii;
         s1.ii <-- s2.oo;

         s1.ooo --> s2.iii;
         s1.iii <-- s2.ooo;
endmodule

module dddd
  parameters: p;
  submodules:
         s1 : SSSS;
         s2 : SSSS;
  connections:
         s1.o --> delay 0.01 --> s2.i;
         s1.i <-- delay 0.01 error 1e-5 <-- s2.o;

         s1.oo --> delay ref p --> s2.ii;
         s1.ii <-- delay ref p <-- s2.oo;

         s1.ooo --> s2.iii;
         s1.iii <-- s2.ooo;
endmodule

