//=========================================================================
//
// PVMMOD.CC - part of
//                          OMNeT++
//           Discrete System Simulation in C++
//
//   Written by:  Zoltan Vass, Apr-June 1996
//                and Andras Varga, 1996-97
//                modifications by Gabor Lencse, July 1997
//   Contents:
//      Method definition of class cPvmMod, the interface module
//      for running simulation on PVM3 (Parallel Virtual Machine Version 3)
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992,99 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <string.h>
#include <stdio.h>
#include <pvm3.h>
#include "cmessage.h"
#include "cmodule.h"
#include "csimul.h"
#include "cstk.h"
#include "macros.h"
#include "ctypes.h"
#include "cenvir.h"
#include "cpar.h"
#include "pvmmod.h"


// SINGLE_HOST: if #defined, OMNeT++ runs all segments of a
// distributed simulation on the local host (good for debugging).
// Still, you have to list *different* host names in the ini file.
//#define SINGLE_HOST

// PVM_DEBUG: enables a lot of ev.printf()s
//#define PVM_DEBUG

// Register_Class is necessary because Envir instantiates cPvmMod
// via createOne("cPvmMod")
Register_Class( cPvmMod );


extern int pack_str(const char * str);
extern char *upack_str(int& err);
extern cObject *upack_object(int& err);

//=========================================================================
int is_started_as_master()
{
    // test PVM
    if (pvm_mytid()<=0)
    {
         ev.printfmsg("PVM not running");
         return STARTUPERROR_MODE;
    }
    else
    {
         // if we have a parent, then we are running in slave mode
         return pvm_parent()>0 ? SLAVE_MODE : MASTER_MODE;
    }
}

//=========================================================================
// cNetGate
//=========================================================================

// The copy constructor.
cNetGate::cNetGate(cNetGate& gate) : cGate(NULL,0)
{
    operator=(gate);
}

// The constructor.
cNetGate::cNetGate(const char *name, int type) : cGate(name,type)
{
   target_Proc=0;
   target_Gate=-1;
}

// The copy operator (though unnecessary in the case of a network gate)
cNetGate& cNetGate::operator=(cNetGate& gate)
{
   if (this==&gate) return *this;

   cGate::operator=(gate);
   target_Proc=gate.target_Proc;
   target_Gate=gate.target_Gate;
   return *this;
}

//=========================================================================
// cPvmMod
//=========================================================================

// The constructor
cPvmMod::cPvmMod()
{
    num_ingates=0;
    num_outgates=0;
    all_tids=NULL;
    segments=0;
    syncpoints=NULL;

    // get my TID
    my_tid=pvm_mytid();

#ifdef SINGLE_HOST
    ev.printf("SINGLE_HOST #defined in sim/pvm/pvmmod.cc:"
              " all segments placed on local host.\n");
#endif

    // join our PVM group
    if (pvm_joingroup("omnetpp_group")<0)
        {opp_error("PVM: Cannot join `omnetpp_group' (pvmgs not running?)");return;}

    // get local daemon's id and VM configuration
    int my_daemon=pvm_tidtohost(my_tid);
    int arch_cnt=0;
    pvm_config(&host_cnt,&arch_cnt,&hosts);
    parent_tid=pvm_parent();

    // get name of this host
    int i=0;
    while (my_daemon!=hosts[i].hi_tid) i++;
    my_host = hosts[i].hi_name;

    if (ev.runningMode()==SLAVE_MODE)
    {
#ifdef SINGLE_HOST
        // pretend to be running on host specified in first command-line arg
        my_host = ev.argv[1];
#endif
        // Receive the tids of the other segments, sent by
        //   start_segments() on the console
        ev.printf("Receiving TIDs of other segments...\n");

        pvm_recv(parent_tid,MSG_BROADCAST_TIDS);
        pvm_upkint(&segments,1,1);
        all_tids=new int[segments];
        pvm_upkint(all_tids,segments,1);
    }

}

//-------------------------------------------------------------------------
// Destructor function
cPvmMod::~cPvmMod()
{
    pvm_exit();
    delete all_tids;
}

//-------------------------------------------------------------------------
// info: Simple information
void cPvmMod::info(char *buf)
{
    sprintf(buf,"%-15.15s (%s,#%d)", fullName(), className(), id() );
}

