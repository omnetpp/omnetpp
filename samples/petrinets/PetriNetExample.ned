//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2019 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


network Counter
{
    submodules:
        scheduler : TransitionScheduler {
            @display("p=50,50");
        }
        transition: Transition {
            @display("p=165,194");
        }
        destination: Place {
            @display("p=266,246");
        }
        transition1: Transition {
            @display("p=165,91");
        }
        place1: Place {
            @display("p=57,137");
        }
        place2: Place {
            @display("p=266,137");
        }
        store: Place {
            @display("p=266,34");
        }
    connections:
        place1.out++ --> Arc --> transition.in++;
        transition.out++ --> Arc --> destination.in++;
        transition.out++ --> Arc --> place2.in++;
        place2.out++ --> Arc --> transition1.in++;
        transition1.out++ --> Arc --> place1.in++;
        store.out++ --> Arc --> transition1.in++;
}


network VendingMachine
{
    @display("bgb=470,259");
    submodules:
        scheduler : TransitionScheduler {
            @display("p=50,50");
        }
        hot: Place {
            @display("p=106,207");
        }
        cold: Place {
            numInitialTokens = 1;
            @display("p=106,55");
        }
        ready: Place {
            numInitialTokens = 1;
            @display("p=265,55");
        }
        accepted: Place {
            @display("p=265,207");
        }
        inserted: Place {
            @display("p=418,138");
        }
        brew: Transition {
            transitionTime = 5s;
            @display("p=26,135");
        }
        dispense: Transition {
            @display("p=178,135");
        }
        reject: Transition {
            @display("p=317,135");
        }
        insert: Transition {
            transitionTime = exponential(1s);
            @display("p=364,55");
        }
        accept: Transition {
            @display("p=355,207");
        }
    connections:
        cold.out++ --> Arc --> brew.in++;
        brew.out++ --> Arc --> hot.in++;
        hot.out++ --> Arc --> dispense.in++;
        dispense.out++ --> Arc --> cold.in++;
        ready.out++ --> Arc --> insert.in++;
        insert.out++ --> Arc --> inserted.in++;
        inserted.out++ --> Arc --> reject.in++;
        reject.out++ --> Arc --> ready.in++;
        inserted.out++ --> Arc --> accept.in++;
        accept.out++ --> Arc --> accepted.in++;
        accepted.out++ --> Arc --> dispense.in++;
        dispense.out++ --> Arc --> ready.in++;
}

network FromPNML
{
    submodules:
        builder: PetriNetBuilder {
            @display("p=50,50");
        }
        scheduler : TransitionScheduler {
            @display("p=120,50");
        }
}
