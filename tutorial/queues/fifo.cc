//-------------------------------------------------------------
// file: fifo.cc
//        (part of Queues - an OMNeT++ demo simulation)
//
// We print the  number of customers in the system upon arrival.
//-------------------------------------------------------------

#include "fifo.h"

Define_Module( Fifo );

void Fifo::initialize()
{
    // Set up the initial number of jobs in the queue
    int index = this->index(); // get the index no of this fifo queue
    char msgname[32];

    for(int i=0; i< (int) par("num_init_jobs"); ++i)
    {
        sprintf( msgname, "job-%d_%d", index, i);

        cMessage *msg = new cMessage( msgname );
        msg->setTimestamp();
        scheduleAt(simTime(), msg);
    }


    // Arrange for the statistics gathering
    int numcells = par("num_cells");

    jobDist = new cDoubleHistogram("Job Distribution", numcells);
    jobDist->setRange(0,numcells);

    jobsInSys.setName("Jobs in System");

    msgServiced = NULL;
    endServiceMsg = new cMessage("end-service");
}

void Fifo::handleMessage(cMessage *msg)
{
    if (msg==endServiceMsg) // Departure
    {
        endService( msgServiced );
        if (queue.empty()) // There is no remaining customer
        {
            msgServiced = NULL;
        }
        else
        {
            msgServiced = (cMessage *) queue.pop();
            simtime_t serviceTime = serviceRequirement( msgServiced );
            scheduleAt( simTime()+serviceTime, endServiceMsg );
        }
    }
    else if (!msgServiced) // Arrival while server is idle
    {
        arrival( msg );

        // Statistics collection
        jobDist->collect (0);
        jobsInSys.record(0);

        msgServiced = msg;
        simtime_t serviceTime = serviceRequirement( msgServiced );
        scheduleAt( simTime()+serviceTime, endServiceMsg );
    }
    else // Arrival while server is busy
    {
        arrival( msg );

        // Statistics collection
        // There is one customer in service, hence queue.length + 1
        jobDist->collect(queue.length()+1);
        jobsInSys.record(queue.length()+1);
        queue.insert( msg );
    }
}

void Fifo::finish()
{
    ev << "*** Module: " << fullPath() << "***" << endl;

    ev << "Total arrivals:\t" << jobDist->samples() <<endl<<endl;
    ev << "Estimation of the stationary  distribution of jobs,\n";
    ev << "as observed by an arrival.\n";
    ev << "The first column indicates the number in queue observed by an arrival.\n";
    ev << "The second the number of arrivals that saw, j say, jobs in queue.\n";
    ev << "The third is the estimated probability density function.\n";
    for(int i=0; i<jobDist->cells(); ++i) {
        if(jobDist->cell(i) > 0) {
            ev << i << ":\t" << jobDist->cell(i);
            ev << "\t" << jobDist->cellPDF(i) << endl;
        }
    }
    jobDist->recordScalar("Job Distribution Statistics");
}


simtime_t Fifo::serviceRequirement(cMessage *msg)
{
    ev << "Starting service of " << msg->name() << endl;
    return par("service_time");
}

void Fifo::endService(cMessage *msg)
{
    ev << "Completed service of " << msg->name() << endl;
    send( msg, "out" );
}


