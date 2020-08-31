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

package updatepacket;

import ned.IdealChannel;

network NetworkBase {
    @display("bgb=500,700");

    submodules:
        pt1: Passthrough {
            @display("p=400,200");
        }
        pt2: NestedPassthrough {
            @display("p=100,300");
        }
        pt3: Passthrough {
            @display("p=400,400");
        }
        pt4: NestedPassthrough {
            @display("p=100,500");
        }
        sink: Sink {
            @display("p=400,600");
        }
    connections:
        pt1.out --> DelayAndRate --> pt2.in;
        pt2.out --> Delay --> pt3.in;
        pt3.out --> Ideal --> pt4.in;
        pt4.out --> Delay --> sink.in;
}

network Network extends NetworkBase
{
    submodules:
        src: Source {
            @display("p=100,100");
        }
    connections:
        src.out --> Ideal --> pt1.in;
}

network DirectNetwork extends NetworkBase
{
    submodules:
        src: DirectSource {
            @display("p=100,100");
        }
        recv: Passthrough {
            @display("p=250,150");
        }
    connections:
        recv.out --> Ideal --> pt1.in;
}

network DirectNestedNetwork extends NetworkBase
{
    submodules:
        srcCont: DirectSourceContainer {
            @display("p=100,100");
        }
        recvCont: DirectReceiverContainer {
            @display("p=250,150");
        }
    connections:
        recvCont.out --> Ideal --> pt1.in;
}
