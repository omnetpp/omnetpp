//=========================================================================
//
// MPIPACK.CC - part of
//                          OMNeT++
//           Discrete System Simulation in C++
//
//   Written by:  Eric Wu, 2001
//
//   Contents:
//      The definition of the function which stores an object
//      in the MPI sender buffer or restores them from the receiver buffer.
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2001 Eric Wu
  Monash University, Dept. of Electrical and Computer Systems Eng.
  Melbourne, Australia

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <string.h>
#include <stdio.h>

#include "mpipack.h"
#include "omnetpp.h"
#include "mpimod.h"

//=========================================================================
// cMpiPack definition
//=========================================================================

cMpiPack* cMpiPack::mInstance = 0;

cMpiPack* cMpiPack::instance()
{
  if(mInstance == 0)
  {
    mInstance = new cMpiPack;
  }
  return mInstance;
}

cMpiPack::cMpiPack()
{
  mBuffer = 0;
  mBufferSize = 0;
  mPosition = 0;
  mMsgSize = 0;
}

cMpiPack::~cMpiPack()
{
  if(mBuffer!=0)
    delete[] mBuffer;

  if(mInstance!=0)
  {
    mInstance = 0;
  }
}

void cMpiPack::addBufferSize(int datalength)
{
  int tempSize = mBufferSize;
  mBufferSize += datalength;

  if(mBuffer==0)
  {
    mBuffer = new char[mBufferSize];
    //    mBuffer = (char*)malloc(mBufferSize);
  }
  else
  {
    // increase the size of the buffer while
    // retaining its own existing contents

    char* tempBuffer = new char[mBufferSize];
    for(int i=0; i<tempSize; i++)
      tempBuffer[i] = mBuffer[i];

    delete[] mBuffer;

    mBuffer = new char[mBufferSize];
    for(int i=0; i<tempSize; i++)
      mBuffer[i] = tempBuffer[i];
    delete[] tempBuffer;
  }
}

int cMpiPack::pack_data(void* data, MPI_Datatype datatype, int datalength, MPI_Comm comm)
{
  int status;

  // space needed to send a single MPI element
  int sendBuffSize = 0;

  // only if the data is of string type, the string length is required
  if(datatype == MPI_CHAR || datatype == MPI_BYTE)
  {
    datalength = strlen((char*)data)+1;

    char* tempData = (char*)malloc(datalength);
    tempData[0] = strlen((char*)data); // store the string length first

    strcpy(tempData+1, (char*)data);

    MPI_Pack_size(datalength, datatype, comm, &sendBuffSize);
    addBufferSize(sendBuffSize);
    status = MPI_Pack(tempData, datalength, datatype, mBuffer, mBufferSize, &mPosition, comm);

    free(tempData);
  }
  else
  {
    MPI_Pack_size(datalength, datatype, comm, &sendBuffSize);
    addBufferSize(sendBuffSize);
    status = MPI_Pack(data, datalength, datatype, mBuffer, mBufferSize, &mPosition, comm);
  }

  return status;
}

int cMpiPack::send_pack(int destination, int tag, bool delFlag, MPI_Comm comm)
{
  //**** test code ********
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  printf("*** SEND || My Rank %d | Message tag %d | TO Destination %d  \n", rank, tag, destination);
  //**** ********************

  int status;

  status = MPI_Send(mBuffer, mPosition, MPI_PACKED, destination, tag, comm);

  if(delFlag!=true)
    return status;

  delete[] mBuffer;
  mBuffer = 0;
  mBufferSize = 0;
  mPosition = 0;
  mMsgSize = 0;

  return status;
}