//-------------------------------------------------------------------------
// callInitialize() and callFinish(). cPvmMod doesn't use initialize() or
// finish() at the moment, but these functions are provided here nevertheless.
//
void cPvmMod::callInitialize()
{
    // This is the interface for calling initialize().
    // We switch to the module's context for the duration of the call.

    cModule *oldcontext = simulation.contextModule();
    simulation.setContextModule( this );

    initialize();

    if (oldcontext)
        simulation.setContextModule( oldcontext );
    else
        simulation.setGlobalContext();
}

void cPvmMod::callFinish()
{
    // This is the interface for calling finish().
    // We switch to the module's context for the duration of the call.

    cModule *oldcontext = simulation.contextModule();
    simulation.setContextModule( this );

    finish();

    if (oldcontext)
        simulation.setContextModule( oldcontext );
    else
        simulation.setGlobalContext();
}

//-------------------------------------------------------------------------
// start_segments:
//    The console module checks whether all PVM hosts are running and
//    starts up each segment.

short cPvmMod::start_segments(cArray& host_list,int argc,char * argv[])
{
    int i,j;
    int err=0;
    const char *act_host;

    if (ev.runningMode()!=MASTER_MODE)
    {
       opp_error("start_segments() can only be called on the console");
       return false;
    }

#ifndef SINGLE_HOST
    // Check if PVM daemon is running on all hosts we need.
    ev.printf("Checking if needed hosts are in the VM...\n");
    for (i=0;i<host_list.items();i++)
    {
        // one host we need
        act_host = ((cPar *)host_list[i])->stringValue();
        ev.printf("  %s\n",act_host);

        // is it in the hosts[] array returned by PVM?
        for(j=0; j<host_cnt; j++)
           if (opp_strcmp(act_host,hosts[j].hi_name)==0)
              break;
        if (j==host_cnt)
        {
           opp_error("PVM: Host `%s' not in virtual machine",act_host);
           return false;
        }
    }
#endif /*SINGLE_HOST*/

#ifndef SINGLE_HOST
    // NORMAL version:
    // prepare an argv2[] for the slaves.
    //  - vector elements are shifted, because argv2[0] is supposed to be
    //    the first argument and NOT the program name.
    //  - argv2[] must be NULL-terminated.
    char **argv2 = new char *[argc];
    for (i=1; i<argc; i++) argv2[i-1] = argv[i];
    argv2[argc-1]=NULL;
#else
    // SINGLE_HOST version:
    // reserve space for an extra argv2[0] command-line argument
    //  - we pass the 'faked' host name to the slaves
    //    as the first command-line arg, argv[0].
    char **argv2 = new char *[argc+1];
    for (i=1; i<argc; i++) argv2[i] = argv[i];
    argv2[argc]=NULL;
#endif /*SINGLE_HOST*/

    // Start up segments and collect their TIDs into all_tids[]
    segments=0;
    ev.printf("Starting up segments...\n");
    all_tids = new int[host_list.items()];
    for(i=0; i<host_list.items(); i++)
    {
       // pick host i
       act_host = ((cPar *)host_list[i])->stringValue();

       // check if it is a duplicate
       for(j=0; j<i; j++)
           if (opp_strcmp(act_host,((cPar *)host_list[j])->stringValue())==0)
              break;

       if (i!=j)
       {
           // duplicate -- already handled
           ev.printf("  host %s: already started\n",act_host);
       }
       else if (opp_strcmp(my_host,act_host)==0)
       {
           ev.printf("  host %s: this segment, TID=0x%x\n",act_host,my_tid);
           all_tids[segments]=my_tid;
           segments++;
       }
       else
       {
#ifdef SINGLE_HOST
           // fill up first command-line arg with host name
           argv2[0]=act_host;
#endif
           // print out command line
           ev.printf("  host %s:",act_host);
           ev.printf(" running '%s",argv[0]);
           for(j=0; argv2[j]; j++)
               ev.printf(" %s",argv2[j]);
           ev.printf("'");

           // start up the slave program
#ifndef SINGLE_HOST
           // NORMAL: on act_host
           err=pvm_spawn(argv[0],argv2,PvmTaskHost,CONST_CAST(act_host),1,all_tids+segments);
#else
           // SINGLE_HOST: on my_host
           err=pvm_spawn(argv[0],argv2,PvmTaskHost,my_host,1,all_tids+segments);
#endif

           if (err<=0)
           {
               // error, kill segments started up so far
               ev.printf(": ERROR, code=%i\n",(all_tids+segments)[0]);
               opp_error("PVM: Could not start segment");
               for (j=0; j<segments; j++)
                   pvm_kill(all_tids[j]);
               return false;
           }
           else
           {
               ev.printf(": TID=0x%x\n", all_tids[segments]);
               segments++;
           }
       }
    }
    ev.printf("Total %d segments counted.\n",segments);
    delete argv2;

    // Now that all the segments have started,
    //   we'll broadcast all TIDs to everyone.
    ev.printf("Broadcasting all TIDs to other segments...\n");
    pvm_initsend(PvmDataDefault);
    pvm_pkint( &segments,1,1 );
    pvm_pkint( all_tids, segments, 1);
    pvm_mcast( all_tids, segments, MSG_BROADCAST_TIDS );
    return true;
}

