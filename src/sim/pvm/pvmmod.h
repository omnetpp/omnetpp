//=========================================================================
//
// PVMMOD.H - part of
//                          OMNeT++
//           Discrete System Simulation in C++
//
//   Written by:  Zoltan Vass, 1996
//   Rewrite,syncpoints,maintenance: Andras Varga, 1996-2000
//
//   Contents:
//      declaration of class cPvmMod, the interface modul for running
//      simulation on Parallel Virtual Machine.
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __PVMMOD_H
#define __PVMMOD_H

#include <cnetmod.h>
#include "pvmdefs.h"

//=========================================================================
class cNetGate : public cGate
{
  private:
    int target_Proc;
    int target_Gate;
  public:
    cNetGate(cNetGate& gate);
    cNetGate(const char *name, int tp);
    virtual ~cNetGate()    {}
    virtual const char *className() const {return "cNetGate";}
    virtual cObject *dup()    {return new cNetGate(*this);}
    cNetGate& operator=(cNetGate& gate);

    void settarget(int proc, int gate_n)   {target_Proc=proc;target_Gate=gate_n;}
    int t_proc()   {return target_Proc;}
    int t_gate()   {return target_Gate;}
};

//=========================================================================
struct sSyncPoint {
    simtime_t t;
    int gate;
    sSyncPoint *next;
};

class cPvmMod : public cNetMod
{
  private:
    cArray out_gatev;             // separate list for output gates

    struct pvmhostinfo *hosts;    // information about the lists
    struct pvmtaskinfo *tasks;    // information about the tasks
    int num_outgates,num_ingates; // gate counts
    int *all_tids;                // array of TIDs of all segments

    int host_cnt;                 // number of hosts in the VM
    int my_tid, parent_tid;       // PVM process IDs (parent=console)
    opp_string my_host;           // name of local host

    sSyncPoint *syncpoints;       // pointer to syncpoint list

  private:
    void net_sendmsg(cMessage *msg,int ongate); // send cMessage over PVM
    void do_process_netmsg(int rbuff);      // core of process_netmsgs
    void synchronize();                     // waits until all segments call synchronize()

    // syncpoints
    void add_syncpoint(simtime_t t, int gate);
    void del_syncpoint(simtime_t msgtime, int gate);
    void cancel_syncpoint(simtime_t t, int gate);
    simtime_t next_syncpoint();

  public:
    cPvmMod();
    virtual ~cPvmMod();
    virtual const char *className() const {return "cPvmMod";}
    virtual void info(char *buf);
    virtual void callInitialize();
    virtual void callFinish();

    // redefined virtual functions
    virtual void sync_after_modinits(); // sync before processing first 'real' event
    virtual void process_netmsgs();
    virtual void process_netmsg_blocking();
    virtual void send_syncpoint( simtime_t t, int gate);
    virtual void send_cancelsyncpoint( simtime_t t, int gate);
    virtual bool block_on_syncpoint( simtime_t t);
    virtual void arrived(cMessage *msg,int ongate, simtime_t at); // sends cMessage over PVM

    // redefined cNetMod virtual functions
    // (to be called from environment classes (TOmnetApp descendants))
    virtual void putmsg_onconsole(const char *s);
    virtual void puts_onconsole(const char *s);
    virtual bool gets_onconsole(const char *promptstr, char *buf, int len);
    virtual bool askyesno_onconsole(const char *question);

    // new functions: handling gates
    cGate *ingate(int g) {return (cNetGate*)gatev[g];}      // find input gate by num.
    cGate *ingate(const char *s);                           // input gate name
    int findingate(const char *s);                          // input gate num. by name
    cGate *outgate(int g) {return (cNetGate*)out_gatev[g];} // output gate by num.
    cGate *outgate(const char *s);                          // output gate by name
    int findoutgate(const char *s);                         // output gate num by name
    virtual void clear();

    // simulation control
    const char *localhost() {return my_host;}
                            // the namestr of the local host
    short start_segments(cArray& host_list,int argc,char* argv[]);
                            // starts up all the simulator on each host
    virtual int net_addgate(cModule *mod, int gate, char tp);
                            // creates a new gate in the PVM module

    virtual void send_runnumber(int run_nr);
    virtual int receive_runnumber(); // called from envir's TSlaveApp
    void setup_connections();        // sets up the connection between the PVM modules

    virtual void stop_all_segments();       // halts the simulation immediately
    virtual void request_stopsimulation();  // asks the console to stop simulation

};

#endif

