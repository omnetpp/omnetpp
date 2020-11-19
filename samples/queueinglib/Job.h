//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __QUEUEING_JOB_H
#define __QUEUEING_JOB_H

#include <vector>
#include "Job_m.h"

namespace queueing {

class JobList;

/**
 * We extend the generated Job_Base class with support for split-join, as well
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
class QUEUEING_API Job: public Job_Base
{
    friend class JobList;
    protected:
        Job *parent = nullptr;
        std::vector<Job*> children;
        JobList *jobList;
        virtual void setParent(Job *parent); // only for addChild()
        virtual void parentDeleted();
        virtual void childDeleted(Job *child);
    public:
        /**
         * Creates a job with the given name, message kind, and jobList. If
         * jobList==nullptr, the default one (or none if none exist) will be chosen.
         */
        Job(const char *name=nullptr, int kind=0, JobList *table=nullptr);

        /** Copy constructor */
        Job(const Job& job);

        /** Destructor */
        virtual ~Job();

        /** Duplicates this job */
        virtual Job *dup() const override {return new Job(*this);}

        /** Assignment operator. Does not affect parent, children and jobList. */
        Job& operator=(const Job& job);

        /** @name Parent-child relationships */
        //@{
        /** Returns the parent job. Returns nullptr if there's no parent or it no longer exists. */
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

        /** Returns the JobList where this job has been registered. */
        JobList *getContainingJobList() {return jobList;}

};

}; // namespace

#endif
