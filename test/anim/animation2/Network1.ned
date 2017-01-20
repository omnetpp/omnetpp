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

package animation2;

network Network1
{
    @display("bgb=600,700");
    @figure[title](type=text;pos=300,75;anchor=c;text="");
    @figure[operation](type=text;pos=25,150;anchor=start;text="");

    submodules:
        source: Source { @display("p=300,450"); }

        sinkA: Sink { @display("p=300,300"); }
        sinkB: Sink { @display("p=300,600"); }

        nodeA: Node { @display("p=150,450"); }
        nodeB: Node { @display("p=450,450"); }

        sinkAA: Sink { @display("p=100,300"); }
        sinkAB: Sink { @display("p=100,600"); }

        sinkBA: Sink { @display("p=500,300"); }
        sinkBB: Sink { @display("p=500,600"); }

    connections:
        source.outSA --> Ideal --> sinkA.in;
        source.outSB --> Ideal --> sinkB.in;

        source.outNA --> DelayAndRate --> nodeA.in;
        source.outNB --> DelayAndRate --> nodeB.in;

        nodeA.outA --> Ideal --> sinkAA.in;
        nodeA.outB --> Delay --> sinkAB.in;

        nodeB.outA --> Rate --> sinkBA.in;
        nodeB.outB --> DelayAndRate --> sinkBB.in;
}
