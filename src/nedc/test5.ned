//
// Test 5: parameter passing
//

simple ManyPars
   parameters:
      speed,
      p1,p2,p3,p4,
      p1e,p2e,p3e,p4e,
      p1c,p2c,p3c,p4c;
   gates:
      in: ing[];
      out: outg[];
endsimple ManyPars

module cmTest5_Main
   parameters:
      timeout, rand, gsiz;
   submodules:
      manypars: ManyPars[2*gsiz]
	 parameters:
	    speed = 5,
	    p1 = timeout,
	    p2 = ref timeout,
	    p3 = ancestor timeout,
	    p4 = ref ancestor timeout,
	    p1e = (timeout),
	    p2e = (ref timeout),
	    p3e = (ancestor timeout),
	    p4e = (ancestor ref timeout),
	    p1c = 1+(timeout),
	    p2c = 1+(ref timeout),
	    p3c = 1+(ancestor timeout),
	    p4c = 1+(ancestor ref timeout);
	 gatesizes:
	    ing[ gsiz ], outg[ gsiz ];
   connections:
      for m=0..2*gsiz-1, g=0..2*gsiz-1 do
	 manypars[m].outg[g] -->manypars[(m+1)%(gsiz-1)].ing[g];
	 manypars[m].outg[g] --> manypars[(m+1)%(gsiz-1)].ing[g];
      endfor;
endmodule cmTest5_Main

network nTest5_Network: cmTest5_Main
   parameters:
      timeout = input(100,"Retry timeout?"),
      rand = uniform( 10, 50 ),
      gsiz = 10;
endnetwork

