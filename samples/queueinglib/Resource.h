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

#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include <omnetpp.h>
#include <list>

namespace queueing {

class IResourcePool;

// interface implemented by modules that (try to) allocate resources
class IResourceAllocator
{
	public:
		// returns true if allocator accepted the resource
		virtual void resourceGranted(IResourcePool *pool) = 0;
		virtual std::string getFullPath() const = 0;
};

// an interface that should be implemented by resource providers
class IResourcePool
{
	public:
		virtual bool tryToAllocate(IResourceAllocator *allocator, long amountToAllocate, int priority) = 0;
		virtual void release(long amountToRelease) = 0;
};


/**
 * A resource representation. Can represent any amount from a single type of resource.
 * Cooperates with ResourceBasedQueues which allocate from this resource. This module do not
 * receive any messages. Resource is allocated/released by direct Method calls
 */
class Resource : public cSimpleModule, public IResourcePool
{
	public:
		struct AllocationRequest {
			IResourceAllocator *allocator;
			long amountToAllocate;
			int priority;
		};
	private:
		long amount;	      // the amount of resource currently available
		typedef std::list<AllocationRequest> AllocationRequestList;
		AllocationRequestList allocatorList;

		void add(AllocationRequest& request);
		void updateDisplayString();

	protected:
		virtual void initialize();
        virtual void finish();

	public:
		Resource();
		virtual ~Resource();
	    virtual bool tryToAllocate(IResourceAllocator *allocator, long amountToAllocate, int priority);
	    virtual void release(long amountToRelease);
};

}; // namespace

#endif
