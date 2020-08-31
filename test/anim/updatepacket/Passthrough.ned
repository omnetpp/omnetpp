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

module Passthrough
{
    gates:
        input in @loose;
        output out;
    connections:
        in --> { @display("m=s"); } --> out;
}

module NestedPassthrough
{
    @display("i=block/tunnel");
    gates:
        input in;
        output out;
    submodules:
        a: Passthrough { @display("p=100,100"); };
        b: Passthrough { @display("p=100,200"); };
    connections:
        in --> { @display("m=n"); } --> a.in;
        a.out --> Delay --> b.in;
        b.out --> { @display("m=s"); } --> out;
}