// blocking receive
MPI_Status cMpiPack::recv_pack(int source, int tag, MPI_Comm comm)
{
  MPI_Status status;

  if(mBuffer!=0)
  {
    delete[] mBuffer;
    mPosition = 0;
  }

  mBuffer = new char[MAXSIZE];
  MPI_Recv(mBuffer, MAXSIZE, MPI_PACKED, source, tag, comm, &status);

  if(status.MPI_ERROR)
  {
    printf("MPI_ERROR = %d \n", status.MPI_ERROR);
    return status;
  }

//**** test code ********
int rank;
MPI_Comm_rank(MPI_COMM_WORLD, &rank);
printf("*** BLOCKING RECV || In Rank %d | From Source %d | Message tag %d \n", rank, status.MPI_SOURCE, status.MPI_TAG);
//**** ********************

  MPI_Get_count(&status, MPI_PACKED, &mMsgSize);
  return status;
}

// non-blocking receive
MPI_Status cMpiPack::nrecv_pack(int source, int tag, int& recvflag, MPI_Comm comm)
{
  MPI_Status status;
  MPI_Request request;
  mBuffer = new char[MAXSIZE];
  MPI_Irecv(mBuffer, MAXSIZE, MPI_PACKED, source, tag, comm, &request);

  for(int j=0; j<WAIT_TIME; j++)
  {
    MPI_Test(&request, &recvflag, &status);
    if(recvflag==1)
      break;
  }
  int size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  if(recvflag==0 || (status.MPI_SOURCE < MPIMASTER) || (status.MPI_SOURCE >= size))
  {
    MPI_Cancel(&request);
    if(mBuffer!=0) delete[] mBuffer;
    mBuffer=0;
    recvflag=0;
    return status;
  }

printf("File %s, Line %d \n", __FILE__, __LINE__);
//**** test code ********
int rank;
MPI_Comm_rank(MPI_COMM_WORLD, &rank);
printf("**** NON-BLOCKING RECV In Rank %d || From Source %d | Message tag %d \n", rank, status.MPI_SOURCE, status.MPI_TAG);
//**** ********************

  MPI_Get_count(&status, MPI_PACKED, &mMsgSize);

  return status;
}

int cMpiPack::unpack_data(void** data, MPI_Datatype datatype, MPI_Comm comm)
{
  int datalength;

  int status;

  if(datatype == MPI_CHAR || datatype == MPI_BYTE)
  {
    datalength = (int)mBuffer[mPosition]; // string length is the first slot of the element
    mPosition++; // move the position pointing to the string
  }

  // type casting
  switch(datatype)
  {
    case MPI_CHAR:
      {
	*data= new char[datalength+1]; // extra space for null terminator
	char* data_addr = (char*)*data;
	data_addr[datalength] = '\0';
      }
      break;
    case MPI_UNSIGNED_CHAR:
      {
	*data= new unsigned char[datalength+1]; // extra space for null terminator
	char* data_addr = (char*)*data;
	data_addr[datalength] = '\0';
      }
      break;
    case MPI_SHORT:
      *data= new short int[datalength];
      break;
    case MPI_UNSIGNED_SHORT:
      *data= new unsigned short int[datalength];
      break;
    case MPI_INT:
      *data= new int[datalength];
      break;
    case MPI_UNSIGNED:
      *data= new unsigned int[datalength];
      break;
    case MPI_LONG:
      *data= new long int[datalength];
      break;
    case MPI_FLOAT:
      *data= new float[datalength];
      break;
    case MPI_DOUBLE:
      *data= new double[datalength];
      break;
    case MPI_LONG_DOUBLE:
      *data= new long double[datalength];
      break;
    case MPI_BYTE:
      *data= new char[datalength];
      break;
    case MPI_UNSIGNED_LONG:
      *data= new unsigned long int[datalength];
      break;
  }
  status = MPI_Unpack(mBuffer, mMsgSize, &mPosition, *data, datalength, datatype, comm);

  if(mPosition == mMsgSize)
  {
    if(mBuffer!=0)
      delete[] mBuffer;

    mBuffer = 0;
    mBufferSize = 0;
    mPosition = 0;
    mMsgSize = 0;
  }

  return status;
}

