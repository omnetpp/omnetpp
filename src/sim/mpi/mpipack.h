//=========================================================================
//
// MPIPACK.H - part of
//                          OMNeT++
//           Discrete System Simulation in C++
//
//   Written by:  Eric Wu, 2001
//
//   Contents:
//      Encapsulate MPI_Pack() and MPI_Unpack() and futher extend their
//      functionalities
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2001 Eric Wu
  Monash University, Dept. of Electrical and Computer Systems Eng.
  Melbourne, Australia

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __MPI_DEMO_H__
#define __MPI_DEMO_H__

#include "mpidefs.h"

class cMpiPack 
{
 private:
  static cMpiPack* mInstance;
  char* mBuffer;
  int mBufferSize;
  int mPosition;
  int mMsgSize; // used for receiving end

 private:
  void addBufferSize(int datalength);

 public:
  static cMpiPack* instance();
  cMpiPack();
  ~cMpiPack();

  // Sender functions:
  // --------------------------------------------------------------
  // Send the pack
  // ---------------------
  // [IN] destination
  // [IN] message tag
  // [IN] communication group
  // [IN] delete flag (for broacasting purpose)
  int send_pack(int destination, int tag, bool delFlag = true, MPI_Comm comm = MPI_COMM_WORLD);

  // Pack the element
  // ---------------------
  // [IN] data element
  // [IN] data type
  // [IN] data length
  // [IN] communication group
  int pack_data(void* data, MPI_Datatype, int datalength = 1, MPI_Comm comm = MPI_COMM_WORLD);

  // Receiver functions
  // --------------------------------------------------------------
  // Receive the pack (blocking)
  // ---------------------
  // [IN] source
  // [IN] message tag
  // [IN] communication group
  MPI_Status recv_pack(int source, int tag, MPI_Comm comm = MPI_COMM_WORLD);

  // Receive the pack (non- blocking) Emulate pvm_nrecv where we know nothing 
  // has been received at this time and return null.
  // ---------------------
  // [IN] source
  // [IN] message tag
  // [OUT] receive flag indicate if message is received
  // [IN] communication group
  MPI_Status nrecv_pack(int source, int tag, int& recvflag, MPI_Comm comm = MPI_COMM_WORLD);

  // UnPack the element
  // ---------------------
  // [OUT] data element
  // [IN] data type
  // [IN] communication group
  int unpack_data(void** data, MPI_Datatype, MPI_Comm comm = MPI_COMM_WORLD); // for string 
  int unpack_data(void* data, MPI_Datatype, MPI_Comm comm = MPI_COMM_WORLD);  

  // Remove the buffer explicitely
  void remove_buffer(void);
};

#endif // __MPI_DEMO_H__
