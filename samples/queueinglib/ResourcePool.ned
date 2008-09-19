//
// Copyright (C) 2008 Rudolf Hornig
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
// A resource object representing a single type of resource.
// More tan one ResourceBasedQueue block can specify this bolck as a resource. 
// In this case unstatisfied resource requests are queued (with priority) and the 
// requestor will be notified once the resource gets available.
//
simple ResourcePool
{
    parameters:
        @group(Queueing);
        @display("i=block/buffer;t=#,r,black");
        int amount = default(1);		// the amount of resource units 
}