int cMpiPack::unpack_data(void* data, MPI_Datatype datatype, MPI_Comm comm)
{
  int status;

  if(datatype == MPI_BYTE || datatype == MPI_CHAR)
  {
    mPosition++;
  }

  status = MPI_Unpack(mBuffer, mMsgSize, &mPosition, data, 1, datatype, comm);

  if(mPosition == mMsgSize)
  {
    if(mBuffer!=0)
      delete[] mBuffer;

    mBuffer = 0;
    mBufferSize = 0;
    mPosition = 0;
    mMsgSize = 0;
  }

  return status;
}

void cMpiPack::remove_buffer(void)
{
  if(mBuffer!=0)
  {
    delete[] mBuffer;
    mBuffer=0;

    mBufferSize = 0;
    mPosition = 0;
    mMsgSize = 0;
  }
}

//=========================================================================
// Helper functions to pack or unpack an object, referenced by its pointer
//=========================================================================

int notnull(void *ptr, int& err)
{
  cMpiPack* pack = cMpiPack::instance();

  char flag=0;

  if (ptr)
    flag=1;
  err=err||pack->pack_data(&flag, MPI_BYTE);
  return flag;
}

int chkflag(int& err)
{
  cMpiPack* pack = cMpiPack::instance();

  char flag;

  err=err||pack->unpack_data((void*)&flag, MPI_BYTE);
  return flag;
}

//=========================================================================
// Function that unpacks an object
//=========================================================================
cObject *upack_object(int& err)
{
  cMpiPack* pack = cMpiPack::instance();
  char* clname;
  err=pack->unpack_data((void**)&clname, MPI_CHAR);
  if (err) {delete clname; return NO(cObject);}
  cObject *obj = createOne(clname);
  obj->netUnpack();
  delete clname;
  return obj;
}

//=========================================================================
// Overriding functions for MPI module
//=========================================================================

int cObject::netPack()
{
  int err=0;
  cMpiPack* pack = cMpiPack::instance();
  err=pack->pack_data(const_cast<char*> (className()), MPI_CHAR);
  err=err||pack->pack_data(namestr, MPI_CHAR);
  return err;
}

int  cObject::netUnpack()
{
  cMpiPack* pack = cMpiPack::instance();
  int err=0;

  // upacking className() string already done by upack_object() at this point
  err=pack->unpack_data((void**)&namestr, MPI_CHAR);
  return err;
}

int cPar::netPack()
{
  cMpiPack* pack = cMpiPack::instance();

  int err=0;
  err|=cObject::netPack();

  err|=pack->pack_data(&typechar, MPI_BYTE);
  err|=pack->pack_data(&inputflag, MPI_BYTE);
  err|=pack->pack_data(&changedflag, MPI_BYTE);
  err|=pack->pack_data((void*)(promptstr.buffer()), MPI_CHAR);

  cFunctionType *ff;
  switch (typechar)
  {
    case 'S':
      err|=pack->pack_data(&(ls.sht), MPI_BYTE);
      if (notnull(ls.str,err))
	err|=pack->pack_data(ls.str, MPI_CHAR);
      break;
    case 'C':
      err|=pack->pack_data(&(ss.sht), MPI_BYTE);
      if (notnull(ss.str,err))
	err|=pack->pack_data(ss.str, MPI_CHAR);
      break;
    case 'L':
      err|=pack->pack_data(&(lng.val), MPI_LONG);
      break;
    case 'D':
      err|=pack->pack_data(&(dbl.val), MPI_DOUBLE);
      break;
    case 'F':
      ff = findfunctionbyptr(func.f);
      if (ff==NULL)
	{opp_error("cPar::netPack(): cannot transmit unregistered function");return 1;}

      err|=pack->pack_data(const_cast<char*> (ff->name()), MPI_CHAR);
      err|=pack->pack_data(&(func.argc), MPI_INT);
      err|=pack->pack_data(&(func.p1), MPI_DOUBLE);
      err|=pack->pack_data(&(func.p2), MPI_DOUBLE);
      err|=pack->pack_data(&(func.p3), MPI_DOUBLE);
      break;
    case 'T':
      if (dtr.res && dtr.res->owner()!=this)
	{opp_error("cPar::netPack(): cannot transmit pointer to \"external\" object");return 1;}
      if (notnull(dtr.res,err))
	err|=dtr.res->netPack();
      break;
    case 'I':
      opp_error("cPar::netPack(): transmitting indirect values (type 'I') not supported");
      return 1;
    case 'X':
      opp_error("cPar::netPack(): transmitting expressions (type 'X') not supported");
      return 1; // not implemented, because there are functions and pointers in it.
    case 'P':
      opp_error("cPar::netPack(): cannot transmit pointer to unknown data structure (type 'P')");
      return 1;
    case 'O':
      if (obj.obj && obj.obj->owner()!=this)
	{opp_error("cPar::netPack(): cannot transmit pointer to \"external\" object");return 1;}
      if (notnull(obj.obj,err))
	err|=obj.obj->netPack();
      break;
  }
  return err;
}

