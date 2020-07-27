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

module SourceHost
{
    parameters:
        @display("i=device/pc");
    gates:
        input in;
        output out;
    submodules:
        source: Source {
            @display("p=200,100");
        }
        phy: Phy {
            @display("p=200,200");
        }
    connections allowunconnected:
        source.out --> phy.upperLayerIn;
        phy.mediumOut --> out;
        in --> phy.mediumIn;
}

module SinkHost
{
    parameters:
        @display("i=device/pc");
    gates:
        input in;
        output out;
    submodules:
        sink: Sink {
            @display("p=200,100");
        }
        phy: Phy {
            @display("p=200,200");
        }
    connections allowunconnected:
        phy.mediumOut --> out;
        in --> phy.mediumIn;
        phy.upperLayerOut --> sink.in;
}

module Relay
{
    gates:
        input in1;
        input in2;
        output out1;
        output out2;
    connections:
        in1 --> out2;
        in2 --> out1;
}

module SwitchNode
{
    parameters:
        @display("i=device/pc");
    gates:
        input in1;
        input in2;
        output out1;
        output out2;
    submodules:
        relay: Relay {
            @display("p=150,80");
        }
        phy1: Phy {
            @display("p=100,180");
        }
        phy2: Phy {
            @display("p=200,180");
        }
    connections allowunconnected:
        phy1.upperLayerOut --> relay.in1;
        phy1.upperLayerIn <-- relay.out1;
        phy2.upperLayerOut --> relay.in2;
        phy2.upperLayerIn <-- relay.out2;
        phy1.cutthroughOut --> phy2.cutthroughIn;
        phy1.cutthroughIn <-- phy2.cutthroughOut;
        phy1.mediumOut --> { @display("m=s"); } --> out1;
        phy1.mediumIn <-- { @display("m=s"); } <-- in1;
        phy2.mediumOut --> { @display("m=s"); } --> out2;
        phy2.mediumIn <-- { @display("m=s"); } <-- in2;
}

network Network
{
    types:
        channel Link extends ned.DatarateChannel {
            delay = 1ms;
        }
    submodules:
        source: SourceHost {
            @display("p=50,80");
        }
        switch: SwitchNode {
            @display("p=150,80");
        }
        sink: SinkHost {
            @display("p=250,80");
        }
    connections:
        source.out --> Link --> switch.in1;
        switch.out1 --> Link --> source.in;
        switch.out2 --> Link --> sink.in;
        sink.out --> Link --> switch.in2;
}