//-------------------------------------------------------------------------
// setup_connection:
//   the PVM module broadcast the full path name of its
//   output gates together with its TID.
//   Then it collects the info arrived from the other hosts, and sets up the
//   connection in the input gates. A PVM connection is identified by the TID
//   and the output gate number of the other PVM module.

void cPvmMod::setup_connections()
{
    ev.printf("Setting up connections across segments...\n");
    char ***temp_link_table = new char **[segments-1];
    int i;
    int err;
    int *segm_tids = new int[segments-1];
    int *segm_numgates =  new int[segments-1];

    // Broadcast the output gate info:
#ifdef PVM_DEBUG
    ev.printf("Broadcasting info about our gates: my_tid=0x%x, num_outgates=%i\n",
          my_tid,num_outgates);
#endif
    err=0;
    pvm_initsend(PvmDataDefault);
    err|=pvm_pkint(&my_tid,1,1);
    err|=pvm_pkint(&num_outgates,1,1);
    for (i=0;i<num_outgates;i++)
    {
#ifdef PVM_DEBUG
      ev.printf("  out_gatev[%i]=%s\n",i,out_gatev[i]->name());
#endif
      err|=pack_str(out_gatev[i]->name()); // --LG
    }
    // (note: though our TID is in all_tids, PVM will NOT send to ourselves)
    pvm_mcast(all_tids,segments,MSG_SETUP_LINKS);
    if (err) opp_error(ePVM,"setup_connections()/sending");

    // Collect from the others:
#ifdef PVM_DEBUG
    ev.printf("... and collecting info from the others...\n");
#endif
    for (i=0;i<segments-1;i++)
    {
        err = 0;
        pvm_recv(-1,MSG_SETUP_LINKS); // collect msgs into separate buffers
        err|=pvm_upkint(segm_tids+i,1,1);
        err|=pvm_upkint(segm_numgates+i,1,1);
#ifdef PVM_DEBUG
        ev.printf("  segm_tids[%i]=0x%x  segm_numgates[%i]=%d\n",i,segm_tids[i],i,segm_numgates[i]);
#endif
        if (err) opp_error(ePVM,"setup_connections()/recv header");
        temp_link_table[i] = new char *[segm_numgates[i]];
        for (int j=0;j<segm_numgates[i];j++) {
            err=0;
            temp_link_table[i][j]=upack_str(err); // --LG
            if (err) opp_error(ePVM,"setup_connections()/recv gatename");
#ifdef PVM_DEBUG
            ev.printf("    %s\n",temp_link_table[i][j]);
#endif
        }
    }

    // Search for output gates that have the same full pathname as ones
    // in the temporary link table, which contains all the input gates
    // of all the PVM modules.
    int sernum=0;
    for (i=0;i<segments-1;i++)
    {
       for (int j=0;j<segm_numgates[i];j++)
       {
          if ((sernum=gatev.find(temp_link_table[i][j]))>=0)
              ((cNetGate *)gatev[sernum])->settarget(segm_tids[i],j);
       }
    }

    // free the temp_link_table...
    for (i=0; i<segments-1; i++)
    {
        for (int j=0; j<segm_numgates[i]; j++) // --LG
            delete temp_link_table[i][j]; // --LG
        delete[] temp_link_table[i];
    }
    delete[] temp_link_table;
    delete[] segm_tids;
    delete[] segm_numgates;

#ifdef PVM_DEBUG
    //ev.printf("Synchronizing at end of connection setup...\n");
#endif
    //synchronize();
}

