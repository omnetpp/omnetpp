
import blabla 34234 ;
import "blabla" garbage;
import "okay";

property wrong1 213131 ( ;
property wrong2 ( bla );
property @wrong3 ( 213131 ;
property @wrong4 ( bla ;

property @okay ( bla );

// syntax error in channels (simple modules, compound modules, interfaces etc should be the same)
channel $@#$@ bla bla { }
channel wrongch1 bla bla { }
channel wrongch2 { delay=10; error=bla bla; cost=0.5;}

simple $@#$@ bla bla { }
simple module wrongmod1 bla bla { }
simple wrongmod2 { delay=10; error=bla bla; cost=0.5; }
simple wrongmod3 { delay=10; error=bla; cost=0.5; }

// syntax error in properties
simple prop1 { delay=10; @; cost=0.5; }
simple prop2 { delay=10; @453534; cost=0.5; }
simple prop3 { delay=10; @wrong bla(); cost=0.5; }
simple prop4 { delay=10; @wrong( ; cost=0.5; color="red"; }
simple prop5 { delay=10; @wrong("value" ; cost=0.5; color="red"; }
simple prop6 { delay=10; @wrong("value", ; cost=0.5; color="red"; }

// syntax error in parameters
simple param1 { $#%@; cost=0.5; }
simple param2 { delay $%#; cost=0.5; }
simple param3 { delay bla bla = 10; cost=0.5; }
simple param4 { delay=%$; cost=0.5; }
simple param5 { delay=10 if ...; cost=0.5; }

// syntax error in gates
simple gates1 { gates: input inout iii; output ooo; }
simple gates2 { gates: input blabla bla; output ooo; }
simple gates3 { gates: input bla[; output ooo; }
simple gates4 { gates: input bla[12; output ooo; }
simple gates5 { gates: blabla iii; output ooo; }
simple gates6 { gates: blabla[; output ooo; }
simple gates7 { gates: blabla[12; output ooo; }
simple gates8 { gates: blabla[12] bla; output ooo; }
simple gates9 { gates: blabla[12] if ...; output ooo; }

// syntax error in submodules
module submod1 { submodules: bla bla; foo:Foo; }  // FIXME ??????
module submod2 { submodules: bla : Bla Bla; foo:Foo; }
module submod3 { submodules: bla : Bla like $$$; foo:Foo; }
module submod4 { submodules: bla bla {} foo:Foo; }   // FIXME submodtype gets empty!!!!
module submod5 { submodules: bla : Bla Bla {} foo:Foo; phi:Phi;}  // note: foo gets lost, but phi is OK
module submod6 { submodules: bla : Bla like $$$ {} foo:Foo; phi:Phi;}  // note: foo gets lost, but phi is OK
module submod7 { submodules: bla : Bla { $@$@ } foo:Foo; }


//TODO connections!  paramgroups, gategroups?

network Network
{
    submodules:
        hosts[100] : Host {
            parameters:
                @display("c=green") if index==3;
                @display("c=red");
                parX = 45 if index>9;
            gates:
                port[5] if index==3;
                port[8];
                if index>9 {
                    portA[34];
                    portB[3];
                }
        };

};

network RandomGraph
{
    submodules:
        node[100]: Node {}
    connections:
        // brief syntax:
        node[i].port++ <--> node[j].port++ where i=0..98, j=i+1..99, uniform()<0.3;

        where i=0..98, j=i+1..99, uniform()<0.3 {
            node[i].port++ <--> node[j].port++;
        };

};