int cPar::netUnpack()
{
  char* funcname;

  cMpiPack* pack = cMpiPack::instance();

  int err=0;
  err|=cObject::netUnpack();

  err|=pack->unpack_data((void*)&typechar, MPI_BYTE);
  err|=pack->unpack_data((void*)&inputflag, MPI_BYTE);
  err|=pack->unpack_data((void*)&changedflag, MPI_BYTE);
  err|=pack->unpack_data((void**)&promptstr, MPI_CHAR);

  char *tip;
  cFunctionType *ff;
  switch (typechar)
  {
    case 'S':
      void* ls_sht;
      err|=pack->unpack_data((void*)&ls_sht, MPI_BYTE);
      err|=pack->unpack_data((void**)&(ls.str), MPI_CHAR);
      break;

    case 'C':
      void* ss_sht;
      err|=pack->unpack_data((void*)&ss_sht, MPI_BYTE);
      err|=pack->unpack_data((void**)&tip, MPI_CHAR);
      if (tip)
	strcpy(ss.str,tip);
      break;
    case 'L':
      void* lng_val;
      err|=pack->unpack_data((void*)&lng_val, MPI_LONG);
      break;
    case 'D':
      void* dbl_val;
      err|=pack->unpack_data((void*)&dbl_val, MPI_DOUBLE);
      break;
    case 'F':
      err|=pack->unpack_data((void**)&funcname, MPI_CHAR);
      ff = findFunction(funcname);
      if (ff==NULL)
      {
	  opp_error("cPar::netUnpack(): transmitted function `%s' not registered here",funcname);
	  delete funcname;
	  return 1;
      }
      func.f = ff->f;

      err|=pack->unpack_data((void*)&(func.p1), MPI_DOUBLE);
      err|=pack->unpack_data((void*)&(func.p2), MPI_DOUBLE);
      err|=pack->unpack_data((void*)&(func.p3), MPI_DOUBLE);
      delete funcname;
      break;
    case 'T':
      if (!chkflag(err))
	dtr.res=NULL;
      else
	take( dtr.res = (cStatistic *)upack_object(err) );
      break;
    case 'I':
    case 'X':
    case 'P':
      opp_error("cPar::netUnpack(): WHAT???");
      return -1;  // sending of 'X' not implemented
    case 'O':
      if (!chkflag(err))
	obj.obj=NULL;
      else
	take( obj.obj = upack_object(err) );
      break;
  }
  return err;
}

int cBag::netPack()
{
  cMpiPack* pack = cMpiPack::instance();
  int err=0;

  err|=pack->pack_data(&elemsize, MPI_INT);
  err|=pack->pack_data(&size, MPI_INT);
  err|=pack->pack_data(&delta, MPI_INT);
  err|=pack->pack_data(&lastused, MPI_INT);
  err|=pack->pack_data(&firstfree, MPI_INT);
  err|=pack->pack_data(vect, MPI_BYTE, size*(sizeof(bool)+elemsize));

  return err;
}

