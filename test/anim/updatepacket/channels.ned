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

channel Ideal extends ned.IdealChannel { @display("ls=,3;t=ideal"); }
channel Delay extends ned.DelayChannel { @display("ls=blue,3;t=10 ms"); delay = 10ms; }
channel Rate extends ned.DatarateChannel { @display("ls=green,3;t=10Mbps"); datarate = 10Mbps; }
channel DelayAndRate extends ned.DatarateChannel {
    @display("ls=cyan,3;t=10Mbps 10ms");
    datarate = 10Mbps;
    delay = 10ms;
}