//-------------------------------------------------------------------------
// sync_after_modinits:
//   Called after all simple modules have processed their activation cMsgs
//   (that is, activity()s have been started) but before any 'real' event
//   has been processed.
//   Blocks until all segments have reached this point and then registers
//   synchronisation points (that came from syncpoint() calls at the top of
//   simple mods' activity() functions).
//-------------------------------------------------------------------------
void cPvmMod::sync_after_modinits()
{
    ev.printf("Blocking until all module activity()s have been started...\n");
    synchronize();
    ev.printf("Continuing...\n");
    // the following call is needed to process syncpoint calls
    process_netmsgs();
}

//-------------------------------------------------------------------------
// arrived:
//   Overrides the virtual function in cSimpleModule. The message will
//   be sent out immediately to the other PVM module.

void cPvmMod::arrived(cMessage *msg,int ongate)
{
    net_sendmsg( msg, ongate);
}

//-------------------------------------------------------------------------
// net_addgate:
//   When a new network connection is encountered during the
//   net creation, the PVM module creates a new gate for it.
//   The input and output gates are in separate lists.

int cPvmMod::net_addgate(cModule * mod,int gate, char tp)
{
    int retval=0;
    cNetGate *newg = new cNetGate( mod->gate(gate)->fullPath(), tp);
    if (tp=='I')   // 'I': input gate
    {
       retval=gatev.add( newg );
       num_ingates++;
    }
    else           // 'O': output gate
    {
       retval=out_gatev.add( newg );
       num_outgates++;
    }
    newg->setOwnerModule(this, retval);
    return retval;   // index in gatev[] or out_gatev[]
}

//-------------------------------------------------------------------------
// net_sendmsg:
//   Packs the cMessage object to a PVM buffer, and sends to the
//   appropriate host, with the serial number of the output gate.

void cPvmMod::net_sendmsg(cMessage *msg,int ongate)
{
    int gate_num = ((cNetGate *)gatev[ongate])->t_gate();
    int pvm_dest = ((cNetGate *)gatev[ongate])->t_proc();

    int err=0;
    pvm_initsend(PvmDataDefault);
    err|=pvm_pkint(&gate_num,1,1);
    err|=msg->netPack();
    err|=pvm_send(pvm_dest,MSG_SIMULATION_CMSG);
    if (err) opp_error(ePVM,"net_sendmsg()");
    delete msg;
}

//-------------------------------------------------------------------------
// putmsg_onconsole:
//    send a message string to the PVM module on the console to display.

void cPvmMod::putmsg_onconsole(const char *strmsg)
{
    int err=0;
    pvm_initsend(PvmDataDefault);
    err=err||pack_str(my_host);
    err=err||pack_str(strmsg);
    err=err||pvm_send(parent_tid, MSG_PUTMSG_ONCONSOLE);
    if (err) opp_error(ePVM, "putmsg_onconsole()");
}

//-------------------------------------------------------------------------
// puts_onconsole:
//    send a string to the PVM module on the console

void cPvmMod::puts_onconsole(const char *strmsg)
{
    int err=0;
    pvm_initsend(PvmDataDefault);
    err=err||pack_str(my_host);
    err=err||pack_str(strmsg);
    cModule *mod = simulation.contextModule();
    err=err||pack_str(mod ? mod->fullPath() : "");
    err=err||pvm_send(parent_tid, MSG_PUTS_ONCONSOLE);
    if (err) opp_error(ePVM, "puts_onconsole()");
}


//-------------------------------------------------------------------------
// gets_onconsole:
//    Generates a user input on console, and waits for the results.