int cBag::netUnpack()
{
  cMpiPack* pack = cMpiPack::instance();

  int err=0;

  err|=pack->unpack_data((void*)&elemsize, MPI_INT);
  err|=pack->unpack_data((void*)&size, MPI_INT);
  err|=pack->unpack_data((void*)&delta, MPI_INT);
  err|=pack->unpack_data((void*)&lastused, MPI_INT);
  err|=pack->unpack_data((void*)&firstfree, MPI_INT);
  err|=pack->unpack_data((void**)&vect, MPI_BYTE, size*(sizeof(bool)+elemsize));

  return err;
}

int cQueue::netPack()
{
  cMpiPack* pack = cMpiPack::instance();

  int err=0;
  err|=cObject::netPack();
  err|=pack->pack_data(&n, MPI_INT);
  err|=pack->pack_data(&asc, MPI_BYTE);

  for (sQElem * pt=headp;pt!=NULL;pt=pt->next)
  {
      if (notnull(pt->obj,err))
      {
	if (pt->obj->owner()!=this)
	{
	  opp_error("cQueue::netPack(): cannot transmit pointer"
		    " to \"external\" object");return 1;
	}
	err|=pt->obj->netPack();
      }
  }
  return err;
}

int cQueue::netUnpack()
{
  cMpiPack* pack = cMpiPack::instance();

  int err=0;
  err=cObject::netUnpack();

  err|=pack->unpack_data((void*)&n, MPI_INT);
  err|=pack->unpack_data((void*)&asc, MPI_BYTE);

  if (n)
  {
    headp = new sQElem;
    sQElem * pt1=headp;
    sQElem * pt2;
    pt1->prev=NULL;
    take( pt1->obj = upack_object(err) );
    for (int i=1;i<n;i++)
    {
      pt2=pt1;
      pt1->next = new sQElem;
      pt1=pt1->next;
      pt1->prev=pt2;
      take( pt1->obj = upack_object(err) );
    }
    pt1->next=NULL;
    tailp=pt1;
  }
  return err;
}

int cArray::netPack()
{
  cMpiPack* pack = cMpiPack::instance();

  int err=0;
  err|=cObject::netPack();

  err|=pack->pack_data(&size, MPI_INT);
  err|=pack->pack_data(&delta, MPI_INT);
  err|=pack->pack_data(&firstfree, MPI_INT);
  err|=pack->pack_data(&last, MPI_INT);

  for (int i=0;i<=last;i++)
  {
    if (notnull(vect[i],err))
    {
      if (vect[i]->owner()!=this)
      {
	  opp_error("cArray::netPack(): cannot transmit pointer"
		   " to \"external\" object");return 1;
      }
      err|=vect[i]->netPack();
    }
  }
  return err;
}

int cArray::netUnpack()
{
  cMpiPack* pack = cMpiPack::instance();

  int err=0;
  err=cObject::netUnpack();
  delete vect;

  err|=pack->unpack_data((void*)&size, MPI_INT);
  err|=pack->unpack_data((void*)&delta, MPI_INT);
  err|=pack->unpack_data((void*)&firstfree, MPI_INT);
  err|=pack->unpack_data((void*)&last, MPI_INT);

  vect = new cObject *[size];
  for (int i=0;i<=last;i++)
  {
    if (!chkflag(err))
      vect[i] = NULL;
    else
      take( vect[i] = upack_object(err) );
  }
  return err;
}

int cLinkedList::netPack()
{
        opp_error("(%s)%s: netPack() not possible -- don't know how to pack an item",className(),fullName());
        return 0;
}

int cLinkedList::netUnpack()
{
        opp_error("(%s)%s: netUnpack() not possible",className(),fullName());
        return 0;
}

