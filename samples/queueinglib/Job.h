//
// Copyright (C) 2008 Andras Varga
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

#ifndef __JOB_H_
#define __JOB_H_

#include <vector>
#include "Job_m.h"

namespace queueing {

class JobList;

/**
 * We extend the generated Job_Base class with support for split-join, and well
 * as the ability to enumerate all jobs in the system.
 *
 * To support split-join, Jobs manage parent-child relationships. A
 * relationship is created with the makeChildOf() or addChild() methods,
 * and lives until the parent or the child Job is destroyed.
 * It can be queried with the getParent() and getNumChildren()/getChild(k)
 * methods.
 *
 * To support enumerating all jobs in the system, each Job automatically
 * registers itself in a JobList module, if one exist in the model.
 * (If there's no JobList module, no registration takes place.) If there
 * are more than one JobList modules, the first one is chosen.
 * JobList can also be explicitly specified in the Job constructor.
 * The default JobList can be obtained with the JobList::getDefaultInstance()
 * method. Then one can query JobList for the set of Jobs currently present.
 */
class Job: public Job_Base
{
    friend class JobList;
    protected:
        Job *parent;
        std::vector<Job*> children;
        JobList *jobList;
        virtual void setParent(Job *parent); // only for addChild()
        virtual void parentDeleted();
        virtual void childDeleted(Job *child);
    public:
        /**
         * Creates a job with the given name, message kind, and jobList. If
         * jobList==NULL, the default one (or none if none exist) will be chosen.
         */
        Job(const char *name=NULL, int kind=0, JobList *table=NULL);

        /** Copy constructor */
        Job(const Job& job);

        /** Destructor */
        virtual ~Job();

        /** Duplicates this job */
        virtual Job *dup() const {return new Job(*this);}

        /** @name Parent-child relationships */
        //@{
        /** Returns the parent job. Returns NULL if there's no parent or it no longer exists. */
        virtual Job *getParent();

        /** Returns the number of children. Deleted children are automatically removed from this list. */
        virtual int getNumChildren() const;

        /** Returns the kth child. Throws an error if index is out of range. */
        virtual Job *getChild(int k);

        /** Marks the given job as the child of this one. */
        void addChild(Job *child);

        /** Same as addChild(), but has to be invoked on the child job */
        virtual void makeChildOf(Job *parent);
        //@}
};

}; // namespace

#endif
