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

#include "Resource.h"

namespace queueing {

Define_Module(Resource);

std::ostream& operator << (std::ostream& out, const Resource::AllocationRequest& req)
{
	out << req.allocator->getFullPath() << " amount: " << req.amountToAllocate << " priority: " << req.priority;
	return out;
}

Resource::Resource()
{

}

Resource::~Resource()
{

}

void Resource::initialize()
{
	WATCH_LIST(allocatorList);
    amount = par("amount");
    if (ev.isGUI())
    	updateDisplayString();
}

void Resource::finish()
{
}


bool Resource::tryToAllocate(IResourceAllocator *allocator, long amountToAllocate, int priority)
{
	Enter_Method("allocate(%ld): %s", amountToAllocate, amount >= amountToAllocate ? "success" : "fail");
	if (amount >= amountToAllocate) {
		amount -= amountToAllocate;
	    if (ev.isGUI())
	    	updateDisplayString();
		return true;
	}
	else {
		// register the allocator for later notification
		AllocationRequest req;
		req.priority = priority;
		req.amountToAllocate = amountToAllocate;
		req.allocator = allocator;
		add(req);
		return false;
	}
}

void Resource::release(long amountToRelease)
{
	Enter_Method("release(%ld)", amountToRelease);

	amount += amountToRelease;
	AllocationRequestList::iterator firstIt = allocatorList.begin();
	if (!allocatorList.empty() && firstIt->amountToAllocate <= amount) {
		amount -= firstIt->amountToAllocate;
		firstIt->allocator->resourceGranted(this);
		allocatorList.pop_front();
	}
    if (ev.isGUI())
    	updateDisplayString();
}


void Resource::add(AllocationRequest& request)
{
	// start from the back and look for the first request where priority is <= than ours.
	// put the new request AFTER that

	for (AllocationRequestList::reverse_iterator i = allocatorList.rbegin(); i != allocatorList.rend(); i++)
		if (i->priority <= request.priority) {
			allocatorList.insert(i.base(), request);
			return;
		}

	// if not found put at the beginning
	allocatorList.push_front(request);
}

void Resource::updateDisplayString()
{
    char buff[80];
    sprintf(buff, "#%ld", amount);
    getDisplayString().setTagArg("t",0, buff);
}

}; // namespace