bool cPvmMod::gets_onconsole(const char *promptstr, char *buffer,int len)
{
    char a;
    int err=0;
    char *buff;
    pvm_initsend(PvmDataDefault);
    err=err||pack_str(my_host);
    err=err||pack_str(promptstr);
    err=err||pack_str(buffer);
    err=err||pvm_pkint(&len,1,1);
    err=err||pvm_pkint(&my_tid,1,1);
    err=err||pvm_send(parent_tid,MSG_GETS_ONCONSOLE);
    if (err) {opp_error(ePVM, "gets_onconsole()/send");return false;}

    pvm_recv(parent_tid,MSG_GETS_ONCONSOLE);
    err=0;
    buff=upack_str(err);
    if (buff) strcpy(buffer,buff);
    err=err||pvm_upkbyte(&a,1,1);
    if (err) {opp_error(ePVM, "gets_onconsole()/recv");return false;};
    return a!=0;
}

//-------------------------------------------------------------------------
// askyesno_onconsole:
//    Generates a user input on console, and waits for the results.

bool cPvmMod::askyesno_onconsole(const char *question)
{
    int err=0;
    pvm_initsend(PvmDataDefault);
    err=err||pack_str(my_host);
    err=err||pack_str(question);
    err=err||pvm_pkint(&my_tid,1,1);
    err=err||pvm_send(parent_tid,MSG_ASKYESNO_ONCONSOLE);
    if (err) {opp_error(ePVM, "askyesno_onconsole()/send");return false;}

    err=0;
    pvm_recv(parent_tid,MSG_ASKYESNO_ONCONSOLE);
    char a;
    err=err||pvm_upkbyte(&a,1,1);
    if (err) {opp_error(ePVM, "askyesno_onconsole()/recv");return false;}
    return a!=0;
}


//-------------------------------------------------------------------------
// clear: clears all the gates in th the PVM module
//-------------------------------------------------------------------------
void cPvmMod::clear()
{
    gatev.clear();
    out_gatev.clear();
}

//-------------------------------------------------------------------------
// stopallsegments:
//   send STOPSIM to all the segments
//-------------------------------------------------------------------------
void cPvmMod::stop_all_segments()
{
    ev.printf("Stopping simulation execution on all segments...\n");
    int err=0;
    pvm_initsend(PvmDataDefault);
    err|=pack_str(my_host);
    err|=pvm_mcast(all_tids,segments,MSG_STOPSIM);
    if (err) ev.printfmsg("PVM error in stop_all_segments()");
    // It would be disastrous to call opp_error() on error,
    // because we were called from there! If we call it back,
    // the result is most probably an endless recursion.
    ////opp_error(ePVM, "stop_all_segments()");
}

//-------------------------------------------------------------------------
// request_stopsimulation:
//   If the module is on the console, sends stopsim message to all the others
//   and stops itself. If it's not, just notify the console. The console
//   itself will send a stopsim message.
// CURRENTLY NOT USED: cSimulation::error() calls stop_all_segments() directly.
//-------------------------------------------------------------------------
void cPvmMod::request_stopsimulation()
{
    if (ev.runningMode()==MASTER_MODE)
    {
        stop_all_segments();
    }
    else
    {
        int err=0;
        pvm_initsend(PvmDataDefault);
        err|=pack_str(my_host);
        err|=pvm_send(parent_tid,MSG_REQUEST_STOPSIM);
        if (err) opp_error(ePVM, "request_stopsimulation()");
    }
}

//==========================================================================
// process_netmsgs(): processes messages currently in the PVM reveice buffers
//==========================================================================
void cPvmMod::process_netmsgs()
{
    int rbuff;
    while ((rbuff=pvm_nrecv(-1,-1))>0)
            do_process_netmsg(rbuff);
    if (rbuff<0) opp_error(ePVM,"process_netmsgs()");
}

//==========================================================================
// process_netmsgs(): waits for msgs to arrive and processes them
//==========================================================================
void cPvmMod::process_netmsg_blocking()
{
    int rbuff=pvm_recv(-1,-1);
    if (rbuff>0)
        do_process_netmsg(rbuff);
    else
        opp_error(ePVM,"process_netmsg_blocking()");
}

