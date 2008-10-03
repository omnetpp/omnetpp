//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2008 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <algorithm>
#include "Job.h"
#include "JobList.h"

namespace queueing {

Job::Job(const char *name, int kind, JobList *jobList) : Job_Base(name, kind)
{
    parent = NULL;
    if (jobList==NULL && JobList::getDefaultInstance()!=NULL)
        jobList = JobList::getDefaultInstance();
    this->jobList = jobList;
    if (jobList!=NULL)
        jobList->registerJob(this);
}

Job::Job(const Job& job)
{
    setName(job.getName());
    operator=(job);
    parent = NULL;
    jobList = job.jobList;
    if (jobList!=NULL)
        jobList->registerJob(this);
}

Job::~Job()
{
    if (parent)
        parent->childDeleted(this);
    for (int i=0; i<(int)children.size(); i++)
        children[i]->parentDeleted();
    if (jobList!=NULL)
        jobList->deregisterJob(this);
}

Job& Job::operator=(const Job& job)
{
    if (this==&job) return *this;
    Job_Base::operator=(job);
    // leave parent and jobList untouched
    return *this;
}

Job *Job::getParent()
{
    return parent;
}

void Job::setParent(Job *parent)
{
    this->parent = parent;
}

int Job::getNumChildren() const
{
    return children.size();
}

Job *Job::getChild(int k)
{
    if (k<0 || k>=(int)children.size())
        throw cRuntimeError(this, "child index %d out of bounds", k);
    return children[k];
}

void Job::makeChildOf(Job *parent)
{
    parent->addChild(this);
}

void Job::addChild(Job *child)
{
    child->setParent(this);
    ASSERT(std::find(children.begin(), children.end(), child)==children.end());
    children.push_back(child);
}

void Job::parentDeleted()
{
    parent = NULL;
}

void Job::childDeleted(Job *child)
{
    std::vector<Job*>::iterator it = std::find(children.begin(), children.end(), child);
    ASSERT(it!=children.end());
    children.erase(it);
}

}; // namespace
