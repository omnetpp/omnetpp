//=========================================================================
//
// MPIMOD.H - part of
//                          OMNeT++
//           Discrete System Simulation in C++
//
//   Written by:  Eric Wu, 2001
//
//   Contents:
//      Method definition of class cMpiMod, the interface module
//      for running simulation on MPI (Message Passing Interface)
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2001 Eric Wu
  Monash University, Dept. of Electrical and Computer Systems Eng.
  Melbourne, Australia

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> /* FIXME */
#include <sys/param.h>
#include "cmessage.h"
#include "cmodule.h"
#include "csimul.h"
#include "macros.h"
#include "ctypes.h"
#include "cenvir.h"
#include "cpar.h"
#include "mpimod.h"
#include "mpipack.h"

// Register_Class is necessary because Envir instantiates cMpiMod
// via createOne("cMpiMod")
Register_Class( cMpiMod );

extern cObject *upack_object(int& err);

//=========================================================================
int is_started_as_master()
{
	printf("is_started_as_master\n");
    int argc = 0;
    char** argv = 0;

    MPI_Init(&argc, &argv);

    int my_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    // test MPI
    if (my_rank<0)
    {
         ev.printfmsg("MPI not running");
         return STARTUPERROR_MODE;
    }
    else
    {
         // if we have a parent, then we are running in slave mode
         return my_rank>0 ? SLAVE_MODE : MASTER_MODE;
    }
}

//=========================================================================
// cNetGate
//=========================================================================

// The copy constructor.
cNetGate::cNetGate(const cNetGate& gate) : cGate(NULL,0)
{
	printf("cNetGate() constructor\n");
    operator=(gate);
}

// The constructor.
cNetGate::cNetGate(const char *name, int type) : cGate(name,type)
{
	printf("cNetGate() constructor\n");
   target_Proc=0;
   target_Gate=-1;
}

// The copy operator (though unnecessary in the case of a network gate)
cNetGate& cNetGate::operator=(cNetGate& gate)
{
	printf("cNetGate() operator=()\n");
   if (this==&gate) return *this;

   cGate::operator=(gate);
   target_Proc=gate.target_Proc;
   target_Gate=gate.target_Gate;
   return *this;
}

//=========================================================================
// cMpiMod
//=========================================================================

// The constructor
cMpiMod::cMpiMod()
{
	printf("Rank %d, cMpiMod::constructor\n", mMy_Rank);
    mNum_Ingates=0;
    mNum_Outgates=0;
    syncpoints=NULL;
    segments=0;

    MPI_Comm_size(MPI_COMM_WORLD, &mSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &mMy_Rank);

    pack = cMpiPack::instance();

// --> ** TODO
// Better to use an MPI function... but what is it?
	char *hostname = new char[MAXHOSTNAMELEN];

	if (gethostname(hostname, MAXHOSTNAMELEN) == 0)
		my_host = hostname;
	else
		my_host = NULL;

	printf("hostname: %s\n", hostname);
	delete [] hostname;
	printf("my_host: %s\n", (const char*)my_host);
// <-- **

    int data, status;

    if(ev.runningMode()==SLAVE_MODE)
    {
	    	printf("Rank %d, calling recv_pack\n", mMy_Rank);
      pack->recv_pack(MPIMASTER, MPIMSG_BROADCAST);
	    	printf("Rank %d, finished calling recv_pack\n", mMy_Rank);
      pack->unpack_data(&data, MPI_INT);

      if(data == MPIMSG_OK)
      {
        pack->pack_data(&data, MPI_INT);
        pack->send_pack(MPIMASTER, MPIMSG_ACK);
      }
      else
        ev.printf("SLAVE: process not initialised. . .");
    }
}

//-------------------------------------------------------------------------
// Destructor function
cMpiMod::~cMpiMod()
{
	printf("Rank %d, cMpiMod::destructor\n", mMy_Rank);
// ** --> TODO
// Uncomment after major debugging has been done.
//  if (pack)
//	delete pack;
// <-- **
MPI_Finalize();
}

//-------------------------------------------------------------------------
// multicast() sends the data to all other processes except itself

