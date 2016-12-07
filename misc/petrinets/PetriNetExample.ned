//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//


network Counter
{
    submodules:
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
            pnmlFile = xmldoc("examples/Ballgame.pnml");
        }
}
