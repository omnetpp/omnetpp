//==========================================================================
//   CMESSAGE.H  -  header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cMessage : message and event object
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992,99 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CMESSAGE_H
#define __CMESSAGE_H

#include <time.h>
#include "cobject.h"
#include "carray.h"
#include "cpar.h"
#include "csimul.h"

//=== classes mentioned:
class cPar;
class cGate;
class cModule;
class cSimpleModule;
class cCompoundModule;
class cSimulation;
class cMessageHeap;

//=== classes declared here:
class  cMessage;

//=== predefined packet kind values
//
// NOTE:
// - zero and positive values can be freely used
// - negative values are reserved for the OMNeT++ system and standard libs
//
enum {
  // internal
  MK_STARTER = -1,  // used by scheduleStart()
  MK_TIMEOUT = -2,  // used by wait() etc

  // cPacket
  MK_PACKET  = -3,  // network packet
  MK_INFO    = -4   // information packet
};

//==========================================================================
// cMessage: the message class

class cMessage : public cObject
{
        friend class cGate;
        friend class cModule;
        friend class cSimpleModule;
        friend class cCompoundModule;
        friend class cSimulation;
        friend class cMessageHeap;

      private:
        int msgkind;            // message kind -- meaning is user-defined
        int prior;              // priority -- used for scheduling msgs with equal times
        long len;               // length of message -- used for bit errors and transm.delay
        bool error;             // bit error occured during transmission
        simtime_t tstamp;       // time stamp -- user-defined meaning
        cArray *parlistp;       // ptr to list of parameters
        cMessage *encapmsg;     // ptr to encapsulated msg
        void *contextptr;       // a stored pointer -- user-defined meaning

        int frommod,fromgate;   // source module and gate IDs -- set internally
        int tomod,togate;       // dest. module and gate IDs -- set internally
        simtime_t created;      // creation time -- set be constructor
        simtime_t sent,delivd;  // time of sending & delivery -- set internally

        int heapindex;             // used by cMessageHeap (-1 if not on heap)
        unsigned long insertordr;  // used by cMessageHeap

        void _createparlist();

      public:
        cMessage(cMessage& msg);
        explicit cMessage(char *name=NULL, int k=0, long len=1, int pri=0, bool err=FALSE);
        virtual ~cMessage() {}     /*parlistp is deleted in ~cObject()*/

        // redefined functions
        virtual char *className()  {return "cMessage";}
        virtual cObject *dup()  {return new cMessage(*this);}
        virtual void info(char *buf);
        virtual char *inspectorFactoryName() {return "cMessageIFC";}
        virtual void forEach( ForeachFunc do_fn );
        virtual void writeContents(ostream& os);
        cMessage& operator=(cMessage& msg);

        virtual int netPack();
        virtual int netUnpack();

        // new functions
        void setKind(int k)     {msgkind=k;}    // set msg kind
        void setPriority(int p) {prior=p;}      // set priority
        void setLength(long l);                 // set message length
        void addLength(long l);                 // change message length
        void setBitError(bool err) {error=err;} // set error flag
        void setTimestamp() {tstamp=simulation.simTime();} // set time stamp to current time
        void setTimestamp(simtime_t t) {tstamp=t;}  // set time stamp to given value
        void setContextPointer(void *p) {contextptr=p;} // set context pointer

        int  kind()     {return msgkind;}       // get msg kind
        int  priority() {return prior;}         // get priority
        long length()   {return len;}           // get message length
        bool hasBitError() {return error;}      // bit error occurred or not
        simtime_t timestamp() {return tstamp;}  // get time stamp
        unsigned long insertOrder() {return insertordr;} // used by cMessageHeap
        void *contextPointer() {return contextptr;} // get context pointer

        bool isSelfMessage() {return togate==-1;}  // tell if message was posted by scheduleAt()
        bool isScheduled() {return heapindex!=-1;} // tell if message is among future events

        // parameter list
        cArray& parList()
            {if(parlistp==NULL) _createparlist(); return *parlistp;}
        cPar& addPar(char *s) {cPar *p=new cPar(s);parList().add(*p);return *p;}
        cPar& addPar(cPar *p) {parList().add(*p); return *p;}
        cPar& addPar(cPar& p) {parList().add(p); return p;} // DEPRECATED
        cPar& par(int n);                             // get parameter by index
        cPar& par(char *s);                           // get parameter by name
        int findPar(char *s);                         // get index of parameter, -1 if doesn't exist
        bool hasPar(char *s) {return findPar(s)>=0;}  // check if parameter exists

        // message encapsulation
        void encapsulate(cMessage *msg);
        cMessage *decapsulate();
        cMessage *encapsulatedMsg() {return encapmsg;}

        // sending/arrival information
        cGate *senderGate();                      // return NULL if scheduled
        cGate *arrivalGate();                     //    or unsent message

        int senderModuleId()  {return frommod;}   // source module
        int senderGateId()    {return fromgate;}  //    "   gate
        int arrivalModuleId() {return tomod;}     // destination
        int arrivalGateId()   {return togate;}    //    "   gate

        simtime_t creationTime() {return created;} // creation time
        simtime_t sendingTime()  {return sent;}    // sending time
        simtime_t arrivalTime()  {return delivd;}  // delivery time

        bool arrivedOn(int g) {return g==togate;}  // arrived on gate g?
        bool arrivedOn(char *s, int g=0);          // arrived on gate s[g]?

        static int cmpbydelivtime(cObject *one, cObject *other); // compare delivery times
        static int cmpbypriority(cObject *one, cObject *other);  // compare priorities
};

#endif

