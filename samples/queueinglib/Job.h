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

class JobTable;

class Job: public Job_Base
{
    friend class JobTable;
    protected:
        Job *parent;
        std::vector<Job*> children;
        JobTable *table;
        virtual void setParent(Job *parent); // only for addChild()
        virtual void parentDeleted();
        virtual void childDeleted(Job *child);
    public:
        Job(const char *name=NULL, int kind=0, JobTable *table=NULL);
        Job(const Job& job);
        virtual ~Job();
        virtual Job *dup() const {return new Job(*this);}

        virtual Job *getParent();
        virtual int getNumChildren() const;
        virtual Job *getChild(int k);

        void addChild(Job *child);
        virtual void makeChildOf(Job *parent);
};

}; // namespace

#endif