int cMessage::netPack()
{
  cMpiPack* pack = cMpiPack::instance();

  int err=0;
  err|=cObject::netPack();

  err|=pack->pack_data(&msgkind, MPI_INT);
  err|=pack->pack_data(&prior, MPI_INT);
  err|=pack->pack_data(&len, MPI_LONG);
  err|=pack->pack_data(&tstamp, MPI_DOUBLE);
  err|=pack->pack_data(&error, MPI_BYTE);
  err|=pack->pack_data(&frommod, MPI_INT);
  err|=pack->pack_data(&fromgate, MPI_INT);
  err|=pack->pack_data(&tomod, MPI_INT);
  err|=pack->pack_data(&togate, MPI_INT);
  err|=pack->pack_data(&sent, MPI_DOUBLE);
  err|=pack->pack_data(&delivd, MPI_DOUBLE);

  if (notnull(parlistp,err))
    parlistp->netPack();
  return err;
}

int cMessage::netUnpack()
{
  cMpiPack* pack = cMpiPack::instance();

  int err=0;
  err=cObject::netUnpack();

  err|=pack->unpack_data((void*)&msgkind, MPI_INT);
  err|=pack->unpack_data((void*)&prior, MPI_INT);
  err|=pack->unpack_data((void*)&len, MPI_LONG);
  err|=pack->unpack_data((void*)&tstamp, MPI_DOUBLE);
  err|=pack->unpack_data((void*)&error, MPI_BYTE);
  err|=pack->unpack_data((void*)&frommod, MPI_INT);
  err|=pack->unpack_data((void*)&fromgate, MPI_INT);
  err|=pack->unpack_data((void*)&tomod, MPI_INT);
  err|=pack->unpack_data((void*)&togate, MPI_INT);
  err|=pack->unpack_data((void*)&sent, MPI_DOUBLE);
  err|=pack->unpack_data((void*)&delivd, MPI_DOUBLE);

  if (chkflag(err))
    take(parlistp = (cArray *)upack_object(err));
  return err;
}

int cPacket::netPack()
{
  cMpiPack* pack = cMpiPack::instance();

  int err=0;
  err|=cMessage::netPack();

  err|=pack->pack_data(&_protocol, MPI_SHORT);
  err|=pack->pack_data(&_pdu, MPI_SHORT);
  return err;
}

int cPacket::netUnpack()
{
  cMpiPack* pack = cMpiPack::instance();

  int err=0;
  err=cMessage::netUnpack();

  err|=pack->unpack_data((void*)&_protocol, MPI_SHORT);
  err|=pack->unpack_data((void*)&_pdu, MPI_SHORT);

  return err;
}


int cTransientDetection::netPack()
{
  int err=0;
  return err||cObject::netPack();
}

int cTransientDetection::netUnpack()
{
  return cObject::netUnpack();
}

int cAccuracyDetection::netPack()
{
  int err=0;
  return err||cObject::netPack();
}

int cAccuracyDetection::netUnpack()
{
  return cObject::netUnpack();
}


int cStatistic::netPack()
{
  cMpiPack* pack = cMpiPack::instance();

  int err=0;
  err=cObject::netPack();

  err|=pack->pack_data(&genk, MPI_INT);

  if (notnull(td,err))
    err|=td->netPack();
  if (notnull(ra,err))
    err|=ra->netPack();
  return err;
}

int cStatistic::netUnpack()
{
  cMpiPack* pack = cMpiPack::instance();

  int err=0;
  err=cObject::netUnpack();

  err|=pack->unpack_data((void*)&genk, MPI_INT);

  if (chkflag(err))
    take( td = (cTransientDetection *)upack_object(err) );
  if (chkflag(err))
    take( ra = (cAccuracyDetection *)upack_object(err) );
  return err;
}


int cStdDev::netPack()
{
  cMpiPack* pack = cMpiPack::instance();

  int err=0;
  err|=cStatistic::netPack();

  err|=pack->pack_data(&num_samples, MPI_LONG);
  err|=pack->pack_data(&min_samples, MPI_DOUBLE);
  err|=pack->pack_data(&max_samples, MPI_DOUBLE);
  err|=pack->pack_data(&sum_samples, MPI_DOUBLE);
  err|=pack->pack_data(&sqrsum_samples, MPI_DOUBLE);

  return err;
}