int cMpiMod::multicast(int tag, MPI_Comm comm)
{
	printf("Rank %d cMpiMod::multicast()\n", mMy_Rank);
  int status = MPI_SUCCESS;

  for(int i=0; i<mSize; i++)
  {
    if(mMy_Rank != i )
    {
      status = pack->send_pack(i, tag, false);
      if(status!=MPI_SUCCESS)
      {
        ev.printf("Process ID %d: Cannot multicast the data. . . ", i);
        return status;
      }
    }
  }
  // Remove the sending buffer
  pack->remove_buffer();

  return status;
}


//-------------------------------------------------------------------------
// info: Simple information
void cMpiMod::info(char *buf)
{
	printf("Rank %d cMpiMod::info()\n", mMy_Rank);
    sprintf(buf,"%-15.15s (%s,#%d)", fullName(), className(), id() );
}

//-------------------------------------------------------------------------
// callInitialize() and callFinish(). cMpiMod doesn't use initialize() or
// finish() at the moment, but these functions are provided here nevertheless.
//

void cMpiMod::callInitialize()
{
	printf("Rank %d cMpiMod::callInitialize()\n", mMy_Rank);
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

void cMpiMod::callFinish()
{
	printf("Rank %d cMpiMod::callFinish()\n", mMy_Rank);
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
//    The console module checks whether all MPI processes are running and
//    starts up each segment.

short cMpiMod::start_segments(cArray& host_list,int argc,char * argv[])
{
	printf("Rank %d cMpiMod::start_segments()\n", mMy_Rank);
    if (ev.runningMode()!=MASTER_MODE)
      throw new cException("start_segments() can only be called on Master process");

    int broadcast = MPIMSG_OK;
    // multicast the message to ensure all processors are running
    pack->pack_data(&broadcast, MPI_INT);
    multicast(MPIMSG_BROADCAST);

    int ack;
    for(int i=0; i<mSize-1; i++) // No. of slave processes = total processes - 1 master process
    {
      // Master process is always at rank and the rest of processes are slaves
      pack->recv_pack(i+1, MPIMSG_ACK);
      pack->unpack_data(&ack, MPI_INT);
      if(ack == MPIMSG_OK)
        ev.printf("Slave process ID %d is running... \n", i+1);
      else
        throw new cException("MPI: process ID %d is failed to start \n", i+1);
    }
    return true;
}

//-------------------------------------------------------------------------
// setup_connection:
//   the MPI module broadcast the full path name of its
//   output gates together with its process ID.
//   Then it collects the info arrived from the other hosts, and sets up the
//   connection in the input gates. A MPI connection is identified by the rank

void cMpiMod::setup_connections()
{
	printf("Rank %d cMpiMod::setup_connections()\n", mMy_Rank);
	synchronize();
    ev.printf("Setting up connections across segments...\n");

    // status of the MPI functions
    MPI_Status status;
    char ***temp_link_table = new char **[mSize];
    char      *str, *tempStr;
	int        pack_length;
    int err = 0;
    int* segm_numgates =  new int[mSize];

    // sending information about my output gates
	err|=pack->pack_data(&mNum_Outgates, MPI_INT);
	for (int i = 0; i < mNum_Outgates; i++)
		err|=pack->pack_data((void*)mOutgates[i]->name(), MPI_CHAR, opp_strlen(mOutgates[i]->name()));

    multicast(MPIMSG_SETUP_LINK);
    if (err)
      throw new cException("setup_connections()/sending");


    // receiving information from others
    for (int i = 0; i < mSize; i++)
    {
      if(i != mMy_Rank)
      {
        err = 0;
        pack->recv_pack(i, MPIMSG_SETUP_LINK);
        err|=pack->unpack_data((void*)&segm_numgates[i], MPI_INT);
        if (err)
          throw new cException("setup_connections()/recv header");
        if(segm_numgates[i]!=0)
        {
          temp_link_table[i] = new char *[segm_numgates[i]];
          for (int j = 0; j < segm_numgates[i]; j++)
          {
            err=0;
            err|=pack->unpack_data((void**)&str, MPI_CHAR); // --LG
            temp_link_table[i][j]=str ; // --LG
printf("In rank: %d, receives from Rank %d , Gates[%d] is %s \n", mMy_Rank, i, j, temp_link_table[i][j]);
            if (err)
              throw new cException("setup_connections()/recv gatename");
          }
        }
      }
    }

    // Search for output gates that have the same full pathname as ones
    // in the temporary link table, which contains all the input gates
    // of all the MPI modules.
    int sernum=0;
    for (int i = 0; i < mSize; i++)
    {
      if(mMy_Rank != i)
      {
		for (int j = 0; j < segm_numgates[i]; j++)
        {
          if ((sernum=gatev.find(temp_link_table[i][j]))>=0)
          {
              ((cNetGate *)gatev[sernum])->settarget(i,j);
printf("FOUND!!! the matched gate info = %s with matrix, %d x %d \n", temp_link_table[i][j], i, j);
          }
        }
      }
    }

    // free the temp_link_table...
    for (int i = 0; i < mSize; i++)
    {
      if(mMy_Rank != i)
      {
        for (int j = 0; j < segm_numgates[i]; j++)
            delete [] temp_link_table[i][j]; // -- delete a string
        delete[] temp_link_table[i]; // delete array of strings
      }
    }

    delete[] temp_link_table; // delete array of process
    delete[] segm_numgates;
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
void cMpiMod::sync_after_modinits()
{
	printf("Rank %d cMpiMod::sync_after_modinits()\n", mMy_Rank);
    ev.printf("Blocking until all module activity()s have been started...\n");
    synchronize();
    ev.printf("Continuing...\n");
    // the following call is needed to process syncpoint calls
    process_netmsgs();
}

//-------------------------------------------------------------------------
// arrived:
//   Overrides the virtual function in cSimpleModule. The message will
//   be sent out immediately to the other MPI module.

void cMpiMod::arrived(cMessage *msg,int ongate)
{
	printf("Rank %dcMpiMod::arrived() fullPath() %s\n", mMy_Rank, fullPath());
    net_sendmsg( msg, ongate);
}

//-------------------------------------------------------------------------
// net_addgate:
//   When a new network connection is encountered during the
//   net creation, the MPI module creates a new gate for it.
//   The input and output gates are in separate lists.

int cMpiMod::net_addgate(cModule * mod,int gate, char tp)
{
	printf("Rank %d cMpiMod::net_addgate()\n", mMy_Rank);
    int retval=0;
    cNetGate *newg = new cNetGate( mod->gate(gate)->fullPath(), tp);
    if (tp=='I')   // 'I': input gate
    {
       retval=gatev.add( newg );
       mNum_Ingates++;
    }
    else           // 'O': output gate
    {
       retval=mOutgates.add( newg );
       mNum_Outgates++;
    }
    newg->setOwnerModule(this, retval);
    return retval;   // index in gatev[] or out_gatev[]
}

//-------------------------------------------------------------------------
// net_sendmsg:
//   Packs the cMessage object to a MPI buffer, and sends to the
//   appropriate host, with the serial number of the output gate.

void cMpiMod::net_sendmsg(cMessage *msg,int ongate)
{
	printf("Rank %d cMpiMod::net_sendmsg()\n", mMy_Rank);
    int gate_num = ((cNetGate *)gatev[ongate])->t_gate();
    int mpi_dest = ((cNetGate *)gatev[ongate])->t_proc();

    int err=0;

    pack->pack_data(&gate_num, MPI_INT);
    err|=msg->netPack();
printf("File %s, Line %d from Rank %d \n", __FILE__, __LINE__, mMy_Rank);
    err|=pack->send_pack(mpi_dest, MPIMSG_SIMULATION_CMSG);
    if (err)
      throw new cException("net_sendmsg()");
    delete msg;
}

//-------------------------------------------------------------------------
// putmsg_onconsole:
//    send a message string to the MPI module on the console to display.

void cMpiMod::putmsg_onconsole(const char *strmsg)
{
    int err=0;

	printf("Rank %d, putmsg_onconsole(): %s\n", mMy_Rank, strmsg);
    err=err||pack->pack_data(&mMy_Rank, MPI_INT);
    err = err||pack->pack_data((void*)strmsg, MPI_CHAR, opp_strlen(strmsg));
printf("File %s, Line %d from Rank %d \n", __FILE__, __LINE__, mMy_Rank);
    err=err||pack->send_pack(MPIMASTER, MPIMSG_PUTMSG_ONCONSOLE);
    if (err)
      throw new cException("putmsg_onconsole()");
}

//-------------------------------------------------------------------------
// puts_onconsole:
//    send a string to the MPI module on the console

void cMpiMod::puts_onconsole(const char *strmsg)
{
    int err=0;
	int   pack_length;

	printf("Rank %d puts_onconsole(): %s\n", mMy_Rank, strmsg);
    err=err||pack->pack_data(&mMy_Rank, MPI_INT);
    err=err||pack->pack_data((void*)strmsg, MPI_CHAR, opp_strlen(strmsg));

    cModule* mod = simulation.contextModule();
	pack_length  = mod ? opp_strlen(mod->fullPath()) : 0;
    err=err||pack->pack_data(const_cast<char *>(mod ? mod->fullPath() : "") , MPI_CHAR, pack_length);

printf("File %s, Line %d from Rank %d \n", __FILE__, __LINE__, mMy_Rank);
    err=err||pack->send_pack(MPIMASTER, MPIMSG_PUTS_ONCONSOLE);
    if (err)
      throw new cException("puts_onconsole()");
}


//-------------------------------------------------------------------------
// gets_onconsole:
//    Generates a user input on console, and waits for the results.

bool cMpiMod::gets_onconsole(const char *promptstr, char *buffer,int len)
{
    char a;
    int err=0;
    char* buff;

	printf("Rank %d cMpiMod::gets_onconsole()\n", mMy_Rank);
    // send to Master(console)

    err=err||pack->pack_data(&mMy_Rank, MPI_INT);
    err=err||pack->pack_data((void*)promptstr, MPI_CHAR, opp_strlen(promptstr));
    err=err||pack->pack_data((void*)buffer, MPI_CHAR, opp_strlen(buffer));
    err=err||pack->pack_data(&len, MPI_INT);
printf("File %s, Line %d from Rank %d \n", __FILE__, __LINE__, mMy_Rank);
    err=err||pack->send_pack(MPIMASTER, MPIMSG_GETS_ONCONSOLE);

    if (err)
        throw new cException("gets_onconsole()/send");

    // receive from Master(console)
    pack = cMpiPack::instance();

    pack->recv_pack(MPIMASTER, MPIMSG_GETS_ONCONSOLE);

    err=0;

    err=err||pack->unpack_data((void**)&buff, MPI_CHAR);

    if (buff)
      strcpy(buffer,buff);

    err=err||pack->unpack_data((void*)&a, MPI_BYTE);

    if (err)
      throw new cException("gets_onconsole()/recv");

    return a!=0;
}

//-------------------------------------------------------------------------
// askyesno_onconsole:
//    Generates a user input on console, and waits for the results.

bool cMpiMod::askyesno_onconsole(const char *question)
{
	printf("Rank %d cMpiMod::askyesno_onconsole()\n", mMy_Rank);
  int err=0;

  err=err||pack->pack_data(&mMy_Rank, MPI_INT);
  err=err||pack->pack_data((char*)question, MPI_CHAR, opp_strlen(question));
printf("File %s, Line %d from Rank %d \n", __FILE__, __LINE__, mMy_Rank);
  err=err||pack->send_pack(MPIMASTER, MPIMSG_ASKYESNO_ONCONSOLE);

  if (err)
    throw new cException("askyesno_onconsole()/send");

  MPI_Status status;
  err=0;
  status=pack->recv_pack(MPIMASTER, MPIMSG_ASKYESNO_ONCONSOLE);
  err|=status.MPI_ERROR;

  char a;
  err|=pack->unpack_data((void*)&a, MPI_CHAR);

  if (err)
    throw new cException("askyesno_onconsole()/recv");
  return a!=0;
}


//-------------------------------------------------------------------------
// clear: clears all the gates in th the MPI module
//-------------------------------------------------------------------------
void cMpiMod::clear()
{
	printf("Rank %d cMpiMod::clear()\n", mMy_Rank);
  gatev.clear();
  mOutgates.clear();
}

//-------------------------------------------------------------------------
// stopallsegments:
//   send STOPSIM to all the segments
//-------------------------------------------------------------------------
void cMpiMod::stop_all_segments()
{
  int err=0;

  printf("Rank %d stop_all_segments()\n", mMy_Rank);
  if (mMy_Rank != 0) request_stopsimulation();
  else {
  ev.printf("Stopping simulation execution on all segments...\n");
  err=err||pack->pack_data(&mMy_Rank, MPI_INT);
printf("File %s, Line %d from Rank %d \n", __FILE__, __LINE__, mMy_Rank);
  err=err||multicast(MPIMSG_STOPSIM);

  if (err)
    ev.printfmsg("MPI error in stop_all_segments()");
}
}

//-------------------------------------------------------------------------
// request_stopsimulation:
//   If the module is on the console, sends stopsim message to all the others
//   and stops itself. If it's not, just notify the console. The console
//   itself will send a stopsim message.
// CURRENTLY NOT USED: cSimulation::error() calls stop_all_segments() directly.
//-------------------------------------------------------------------------
void cMpiMod::request_stopsimulation()
{
	printf("Rank %d reques_stopsimulation()\n", mMy_Rank);
printf("File %s, Line %d from Rank %d \n", __FILE__, __LINE__, mMy_Rank);
  if (ev.runningMode()==MASTER_MODE)
  {
    stop_all_segments();
  }
  else
  {
    int err=0;

    err=err||pack->pack_data(&mMy_Rank, MPI_INT);
    err=err||pack->send_pack(MPIMASTER, MPIMSG_REQUEST_STOPSIM);
    if (err)
      throw new cException("request_stopsimulation()");
  }
}

//==========================================================================
// process_netmsgs(): processes messages currently in the MPI reveice buffers
//==========================================================================
void cMpiMod::process_netmsgs()
{
	printf("Rank %d cMpiMod::process_netmsgs()\n", mMy_Rank);
printf("Rank %d, File %s, Line %d \n", mMy_Rank, __FILE__, __LINE__);
    int flag;
    MPI_Status status;
    status = pack->nrecv_pack(MPI_ANY_SOURCE, MPI_ANY_TAG, flag);
    while (flag)
    {
      do_process_netmsg(status.MPI_TAG);
      // check for the next receive buffer
      status = pack->nrecv_pack(MPI_ANY_SOURCE, MPI_ANY_TAG, flag);
    }
}

//==========================================================================
// process_netmsgs(): waits for msgs to arrive and processes them
//==========================================================================
void cMpiMod::process_netmsg_blocking()
{
	printf("Rank %d cMpiMod::process_netmsg_blocking()\n", mMy_Rank);
printf("Rank %d, File %s, Line %d \n", mMy_Rank, __FILE__, __LINE__);
    MPI_Status status;
    status = pack->recv_pack(MPI_ANY_SOURCE, MPI_ANY_TAG);
    if (!status.MPI_ERROR)
        do_process_netmsg(status.MPI_TAG);
}

//==========================================================================
// do_process_netmsgs(): actual work for the previous two funcs
//==========================================================================
void cMpiMod::do_process_netmsg(int tag)
{
	printf("Rank %d, do_process_netmsg()\n", mMy_Rank);

    int err=0;
    int gate_num;
    double t;
    int incomingRank;;
    char *str, *mod;
    char *promptstr;
    char *buff;
    int srctid;
    int length;
    char res;
    cNetGate *netg;
    cMessage *msg;
    char hostname[5]; // integer plus null terminator

    switch (tag)
    {
        // cMessage for a simple module
        case MPIMSG_SIMULATION_CMSG:
			printf("simulation message\n");
            err=0;
            err|=pack->unpack_data((void*)&gate_num, MPI_INT);
            msg = (cMessage *)upack_object(err);
            if (err)
              throw new cException("do_process_netmsg()/unpacking the message");


            //FIXME            
            // del_syncpoint(msg->arrivalTime(),gate_num);

            netg = (cNetGate *)mOutgates[gate_num];
            if (netg->toGate()==NULL)
              throw new cException(eNODEL);

            if (msg->arrivalTime() < simulation.simTime())
              throw new cException("Arrival time of cMessage from another segment already passed");

            netg->deliver(msg);
            break;

        // syncpoint request
        case MPIMSG_SYNCPOINT:
            err=0;
            err|=pack->unpack_data((void*)&t, MPI_DOUBLE);
            err|=pack->unpack_data((void*)&gate_num, MPI_INT);
            add_syncpoint(t,gate_num);
            break;

        // cancelsyncpoint request
        case MPIMSG_CANCELSYNCPOINT:
            err=0;
            err|=pack->unpack_data((void*)&t, MPI_DOUBLE);
            err|=pack->unpack_data((void*)&gate_num, MPI_INT);
            cancel_syncpoint(t,gate_num);
            break;

        // A message to display on the console host.
        case MPIMSG_PUTMSG_ONCONSOLE:
            err=0;
            err|=pack->unpack_data((void*)&incomingRank, MPI_INT);
            err|=pack->unpack_data((void**)&str, MPI_CHAR);
            if (!err)
                ev.printfmsg("Process ID %d: %s", incomingRank, str);
            else
                throw new cException("MPI: Bad console message");
            break;

        // Module or info message to display on the console host.
        case MPIMSG_PUTS_ONCONSOLE:
            err=0;
            err|=pack->unpack_data((void*)&incomingRank, MPI_INT);
            err|=pack->unpack_data((void**)&str, MPI_CHAR);
            err|=pack->unpack_data((void**)&mod, MPI_CHAR);
            // convert process ID into string
            sprintf(hostname, "%d", incomingRank);
            if (!err)
                ev.foreignPuts(hostname, mod, str);
            else
                throw new cException("MPI: Bad console message");
            break;

        // A message which prompts for user input on the console
        case MPIMSG_GETS_ONCONSOLE:
            err=0;
            err|=pack->unpack_data((void*)&incomingRank, MPI_INT);
            err|=pack->unpack_data((void**)&promptstr, MPI_CHAR);
            err|=pack->unpack_data((void**)&buff, MPI_CHAR);
            err|=pack->unpack_data((void*)&length, MPI_INT);

            if (err)
              throw new cException("do_process_netmsg()/remote gets() req");

            res=ev.gets(promptstr,buff,length); // hostname?

            err=0;
			err=err||pack->pack_data(buff, MPI_CHAR, opp_strlen(buff));
			err=err||pack->pack_data(&res, MPI_CHAR);
            err=err||pack->send_pack(incomingRank, MPIMSG_GETS_ONCONSOLE);

            if (err)
              throw new cException("do_process_netmsg()/answering remote gets()");
            break;

        case MPIMSG_ASKYESNO_ONCONSOLE:
            err=0;
            err|=pack->unpack_data((void*)&incomingRank, MPI_INT);
            err|=pack->unpack_data((void**)&promptstr, MPI_CHAR);

            if (err)
              throw new cException(ePVM, "do_process_netmsg()/remote askyesno() request");

            res=ev.askYesNo(promptstr);

            err=0;

            err=err||pack->pack_data(&res, MPI_CHAR);
            err=err||pack->send_pack(incomingRank, MPIMSG_ASKYESNO_ONCONSOLE);

            if (err)
              throw new cException("do_process_netmsg()/answering remote askyesno()");
            break;

        // A message to halt the simulation.
        case MPIMSG_STOPSIM:
            err|=pack->unpack_data((void*)&incomingRank, MPI_INT);
            // convert process ID into string
            sprintf(hostname, "%d", incomingRank);
            throw new cTerminationException(eSTOPSIMRCVD, hostname);
            // CAUTION: The error code eSTOPSIMRCVD must be kept!
            //  (it is checked at other places in sim)
            break;

        // A message or query to stop the simulation.
        case MPIMSG_REQUEST_STOPSIM:
            if (ev.runningMode()==MASTER_MODE)
            {
              err|=pack->unpack_data((void*)&incomingRank, MPI_INT);
              ev.printfmsg("Stopping simulation by request of segment on process ID `%d'", incomingRank);
              stop_all_segments();
            }
            break;
        default:
            throw new cException("do_process_netmsg(): bad MPI message tag %i",tag); // --LG
    }
}

//==========================================================================
// send_syncpoint()
//  called by cSimpleModule::syncpoint()
//==========================================================================

void cMpiMod::send_syncpoint( simtime_t t, int ongate)
{
	printf("Rank %d cMpiMod::send_syncpoint()\n", mMy_Rank);
  int mpi_dest = ((cNetGate *)gatev[ongate])->t_proc();
  int gate_num = ((cNetGate *)gatev[ongate])->t_gate();

  int err = 0;

  err|=pack->pack_data(&t, MPI_DOUBLE);
  err|=pack->pack_data(&gate_num, MPI_INT);
  err|=pack->send_pack(mpi_dest, MPIMSG_SYNCPOINT);

  if (err)
    throw new cException("send_syncpoint()");
}

//==========================================================================
// send_cancelsyncpoint()
//  called by cSimpleModule::cancelSyncpoint()
//==========================================================================

void cMpiMod::send_cancelsyncpoint( simtime_t t, int ongate)
{
	printf("Rank %d cMpiMod::send_cancelsyncpoint()\n", mMy_Rank);
  int mpi_dest = ((cNetGate *)gatev[ongate])->t_proc();
  int gate_num = ((cNetGate *)gatev[ongate])->t_gate();

  int err = 0;
  err|=pack->pack_data(&t, MPI_DOUBLE);
  err|=pack->pack_data(&gate_num, MPI_INT);
  err|=pack->send_pack(mpi_dest, MPIMSG_CANCELSYNCPOINT);

  if (err)
    throw new cException("send_cancelsyncpoint()");
}

//==========================================================================
// block_on_syncpoint()
//  blocks on next syncpoint if necessary
//  returns true if actually blocked
//          false if nothing happened
//==========================================================================

bool cMpiMod::block_on_syncpoint( simtime_t nextlocalevent)
{
	printf("Rank %d cMpiMod::block_on_syncpoint()\n", mMy_Rank);
    MPI_Status status;

    if (nextlocalevent>next_syncpoint())
    {
		printf("blocking on sycpoint!!!!!!\n");
       // wait until syncpoint gets deleted by a cMessage from another segment
       while (nextlocalevent>next_syncpoint())
       {
         status = pack->recv_pack(MPI_ANY_SOURCE, MPI_ANY_TAG);

         if (status.MPI_ERROR)
           throw new cException("block_on_syncpoint()");

         do_process_netmsg(status.MPI_TAG);
       }
       return true;
    }
    return false;
}

//==========================================================================
// add_syncpoint()
//  add a syncpoint to internal list
//==========================================================================
void cMpiMod::add_syncpoint( simtime_t t, int ongate)
{
	printf("Rank %d cMpiMod::add_syncpoint()\n", mMy_Rank);
  sSyncPoint **pp = &syncpoints;
  while (*pp && t>(*pp)->t)
    pp = &((*pp)->next);
  sSyncPoint *rest = *pp;
  *pp = new sSyncPoint;
  (*pp)->t = t;
  (*pp)->gate = ongate;
  (*pp)->next = rest;
}

//==========================================================================
// del_syncpoint()
//  delete a syncpoint from internal list when a msg has arrived
//==========================================================================
void cMpiMod::del_syncpoint(simtime_t msgtime, int gate)
{
	printf("Rank %d cMpiMod::del_syncpoint()\n", mMy_Rank);
  sSyncPoint **pp = &syncpoints;
  while (*pp && (*pp)->gate!=gate)
    pp = &((*pp)->next);

  if (!*pp)
  {
/* --> ** TODO
// Handle all syncpoints afterwards
//    opp_warning("cMessage from another segment received with no syncpoint");
*/
    return;
  }
  if ((*pp)->t > msgtime)
    opp_warning("cMessage from another segment arrived before first syncpoint");

  sSyncPoint *rest = (*pp)->next;

  delete *pp;
  *pp = rest;
}

//==========================================================================
// cancel_syncpoint()
//  delete syncpoint from internal list because of a cancelSyncpoint() call
//==========================================================================
void cMpiMod::cancel_syncpoint(simtime_t t, int gate)
{
	printf("Rank %d cMpiMod::cancel_syncpoint()\n", mMy_Rank);
  sSyncPoint **pp = &syncpoints;
  while (*pp && ((*pp)->gate!=gate || (*pp)->t!=t))
    pp = &((*pp)->next);

  if (!*pp)
  {
    opp_warning("Received cancelSyncpoint(): no such syncpoint or already passed");
    return;
  }

  sSyncPoint *rest = (*pp)->next;

  delete *pp;
  *pp = rest;
}

//==========================================================================
// next_syncpoint()
//  return time of next syncpoint
//==========================================================================
simtime_t cMpiMod::next_syncpoint()
{
	printf("Rank %d, cMpiMod::next_syncpoint()\n", mMy_Rank);
  return syncpoints ? syncpoints->t : MAXTIME;
}

//==========================================================================
// synchronize()
//   wrapper around MPI_Barrier(). Synchronizes the pvm modules.
//   All modules have to call this function to cross the barrier.
//==========================================================================
void cMpiMod::synchronize()
{
	printf("Rank %d cMpiMod::synchronize()\n", mMy_Rank);
  if (MPI_Barrier(MPI_COMM_WORLD)!=MPI_SUCCESS)
    throw new cException("synchronize()");
}

//==========================================================================
// The next 4 functions are necessary to be defined because of the separate
// input and output gate lists.
//==========================================================================

//--------------------------------------------------------------------------
// findingate : search for serial num of input gate by its name

int cMpiMod::findingate(const char *s)
{
	printf("Rank %d cMpiMod::findingate()\n", mMy_Rank);
  return gatev.find(s);
}

//--------------------------------------------------------------------------
// ingate: reference to input gate with name s.

cGate* cMpiMod::ingate(const char *s)
{
	printf("Rank %d cMpiMod::ingate()\n", mMy_Rank);
  int i=gatev.find(s);
  if (i==-1)
    {opp_warning(eNULLREF,className(),fullPath()); return NULL;}
  else
    return (cNetGate *)gatev.get(i);
}

//--------------------------------------------------------------------------
// findoutgate : search for serial num of output gate by its name

int cMpiMod::findoutgate(const char *s)
{
	printf("Rank %d cMpiMod::findoutgate()\n", mMy_Rank);
  return mOutgates.find(s);
}

//--------------------------------------------------------------------------
// outgate: reference to output gate with name s.

cGate* cMpiMod::outgate(const char *s)
{
	printf("Rank %d cMpiMod::outgate()\n", mMy_Rank);
  int i=mOutgates.find(s);
  if (i==-1)
    {opp_warning(eNULLREF,className(),fullPath()); return NULL;}
  else
    return (cNetGate *)mOutgates[i];
}

//==========================================================================

int cMpiMod::receive_runnumber()
{
	printf("Rank %d cMpiMod::receive_runnumber()\n", mMy_Rank);
  ev.printf("Waiting for run number...\n");

  pack->recv_pack(MPIMASTER, MPIMSG_RUNNUMBER);

  int run_nr = 0;
  int err=pack->unpack_data((void*)&run_nr, MPI_INT);
printf("FILE: %s, LINE: %d \n", __FILE__, __LINE__);
  printf("Rank %d, Run number: %d \n", mMy_Rank, run_nr);

  if (err)
    throw new cException("receive_runnumber()");

  ev.printf("Got run number: %d\n",run_nr);
  return run_nr;
}

void cMpiMod::send_runnumber(int run_nr)
{
	printf("Rank %d cMpiMod::send_runnumber()\n", mMy_Rank);
  ev.printf("Sending run number %d to other segments...\n",run_nr);

  int err=pack->pack_data(&run_nr, MPI_INT);
printf("File %s, Line %d from Rank %d \n", __FILE__, __LINE__, mMy_Rank);
  multicast(MPIMSG_RUNNUMBER);

  if (err)
    throw new cException("send_runnumber()");
}

const char* cMpiMod::localhost()
{
	printf("Rank %d cMpiMod::localhost()\n", mMy_Rank);
	printf("...Local host is %s\n", (const char*)my_host);
  return my_host;
}

int cMpiMod::rank ()
{
	return mMy_Rank;
}
