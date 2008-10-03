//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2008 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __JOBTABLE_H__
#define __JOBTABLE_H__

#include <omnetpp.h>
#include <set>

namespace queueing {

/**
 * Makes it possible to iterate over all Job messages in the system.
 */
class JobList : public cSimpleModule
{
    friend class Job;
    protected:
        std::set<Job*> jobs;
        static JobList *defaultInstance;
    public:
        JobList();
        ~JobList();
    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        void registerJob(Job *job);
        void deregisterJob(Job *job);
    public:
        /**
         * Returns pointer to the default instance of JobList, which is
         * the first JobList module in the network. Returns NULL if
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