//==========================================================================
// do_process_netmsgs(): actual work for the previous two funcs
//==========================================================================
void cPvmMod::do_process_netmsg(int rbuff)
{
    int err=0;
    int gate_num;
    double t;
    int type,tmp1,tmp2;
    char *hostname;
    char *str, *mod;
    char *promptstr;
    char *buff;
    int srctid;
    int length;
    char res;
    cNetGate *netg;
    cMessage *msg;

    pvm_bufinfo(rbuff,&tmp1,&type,&tmp2);
    switch (type)
    {
        // cMessage for a simple module
        case MSG_SIMULATION_CMSG:
            err=0;
            err|=pvm_upkint(&gate_num,1,1);
            msg = (cMessage *)upack_object(err);
            if (err)
                {opp_error(ePVM, "do_process_netmsg()/unpacking the message");break;}

            del_syncpoint(msg->arrivalTime(),gate_num);

            netg = (cNetGate *)out_gatev[gate_num];
            if (netg->toGate()==NULL)
                {opp_error(eNODEL);break;}

            if (msg->arrivalTime() < simulation.simTime())
                {opp_error("Arrival time of cMessage from another segment already passed");break;}

            netg->deliver(msg);
            break;

        // syncpoint request
        case MSG_SYNCPOINT:
            pvm_upkdouble(&t,1,1);
            pvm_upkint(&gate_num,1,1);
            add_syncpoint(t,gate_num);
            break;

        // cancelsyncpoint request
        case MSG_CANCELSYNCPOINT:
            pvm_upkdouble(&t,1,1);
            pvm_upkint(&gate_num,1,1);
            cancel_syncpoint(t,gate_num);
            break;

        // A message to display on the console host.
        case MSG_PUTMSG_ONCONSOLE:
            err=0;
            hostname=upack_str(err);
            str=upack_str(err);
            if (!err)
                ev.printfmsg("Host `%s': %s", hostname, str);
            else
                opp_error("PVM: Bad console message");
            break;

        // Module or info message to display on the console host.
        case MSG_PUTS_ONCONSOLE:
            err=0;
            hostname=upack_str(err);
            str=upack_str(err);
            mod=upack_str(err);
            if (!err)
                ev.foreignPuts(hostname, mod, str);
            else
                opp_error("PVM: Bad console message");
            break;

        // A message which prompts for user input on the console
        case MSG_GETS_ONCONSOLE:
            err=0;
            hostname=upack_str(err);
            promptstr=upack_str(err);
            buff=upack_str(err);
            if (!buff) {buff=new char[80];buff[0]=0;};
            err=err||pvm_upkint(&length,1,1);
            err=err||pvm_upkint(&srctid,1,1);
            if (err) opp_error(ePVM, "do_process_netmsg()/remote gets() req");

            res=ev.gets(promptstr,buff,length); // hostname?
            pvm_initsend(PvmDataDefault);
            err=0;
            err=err||pack_str(buff);
            err=err||pvm_pkbyte(&res,1,1);
            err=err||pvm_send(srctid,MSG_GETS_ONCONSOLE);
            if (err) opp_error(ePVM,"do_process_netmsg()/answering remote gets()");
            break;

        case MSG_ASKYESNO_ONCONSOLE:
            err=0;
            hostname=upack_str(err);
            promptstr=upack_str(err);
            err=err||pvm_upkint(&srctid,1,1);
            if (err) opp_error(ePVM, "do_process_netmsg()/remote askyesno() request");

            res=ev.askYesNo(promptstr);
            pvm_initsend(PvmDataDefault);
            err=0;
            err=err||pvm_pkbyte(&res,1,1);
            err=err||pvm_send(srctid,MSG_ASKYESNO_ONCONSOLE);
            if (err) opp_error(ePVM,"do_process_netmsg()/answering remote askyesno()");
            break;

        // A message to halt the simulation.
        case MSG_STOPSIM:
            //if (ev.runningMode()==MASTER_MODE)
            //{
                hostname=upack_str(err);
                opp_terminate(eSTOPSIMRCVD, hostname);
                   // CAUTION: The error code eSTOPSIMRCVD must be kept!
                   //  (it is checked at other places in sim
                delete [] hostname;
            //}
            break;

        // A message or query to stop the simulation.
        case MSG_REQUEST_STOPSIM:
            if (ev.runningMode()==MASTER_MODE)
            {
                hostname=upack_str(err);
                ev.printfmsg("Stopping simulation by request of segment on host `%s'", hostname);
                delete [] hostname;
                stop_all_segments();
            }
            break;
        default:
            opp_error("do_process_netmsg(): bad PVM message tag %i",type); // --LG
    }
}

