//
// Copyright (C) 2006 Rudolf Hornig
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

package omnetpp.queueing;

//
// Sends out incoming messages on ALL output gate. The original
// message is sent out on out[0]. The other out gates will get a copy
// of the original message.
//
simple Fork
{
    parameters:
        @group(Queueing);
        @display("i=block/fork");
        bool changeMsgNames = default(true);  // whether to change the message names when they go through te module
    gates:
        input in[];
        output out[];
}
