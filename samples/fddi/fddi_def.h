//==========================================================================
//  FDDI_DEF.H  -  Definitions for the FDDI MAC model for
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

#ifndef __FDDI_DEF_H
#define __FDDI_DEF_H

#define FDDI_MAC_HEAPSIZE 65535

// definitions for the FDDI model

// constants
#define byte_tr_time 8e-8 /* 8*(1/100 000 000)= 8e-8 */
//#define delay_bef_repeat 16*byte_tr_time /* 8+1+1+6=16 */
#define TokenLength 11  /* 8+1+1+1=11 */
#define token_time TokenLength*byte_tr_time
#define station_latency 100e-9 /* 100ns, taken from the OPNET FDDI modell */
#define idle_bytes 8 /* 16 idle symbols */
#define inorder_epsilon 1e-10 /* to assure that receive or repeat ends before the next packet arrives */

// types

typedef unsigned int MACState;

// The message types (possible values of msgkind):
#define FDDI_TOKEN 0
#define FDDI_FRAME 1

#define FDDI_MAC_MSG2SELF 100  /* this is a limit */
/* if msgkind >= FDDI_MAC_MSG2SELF then it is an FDDI_MAC event */
// The events are:
#define TOKEN_ARRIVED     100 /* The edge of a token arrived to the station */
#define TOKEN_RECEIVE_END 101 /* The receiving of the token was finished */
#define TRANSMISSION_END  102 /* The transmission of an own frame ended */
#define TOKEN_SEND_END    103 /* The transmission of the token was finished */
#define OWN_FRAME_ARRIVED 104 /* The edge of a frame from this station arrived back */
#define FRAME_STRIP_END   105 /* Finished stripping the own frame from the ring */
#define FR2ME_ARRIVED     106 /* The edge of a frame addressed to this station arrived */
#define FR2ME_RECEIVE_END 107 /* The frame addressed to this station completely received */
#define FR2REP_ARRIVED    108 /* The edge of a frame arrived, that should be repeated */
#define REPEAT_END        109 /* The repeating of a frame ended */
// #define WAIT4REPEAT_END        /* Wait for REPEAT ended, delayed event can be processed */
#define TRT_EXPIRED       110 /* The TRT ContDownCounter expired */
#define REPEAT_BEGIN      111 /* The repeating of a frame may start now */
#define TOKEN_REPEAT_BEGIN 112 /* The repeating of the token starts now */
#define TOKEN_REPEAT_END 113 /* The repeating of the token ends now */
#define TOKEN_CAPTURE_END 114 /* The capturing of the token ends now */
#define TRANSMIT_OWN_BEGIN 115 /* An own frame will be transmitted from now */

// The states
// Remark:
// The system can be in more than one state at a time
// each state is a bit in the state variable
// if State&GIVEN_STATE then system is in GIVEN_STATE
// of course, the IDLE is not a real "state" of this manner
#define IDLE            0x0 /* The system is in no other states */
#define TOKEN_RECEIVE   0x1 /* A token is being received - but not captured, it will be repeated */
#define REPEAT          0x2 /* A frame is being repeated */
// #define WAIT4REPEAT     0x4 /* The processing of an event is delayed until the end of repeating */
#define TRANSMIT_OWN    0x8 /* An own frame is being transmitted */
#define TOKEN_SEND     0x10 /* A token is being sent */
#define FRAME_STRIP    0x20 /* A frame is being stripped out from the ring */
#define FR2ME_RECEIVE  0x40 /* A frame addressed to this station is being received */
// to model station_latency, before the repeating of a frame may start, the
// system is in the following state until the arrival of a REPEAT_BEGIN event
#define BEFORE_REPEAT  0x80 /* Waiting for a REPEAT_BEGIN event to start repeating a frame */
// A properly working FDDI MAC may never enter to the following state:
#define TOKEN_LOST    0x100 /* The token was lost (TRT_EXPIRED when LateCounter==1) */
#define TOKEN_CAPTURE 0x200 /* The token is being captured (stripped from the ring) for frame transmission */
#define BEFORE_TOKEN_REPEAT 0x400 /* If a token is not captured, MAC waits station_latency time befor repeating it */
#define TOKEN_REPEAT 0x800 /* The token is being repeated (retransmitted to the ring) */
#define BEFORE_TRANSMIT_OWN 0x1000/* before the first own frame is transmitted, MAC waits station_latency time */

#endif

