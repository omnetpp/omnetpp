//==========================================================================
//  FDDI_MAC.H -   Definitions & Declarations for
//                 Discrete System Simulation in OMNeT++
//
//  Simulated system: TUB Northern&Southern FDDI Rings
//
//  Author: Gabor.Lencse@hit.bme.hu
//==========================================================================

//--------------------------------------------------------------*
//  Copyright (C) 1996,97 Gabor Lencse,
//  Technical University of Budapest, Dept. of Telecommunications,
//  Stoczek u.2, H-1111 Budapest, Hungary.
//
//  This file is distributed WITHOUT ANY WARRANTY. See the file
//  `license' for details on this and other legal matters.
//--------------------------------------------------------------*/

#ifndef __FDDI_MAC_H
#define __FDDI_MAC_H

#include <omnetpp.h>

class CountDownCounter // implements counter that counts downwards
  {
  private:
    simtime_t StartTime;
    double InitValue;
    bool enabled;
  public:
    void operator= (double InitVal)
      {
      StartTime = simulation.simTime();
      InitValue = InitVal;
      enabled = true;
      }
    operator double()
      {
      return InitValue - (simulation.simTime() - StartTime);
      }
    void disable()
      {
      enabled = false;
      }
    void enable()
      {
      StartTime = simulation.simTime();
      enabled = true;
      }
  };

class FDDI_MAC : public cSimpleModule // Media Access Control sublayer for FDDI
  {
  protected:
  CountDownCounter TRT; // Token Rotation Time
  CountDownCounter THT; // Token HOLDING Time
  MACState State;       // The current State of the MAC
  unsigned CurrentEvent;// the event under processing

  cMessage *msg_being_recd; // message is currently being received
  cMessage *msg_to_repeat;  // this msg will be repeated when REPEAT_BEGIN event arrives
  simtime_t RepeatEndsAt; // the current repeating will end at this time

  bool EarlyToken;         // Token is early
  bool RestrictedToken;    // Token is restricted
  int LateCounter;         // TRT expired LateCounter times since last TOKEN_ARRIVE
  cMessage *TRTExpire;     // holds a pointer to the TRT expire event
  long AllocatedSyncBandwidth; // Allocated Synchronous Bandwidth (in bytes)
  long UsedSyncBandwidth;  // Used Synchronous Bandwidth (in bytes)
  double T_Opr;            // operational value for the TTRT (Target Token Rotation Time)
  short RingID;            // This FDDI_MAC is connected to ring RingID
  long IdleTimes;          // # of tokens arrived since packet was transmitted for the last time
  int my_station_id;       // the stations are numbered, number 0 issues the token
  bool IAmRestrictedOwner; // This station is a/the ResrictedToken owner
  bool SyncFinished;       // transm. of sync. packets is already finished

  cQueue sync_buf;         // the queue of the synchronous packets
  cQueue async_buf;        // the queue of the asynchronous packets
  int priority_i;          // holds the cycle variable for the priority classes

// external events:
  void TokenArrived(cMessage *msg);
  virtual void OwnFrameArrived(cMessage *msg);
  void Frame2MeArrived(cMessage *msg);
  virtual void Frame2RepArrived(cMessage *msg);

// internal events:
  void TokenRepeatBegin(cMessage *msg);
  void TokenRepeatEnd(cMessage *msg);
  void TokenReceiveEnd(cMessage *msg);
  void TokenCaptureEnd(cMessage *msg);
  void TransmitOwnBegin(cMessage *msg);
  void TransmissionEnd(cMessage *msg);
  void TokenSendEnd(cMessage *msg);
  void FrameStripEnd(cMessage *msg);
  void FrameToMeReceiveEnd(cMessage *msg);
  void RepeatEnd(cMessage *msg);
  void Wait4RepeatEnd(cMessage *msg);
  void TRTExpired(cMessage *msg);
  void RepeatBegin(cMessage *msg);

// other member functions:
  void PlayTTRP(); // Play the Timed Token Ring Protocol
  void StateCheck(MACState required_state, MACState also_good_state);
  virtual void activity();

  public:
  FDDI_MAC(const char *namestr, cModule *parentmod);
  virtual const char *className()  {return "FDDI_MAC";}
  };

class FDDI_MAC4Ring : public FDDI_MAC
  {
  public:
  FDDI_MAC4Ring(const char *namestr, cModule *parentmod);
  virtual const char *className()  {return "FDDI_MAC4Ring";}
  };

class FDDI_MAC4Sniffer : public FDDI_MAC
  {
  virtual void OwnFrameArrived(cMessage *);
  virtual void Frame2RepArrived(cMessage *);

  public:
  FDDI_MAC4Sniffer(const char *namestr, cModule *parentmod):
    FDDI_MAC(namestr, parentmod) { }
  virtual const char *className()  {return "FDDI_MAC4Sniffer";}
  };

#endif

