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

#include <algorithm>
#include "Job.h"
#include "JobTable.h"

namespace queueing {

Job::Job(const char *name, int kind, JobTable *table) : Job_Base(name, kind) {
    parent = NULL;
    if (table==NULL && JobTable::getDefaultInstance()!=NULL)
        table = JobTable::getDefaultInstance();
    this->table = table;
    if (table!=NULL)
        table->registerJob(this);
}

Job::Job(const Job& job) {
    setName(job.getName());
    parent = NULL;
    table = job.table;
    if (table!=NULL)
        table->registerJob(this);
}

Job::~Job() {
    if (parent)
        parent->childDeleted(this);
    for (int i=0; i<children.size(); i++)
        children[i]->parentDeleted();
    if (table!=NULL)
        table->deregisterJob(this);
}

Job *Job::getParent() {
    return parent;
}

void Job::setParent(Job *parent) {
    this->parent = parent;
}

int Job::getNumChildren() const {
    return children.size();
}

Job *Job::getChild(int k) {
    if (k<0 || k>=children.size())
        throw cRuntimeError(this, "child index %d out of bounds", k);
    return children[k];
}

void Job::makeChildOf(Job *parent) {
    parent->addChild(this);
}

void Job::addChild(Job *child) {
    child->setParent(this);
    ASSERT(std::find(children.begin(), children.end(), child)==children.end());
    children.push_back(child);
}

void Job::parentDeleted() {
    parent = NULL;
}

void Job::childDeleted(Job *child) {
    std::vector<Job*>::iterator it = std::find(children.begin(), children.end(), child);
    ASSERT(it!=children.end());
    children.erase(it);
}

}; // namespace