//==========================================================================
// send_syncpoint()
//  called by cSimpleModule::syncpoint()
//==========================================================================

void cPvmMod::send_syncpoint( simtime_t t, int ongate)
{
    int pvm_dest = ((cNetGate *)gatev[ongate])->t_proc();
    int gate_num = ((cNetGate *)gatev[ongate])->t_gate();

#ifdef PVM_DEBUG
    ev.printf("Sending SYNCPOINT to 0x%x: gate=%d  t=%lf\n",pvm_dest,gate_num,t);
#endif

    int err = 0;
    pvm_initsend(PvmDataDefault);
    err|=pvm_pkdouble(&t,1,1);
    err|=pvm_pkint(&gate_num,1,1);
    err|=pvm_send(pvm_dest,MSG_SYNCPOINT);
    if (err)  opp_error(ePVM, "send_syncpoint()");
}

//==========================================================================
// send_cancelsyncpoint()
//  called by cSimpleModule::cancelSyncpoint()
//==========================================================================

void cPvmMod::send_cancelsyncpoint( simtime_t t, int ongate)
{
    int pvm_dest = ((cNetGate *)gatev[ongate])->t_proc();
    int gate_num = ((cNetGate *)gatev[ongate])->t_gate();

#ifdef PVM_DEBUG
    ev.printf("Sending CANCELSYNCPOINT to 0x%x: gate=%d\n",pvm_dest,gate_num);
#endif

    int err = 0;
    pvm_initsend(PvmDataDefault);
    err|=pvm_pkdouble(&t,1,1);
    err|=pvm_pkint(&gate_num,1,1);
    err|=pvm_send(pvm_dest,MSG_CANCELSYNCPOINT);
    if (err)  opp_error(ePVM, "send_cancelsyncpoint()");
}

//==========================================================================
// block_on_syncpoint()
//  blocks on next syncpoint if necessary
//  returns true if actually blocked
//          false if nothing happened
//==========================================================================

bool cPvmMod::block_on_syncpoint( simtime_t nextlocalevent)
{
    if (nextlocalevent>next_syncpoint())
    {
#ifdef PVM_DEBUG
       ev.printf("Blocking on SYNCPOINT t=%lf (next local event: t=%lf)\n",
              next_syncpoint(), nextlocalevent);
#endif
       // wait until syncpoint gets deleted by a cMessage from another segment
       while (nextlocalevent>next_syncpoint())
       {
            int rbuff=pvm_recv(-1,-1);
            if (rbuff<0) {opp_error(ePVM,"block_on_syncpoint()");break;}
            do_process_netmsg(rbuff);
       }
#ifdef PVM_DEBUG
       ev.printf("Continuing after SYNCPOINT\n");
#endif
       return true;
    }
    return false;
}

//==========================================================================
// add_syncpoint()
//  add a syncpoint to internal list
//==========================================================================
void cPvmMod::add_syncpoint( simtime_t t, int ongate)
{
    sSyncPoint **pp = &syncpoints;
    while (*pp && t>(*pp)->t)
       pp = &((*pp)->next);
    sSyncPoint *rest = *pp;
    *pp = new sSyncPoint;
    (*pp)->t = t;
    (*pp)->gate = ongate;
    (*pp)->next = rest;
#ifdef PVM_DEBUG
    ev.printf("Added SYNCPOINT: gate=%d  t=%lf, current list:\n",ongate,t);
    sSyncPoint *p = syncpoints;
    while (p) {
       ev.printf("  gate=%d  t=%lf\n",p->gate,p->t);
       p=p->next;
   }
#endif
}