int cStdDev::netUnpack()
{
  cMpiPack* pack = cMpiPack::instance();

  int err=0;

  err=cStatistic::netUnpack();

  err|=pack->unpack_data((void*)&num_samples, MPI_LONG);
  err|=pack->unpack_data((void*)&min_samples, MPI_DOUBLE);
  err|=pack->unpack_data((void*)&max_samples, MPI_DOUBLE);
  err|=pack->unpack_data((void*)&sum_samples, MPI_DOUBLE);
  err|=pack->unpack_data((void*)&sqrsum_samples, MPI_DOUBLE);

  return err;
}


int cWeightedStdDev::netPack()
{
  cMpiPack* pack = cMpiPack::instance();

  int err=0;
  err|=cStdDev::netPack();

  err|=pack->pack_data(&sum_weights, MPI_DOUBLE);
  return err;
}

int cWeightedStdDev::netUnpack()
{
  cMpiPack* pack = cMpiPack::instance();

  int err=0;
  err=cStdDev::netUnpack();

  err|=pack->unpack_data((void*)&sum_weights, MPI_DOUBLE);

  return err;
}

int cDensityEstBase::netPack()
{
  cMpiPack* pack = cMpiPack::instance();

  int err=0;
  err|=cStdDev::netPack();

  err|=pack->pack_data(&rangemin, MPI_DOUBLE);
  err|=pack->pack_data(&rangemax, MPI_DOUBLE);
  err|=pack->pack_data(&num_firstvals, MPI_LONG);
  err|=pack->pack_data(&cell_under, MPI_UNSIGNED_LONG);
  err|=pack->pack_data(&cell_over, MPI_UNSIGNED_LONG);
  err|=pack->pack_data(&range_ext_factor, MPI_DOUBLE);
  err|=pack->pack_data(&range_mode, MPI_INT);

  int t=transfd;
  err|=pack->pack_data(&t, MPI_INT);

  if (notnull(firstvals,err))
    err|=pack->pack_data(firstvals, MPI_DOUBLE, num_firstvals);

  return err;
}

int cDensityEstBase::netUnpack()
{
  cMpiPack* pack = cMpiPack::instance();

  int err=0;
  err=cStdDev::netUnpack();

  err|=pack->unpack_data((void*)&rangemin, MPI_DOUBLE);
  err|=pack->unpack_data((void*)&rangemax, MPI_DOUBLE);
  err|=pack->unpack_data((void*)&num_firstvals, MPI_LONG);
  err|=pack->unpack_data((void*)&cell_under, MPI_UNSIGNED_LONG);
  err|=pack->unpack_data((void*)&cell_over, MPI_UNSIGNED_LONG);
  err|=pack->unpack_data((void*)&range_ext_factor, MPI_DOUBLE);
  err|=pack->unpack_data((void*)&range_mode, MPI_INT);


  int t;
  err|=pack->unpack_data((void*)&t, MPI_INT);
  transfd=(t!=0);

  if (chkflag(err))
    err|=pack->unpack_data((void**)&firstvals, MPI_DOUBLE, num_firstvals);

  return err;
}

int cHistogramBase::netPack()
{
  cMpiPack* pack = cMpiPack::instance();

  int err=0;
  err|=cDensityEstBase::netPack();
  err|=pack->pack_data(&num_cells, MPI_INT);

  if (notnull(cellv,err))
    err|=pack->pack_data(cellv, MPI_UNSIGNED, num_cells);
  return err;
}

int cHistogramBase::netUnpack()
{
  cMpiPack* pack = cMpiPack::instance();

  int err=0;
  err=cDensityEstBase::netUnpack();
  err|=pack->pack_data(&num_cells, MPI_INT);

  if (chkflag(err))
  {
    err|=pack->unpack_data((void**)&cellv, MPI_UNSIGNED, num_cells);
  }
  return err;
}

int cEqdHistogramBase::netPack()
{
  cMpiPack* pack = cMpiPack::instance();

  int err=0;
  err|=cHistogramBase::netPack();
  err|=pack->pack_data(&cellsize, MPI_DOUBLE);
  return err;
}

