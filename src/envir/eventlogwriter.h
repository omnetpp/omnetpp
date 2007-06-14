//=========================================================================
// EVENTLOGWRITER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  This is a generated file -- do not modify.
//
//=========================================================================

#ifndef __EVENTLOGWRITER_H_
#define __EVENTLOGWRITER_H_

#include <stdio.h>
#include "envdefs.h"
#include "cexception.h"

class EventLogWriter
{
  public:
    static void recordLogLine(FILE *f, const char *s, int n);
    static void recordBubbleEntry_id_txt(FILE *f, int moduleId, const char * text);
    static void recordModuleMethodBeginEntry_sm_tm_m(FILE *f, int fromModuleId, int toModuleId, const char * method);
    static void recordModuleMethodEndEntry(FILE *f);
    static void recordModuleCreatedEntry_id_c_n(FILE *f, int moduleId, const char * moduleClassName, const char * fullName);
    static void recordModuleCreatedEntry_id_c_pid_n(FILE *f, int moduleId, const char * moduleClassName, int parentModuleId, const char * fullName);
    static void recordModuleDeletedEntry_id(FILE *f, int moduleId);
    static void recordModuleReparentedEntry_id_p(FILE *f, int moduleId, int newParentModuleId);
    static void recordConnectionCreatedEntry_sm_sg_sn_dm_dg_dn(FILE *f, int sourceModuleId, int sourceGateId, const char * sourceGateFullName, int destModuleId, int destGateId, const char * destGateFullName);
    static void recordConnectionDeletedEntry_sm_sg(FILE *f, int sourceModuleId, int sourceGateId);
    static void recordConnectionDisplayStringChangedEntry_sm_sg_d(FILE *f, int sourceModuleId, int sourceGateId, const char * displayString);
    static void recordModuleDisplayStringChangedEntry_id_d(FILE *f, int moduleId, const char * displayString);
    static void recordEventEntry_e_t_m_msg(FILE *f, long eventNumber, simtime_t simulationTime, int moduleId, long messageId);
    static void recordEventEntry_e_t_m_ce_msg(FILE *f, long eventNumber, simtime_t simulationTime, int moduleId, long causeEventNumber, long messageId);
    static void recordCancelEventEntry_id(FILE *f, long messageId);
    static void recordCancelEventEntry_id_pe(FILE *f, long messageId, long previousEventNumber);
    static void recordBeginSendEntry_id_tid_c_n(FILE *f, long messageId, long messageTreeId, const char * messageClassName, const char * messageFullName);
    static void recordBeginSendEntry_id_tid_eid_etid_c_n_pe_k_l_p_er_d(FILE *f, long messageId, long messageTreeId, long messageEncapsulationId, long messageEncapsulationTreeId, const char * messageClassName, const char * messageFullName, long previousEventNumber, int messageKind, long messageLength, int messagePriority, bool hasBitError, const char * detail);
    static void recordEndSendEntry_t(FILE *f, simtime_t arrivalTime);
    static void recordSendDirectEntry_sm_dm_dg(FILE *f, int senderModuleId, int destModuleId, int destGateId);
    static void recordSendDirectEntry_sm_dm_dg_pd_td(FILE *f, int senderModuleId, int destModuleId, int destGateId, simtime_t propagationDelay, simtime_t transmissionDelay);
    static void recordSendHopEntry_sm_sg(FILE *f, int senderModuleId, int senderGateId);
    static void recordSendHopEntry_sm_sg_pd_td(FILE *f, int senderModuleId, int senderGateId, simtime_t propagationDelay, simtime_t transmissionDelay);
    static void recordDeleteMessageEntry_id(FILE *f, int messageId);
    static void recordDeleteMessageEntry_id_pe(FILE *f, int messageId, long previousEventNumber);
};

#endif
