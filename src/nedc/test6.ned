//
// Test 6: expressions, functions in expressions
//

simple Foo
   parameters:
      p1,p2,p3,p4;
endsimple Foo

module cmTest6_Main
   parameters:
      timeout;
   submodules:
      foo: Foo
         parameters:
            p1 = fmod( timeout, sin( ref ancestor x ) ) + random(),
            p2 = fabs( ref timeout * 2 ),
            p3 = max( timeout, 10 ),
            p4 = 3 * semmi();
endmodule cmTest6_Main

network nTest6_Network: cmTest6_Main
   parameters:
      timeout = input(2500ms,"Retry timeout?");
endnetwork