//==========================================================================
// del_syncpoint()
//  delete a syncpoint from internal list when a msg has arrived
//==========================================================================
void cPvmMod::del_syncpoint(simtime_t msgtime, int gate)
{
#ifdef PVM_DEBUG
    ev.printf("del_syncpoint(): gate=%d  t=%lf\n",gate,msgtime);
#endif
    sSyncPoint **pp = &syncpoints;
    while (*pp && (*pp)->gate!=gate)
        pp = &((*pp)->next);

    if (!*pp)
    {
        opp_warning("cMessage from another segment received with no syncpoint");
        return;
    }
    if ((*pp)->t > msgtime)
        opp_warning("cMessage from another segment arrived before first syncpoint");

    sSyncPoint *rest = (*pp)->next;
#ifdef PVM_DEBUG
    ev.printf("Deleting SYNCPOINT: gate=%d  t=%lf\n",(*pp)->gate,(*pp)->t);
#endif
    delete *pp;
    *pp = rest;
}

//==========================================================================
// cancel_syncpoint()
//  delete syncpoint from internal list because of a cancelSyncpoint() call
//==========================================================================
void cPvmMod::cancel_syncpoint(simtime_t t, int gate)
{
#ifdef PVM_DEBUG
    ev.printf("cancel_syncpoint(): gate=%d  t=%lf\n",gate,t);
#endif
    sSyncPoint **pp = &syncpoints;
    while (*pp && ((*pp)->gate!=gate || (*pp)->t!=t))
        pp = &((*pp)->next);

    if (!*pp)
    {
        opp_warning("Received cancelSyncpoint(): no such syncpoint or already passed");
        return;
    }

    sSyncPoint *rest = (*pp)->next;
#ifdef PVM_DEBUG
    ev.printf("Deleting SYNCPOINT: gate=%d  t=%lf\n",(*pp)->gate,(*pp)->t);
#endif
    delete *pp;
    *pp = rest;
}

//==========================================================================
// next_syncpoint()
//  return time of next syncpoint
//==========================================================================
simtime_t cPvmMod::next_syncpoint()
{
    return syncpoints ? syncpoints->t : MAXTIME;
}

//==========================================================================
// synchronize()
//   wrapper around pvm_barrier(). Synchronizes the pvm modules.
//   All modules have to call this function to cross the barrier.
//==========================================================================
void cPvmMod::synchronize()
{
#ifdef PVM_DEBUG
    ev.printf("synchronize() called...\n");
#endif
    if (pvm_barrier("omnetpp_group",segments)<0)
        opp_error(ePVM,"synchronize()");
#ifdef PVM_DEBUG
    ev.printf("end synchronize().\n");
#endif
}

//==========================================================================
// The next 4 functions are necessary to be defined because of the separate
// input and output gate lists.
//==========================================================================

//--------------------------------------------------------------------------
// findingate : search for serial num of input gate by its name

int cPvmMod::findingate(const char *s)
{
    return gatev.find(s);
}

//--------------------------------------------------------------------------
// ingate: reference to input gate with name s.

cGate *cPvmMod::ingate(const char *s)
{
    int i=gatev.find(s);
    if (i==-1)
       {opp_warning(eNULLREF,className(),fullPath()); return NO(cNetGate);}
    else
       return (cNetGate *)gatev.get(i);
}

//--------------------------------------------------------------------------
// findoutgate : search for serial num of output gate by its name

int cPvmMod::findoutgate(const char *s)
{
    return out_gatev.find(s);
}

//--------------------------------------------------------------------------
// outgate: reference to output gate with name s.

cGate *cPvmMod::outgate(const char *s)
{
    int i=out_gatev.find(s);
    if (i==-1)
       {opp_warning(eNULLREF,className(),fullPath()); return NO(cNetGate);}
    else
       return (cNetGate *)out_gatev[i];
}

//==========================================================================

int cPvmMod::receive_runnumber()
{
    ev.printf("Waiting for run number...\n");
    //synchronize();

    pvm_recv(parent_tid,MSG_RUNNUMBER);
    int run_nr;
    int err=pvm_upkint(&run_nr,1,1);
    if (err) {opp_error(ePVM,"receive_runnumber()");return -1;}

    ev.printf("Got run number: %d\n",run_nr);
    return run_nr;
}

void cPvmMod::send_runnumber(int run_nr)
{
     ev.printf("Sending run number %d to other segments...\n",run_nr);
     //synchronize();

    int err=0;
    pvm_initsend(PvmDataDefault);
    err|=pvm_pkint(&run_nr,1,1);
    err|=pvm_mcast(all_tids,segments,MSG_RUNNUMBER);
    if (err) opp_error(ePVM,"send_runnumber()");
}

