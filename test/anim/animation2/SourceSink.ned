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

//
// Sample network, consisting of a source and a sink.
//
network SourceSink
{
    @display("bgb=492,440;bgl=18");

    submodules:
        source: Source { @display("p=246,218"); }

        sinkA: Sink { @display("p=246,47"); }
        sinkB: Sink { @display("p=246,410"); }

        nodeA: Node { @display("p=131,218"); }
        nodeB: Node { @display("p=370,218"); }

        sinkAA: Sink { @display("p=30,47"); }
        sinkAB: Sink { @display("p=33,410"); }

        sinkBA: Sink { @display("p=456,410"); }
        sinkBB: Sink { @display("p=456,47"); }

    connections:
        source.outSA --> sinkA.in;
        source.outSB --> sinkB.in;

        source.outNA --> {  datarate = 10Mbps; delay = 10ms; } --> nodeA.in;
        source.outNB --> {  datarate = 10Mbps; delay = 10ms; } --> nodeB.in;

        nodeA.outA --> sinkAA.in;
        nodeA.outB --> {  delay = 10ms; } --> sinkAB.in;

        nodeB.outA --> {  datarate = 10Mbps; } --> sinkBA.in;
        nodeB.outB --> {  datarate = 10Mbps; delay = 10ms; } --> sinkBB.in;
}