int cEqdHistogramBase::netUnpack()
{
  cMpiPack* pack = cMpiPack::instance();

  int err=0;

  err=cHistogramBase::netUnpack();
  err|=pack->unpack_data((void*)&cellsize, MPI_DOUBLE);
  return err;
}

int cLongHistogram::netPack()
{
  int err=0;
  err=cEqdHistogramBase::netPack();
  return err;
}

int cLongHistogram::netUnpack()
{
  int err=0;
  err=cEqdHistogramBase::netUnpack();
  return err;
}

int cDoubleHistogram::netPack()
{
  int err=0;
  err=cEqdHistogramBase::netPack();
  return err;
}

int cDoubleHistogram::netUnpack()
{
  int err=0;
  err=cEqdHistogramBase::netUnpack();
  return err;
}

int cVarHistogram::netPack()
{
  cMpiPack* pack = cMpiPack::instance();

  int err=0;
  err|=cHistogramBase::netPack();
  err|=pack->pack_data(&max_num_cells, MPI_INT);

  if (notnull(cellv,err))
    err|=pack->pack_data(cellv, MPI_UNSIGNED, max_num_cells);

  if (notnull(bin_bounds,err))
    err|=pack->pack_data(bin_bounds, MPI_DOUBLE, max_num_cells+1);
  return err;
}

int cVarHistogram::netUnpack()
{
  cMpiPack* pack = cMpiPack::instance();

  int err=0;
  err=cHistogramBase::netUnpack();

  err|=pack->unpack_data((void*)&max_num_cells, MPI_INT);

  delete cellv; cellv=NULL; // must recreate with different size

  if (chkflag(err))
    err|=pack->unpack_data((void**)&cellv, MPI_UNSIGNED, max_num_cells);

  if (chkflag(err))
    err|=pack->unpack_data((void**)&bin_bounds, MPI_DOUBLE, max_num_cells+1);
  return err;
}


int cPSquare::netPack()
{
  cMpiPack* pack = cMpiPack::instance();

  int err=0;
  err|=cDensityEstBase::netPack();

  err|=pack->pack_data(&numcells, MPI_INT);
  err|=pack->pack_data(&numobs, MPI_LONG);

  if (notnull(n,err))
    err|=pack->pack_data(n, MPI_INT, numcells+2);
  if (notnull(q,err))
    err|=pack->pack_data(q, MPI_DOUBLE, numcells+2);
  return err;
}

int cPSquare::netUnpack()
{
  cMpiPack* pack = cMpiPack::instance();

  int err=0;
  err=cDensityEstBase::netUnpack();

  err|=pack->unpack_data((void*)&numcells, MPI_INT);
  err|=pack->unpack_data((void*)&numobs, MPI_LONG);

  if (chkflag(err))
    err|=pack->unpack_data((void**)&n, MPI_INT, numcells+2);
  if (chkflag(err))
    err|=pack->unpack_data((void**)&q, MPI_DOUBLE, numcells+2);

  return err;
}


int cKSplit::netPack()
{
  int err=0;
  err|=cDensityEstBase::netPack();

  opp_error("(%s)%s: netPack() not implemented",className(), fullName());
  return err;
}

int cKSplit::netUnpack()
{
  int err=0;
  err=cDensityEstBase::netUnpack();

  opp_error("(%s)%s: netUnpack() not implemented",className(),fullName());
  return err;
}


int cTopology::netPack()
{
  opp_error("(%s)%s: netPack() not implemented",className(), fullName());
  return 0;
}

int cTopology::netUnpack()
{
  opp_error("(%s)%s: netUnpack() not implemented",className(),fullName());
  return 0;
}

int cFSM::netPack()
{
  opp_error("(%s)%s: netPack() not implemented",className(), fullName());
  return 0;
}

int cFSM::netUnpack()
{
  opp_error("(%s)%s: netUnpack() not implemented",className(),fullName());
  return 0;
}

