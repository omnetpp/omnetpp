module ConnColors
{
    @display("bgb=779,384");
    gates:
        input in[];
        output out[];
        inout port[];
    submodules:
        referencePic: Dummy {
            @display("p=718,345;tt=open me to see,how the module should look like;i=device/scanner;bgi=conncolors;bgb=783,411,,,0");
        }
        src1: Dummy {
            parameters:
                @display("i=misc/node2_s;p=35,28");
        }
        dest1: Dummy {
            parameters:
                @display("i=misc/node2_s;p=174,51");
        }
        src2: Dummy {
            parameters:
                @display("i=misc/node2_s;p=35,74");
        }
        dest2: Dummy {
            parameters:
                @display("i=misc/node2_s;p=174,97");
        }
        src3: Dummy {
            parameters:
                @display("i=misc/node2_s;p=35,126");
        }
        dest3: Dummy {
            parameters:
                @display("i=misc/node2_s;p=174,149");
        }
        src4: Dummy {
            parameters:
                @display("i=misc/node2_s;p=220,28");
        }
        dest4: Dummy {
            parameters:
                @display("i=misc/node2_s;p=354,51");
        }
        src5: Dummy {
            parameters:
                @display("i=misc/node2_s;p=220,74");
        }
        dest5: Dummy {
            parameters:
                @display("i=misc/node2_s;p=354,97");
        }
        src6: Dummy {
            parameters:
                @display("i=misc/node2_s;p=220,126");
        }
        dest6: Dummy {
            parameters:
                @display("i=misc/node2_s;p=354,149");
        }
        dest7: Dummy {
            parameters:
                @display("i=misc/node2_s;p=354,201");
        }
        src7: Dummy {
            parameters:
                @display("i=misc/node2_s;p=220,178");
        }
        src8: Dummy {
            parameters:
                @display("i=misc/node2_s;p=407,28");
        }
        src9: Dummy {
            parameters:
                @display("i=misc/node2_s;p=407,178");
        }
        dest8: Dummy {
            parameters:
                @display("i=misc/node2_s;p=541,149");
        }
        dest9: Dummy {
            parameters:
                @display("i=misc/node2_s;p=541,201");
        }
        src10: Dummy {
            parameters:
                @display("i=misc/node2_s;p=407,126");
        }
        dest10: Dummy {
            parameters:
                @display("i=misc/node2_s;p=541,97");
        }
        dest11: Dummy {
            parameters:
                @display("i=misc/node2_s;p=541,51");
        }
        src11: Dummy {
            parameters:
                @display("i=misc/node2_s;p=407,74");
        }
        dest12: Dummy {
            parameters:
                @display("i=misc/node2_s;p=718,149");
        }
        src12: Dummy {
            parameters:
                @display("i=misc/node2_s;p=584,126");
        }
        dest13: Dummy {
            parameters:
                @display("i=misc/node2_s;p=718,51");
        }
        src13: Dummy {
            parameters:
                @display("i=misc/node2_s;p=584,74");
        }
        dest14: Dummy {
            parameters:
                @display("i=misc/node2_s;p=718,201");
        }
        src14: Dummy {
            parameters:
                @display("i=misc/node2_s;p=584,178");
        }
        dest15: Dummy {
            parameters:
                @display("i=misc/node2_s;p=718,97");
        }
        src15: Dummy {
            parameters:
                @display("i=misc/node2_s;p=584,28");
        }
        dest16: Dummy {
            parameters:
                @display("i=misc/node2_s;p=174,235");
        }
        src16: Dummy {
            parameters:
                @display("i=misc/node2_s;p=35,212");
        }
        dest17: Dummy {
            parameters:
                @display("i=misc/node2_s;p=174,281");
        }
        src17: Dummy {
            parameters:
                @display("i=misc/node2_s;p=35,258");
        }
        dest19: Dummy {
            parameters:
                @display("i=misc/node2_s;p=541,247");
        }
        src19: Dummy {
            parameters:
                @display("i=misc/node2_s;p=584,224");
        }
        src20: Dummy {
            parameters:
                @display("i=misc/node2_s;p=407,224");
        }
        dest20: Dummy {
            parameters:
                @display("i=misc/node2_s;p=354,247");
        }
        src21: Dummy {
            parameters:
                @display("i=misc/node2_s;p=220,224");
        }
        dest21: Dummy {
            parameters:
                @display("i=misc/node2_s;p=718,247");
        }
        src18: Dummy {
            parameters:
                @display("i=misc/node2_s;p=220,272");
        }
        dest18: Dummy {
            parameters:
                @display("i=misc/node2_s;p=354,295");
        }
        src22: Dummy {
            parameters:
                @display("i=misc/node2_s;p=407,272");
        }
        dest22: Dummy {
            parameters:
                @display("i=misc/node2_s;p=541,295");
        }
        src23: Dummy {
            parameters:
                @display("i=misc/node2_s;p=584,272");
        }
        dest23: Dummy {
            parameters:
                @display("i=misc/node2_s;p=718,295");
        }
    connections allowunconnected:
        src1.out++ --> { @display("t=default;tt=tooltip text"); } --> dest1.in++;
        src2.out++ --> { @display("t=two oneway 1"); } --> dest2.in++;
        dest2.out++ --> { @display("t=two oneway 2"); } --> src2.in++;
        src3.twoway++ <--> { @display("t=bidirectional"); } <--> dest3.twoway++;
        src4.out++ --> { @display("t=0px (invisible);tt=tooltip text;ls=,0"); } --> dest4.in++;
        src5.out++ --> { @display("t=1px width;ls=,1"); } --> dest5.in++;
        src6.out++ --> { @display("t=3px width;ls=,3"); } --> dest6.in++;
        src7.out++ --> { @display("t=10px width;ls=,10"); } --> dest7.in++;
        src8.out++ --> { @display("t=0px (invisible);tt=tooltip text;ls=,0,d"); } --> dest11.in++;
        src9.out++ --> { @display("t=10px dotted;ls=,10,d"); } --> dest9.in++;
        src10.out++ --> { @display("t=3px dotted;ls=,3,d"); } --> dest8.in++;
        src11.out++ --> { @display("t=1px dotted;ls=,1,d"); } --> dest10.in++;
        src12.out++ --> { @display("t=3px dashed;ls=,3,da"); } --> dest12.in++;
        src13.out++ --> { @display("t=1px dashed;ls=,1,da"); } --> dest15.in++;
        src14.out++ --> { @display("t=10px dashed;ls=,10,da"); } --> dest14.in++;
        src15.out++ --> { @display("t=0px (invisible);tt=tooltip text;ls=,0,da"); } --> dest13.in++;
        src16.out++ --> { @display("t=yellow;ls=yellow"); } --> dest16.in++;
        src17.out++ --> { @display("t=yellow\nblue text,,blue;ls=yellow"); } --> dest17.in++;
        src19.out++ --> { @display("t=3px dashed;ls=yellow,3,da"); } --> dest21.in++;
        src20.out++ --> { @display("t=3px dotted;ls=yellow,3,d"); } --> dest19.in++;
        src21.out++ --> { @display("t=3px width;ls=yellow,3"); } --> dest20.in++;
        src18.out++ --> { @display("t=3px yellow\nblue txt,,blue;ls=yellow,3"); } --> dest18.in++;
        src22.out++ --> { @display("t=3px dotted yellow\nblue text,,blue;ls=yellow,3,d"); } --> dest22.in++;
        src23.out++ --> { @display("t=3px dashed yellow\nblue text,,blue;ls=yellow,3,da"); } --> dest23.in++;
}

