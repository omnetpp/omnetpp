//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __QUEUEING_JOBLIST_H
#define __QUEUEING_JOBLIST_H

#include "QueueingDefs.h"
#include <set>

namespace queueing {

/**
 * Makes it possible to iterate over all Job messages in the system.
 */
class QUEUEING_API JobList : public cSimpleModule
{
    friend class Job;
    protected:
        std::set<Job*> jobs;
        static JobList *defaultInstance;
    public:
        JobList();
        ~JobList();
    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
        void registerJob(Job *job);
        void deregisterJob(Job *job);
    public:
        /**
         * Returns pointer to the default instance of JobList, which is
         * the first JobList module in the network. Returns nullptr if
         * there's no JobList module.
         */
        static JobList *getDefaultInstance();

        /**
         * Returns the pointers of the jobs currently existing in the model.
         */
        const std::set<Job*> getJobs();
};

}; // namespace

#endif


