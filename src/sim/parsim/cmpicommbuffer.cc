//=========================================================================
//
// CMPICOMMBUFFER.CC - part of
//                          OMNeT++
//           Discrete System Simulation in C++
//
//   Written by:  Andras Varga & David Wu, 2003
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2004 Andras Varga & David Wu
  Monash University, Dept. of Electrical and Computer Systems Eng.
  Melbourne, Australia

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <string.h>
#include <mpi.h>
#include "cmpicommbuffer.h"
#include "macros.h"

Register_Class(cMPICommBuffer);


cMPICommBuffer::cMPICommBuffer()
{
}

cMPICommBuffer::~cMPICommBuffer()
{
}

void cMPICommBuffer::extendBufferFor(int count, MPI_Datatype dataType)
{
    int dataSize = count * sizeof(double);  // FIXME use more precise size estimation

    // FIXME move reallocate+copy out of loop
    while (mMsgSize+dataSize >= mBufferSize)
    {
        // increase the size of the buffer while
        // retaining its own existing contents
        char *tempBuffer;
        int i, oldBufferSize = 0;

        oldBufferSize = mBufferSize;
        if (mBufferSize == 0)
            mBufferSize = 1000;
        else
            mBufferSize += mBufferSize;

        tempBuffer = new char[mBufferSize];
        for(i = 0; i < oldBufferSize; i++)
            tempBuffer[i] = mBuffer[i];

        delete [] mBuffer;
        mBuffer = tempBuffer;
    }
}

void cMPICommBuffer::pack(char d)
{
    extendBufferFor(1, MPI_CHAR);
    if (MPI_Pack(&d, 1, MPI_CHAR, mBuffer, mBufferSize, &mMsgSize, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::pack(char): MPI_Pack() returned error");
}


void cMPICommBuffer::pack(unsigned char d)
{
    extendBufferFor(1, MPI_UNSIGNED_CHAR);
    if (MPI_Pack(&d, 1, MPI_UNSIGNED_CHAR, mBuffer, mBufferSize, &mMsgSize, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::pack(unsigned char): MPI_Pack() returned error");
}


void cMPICommBuffer::pack(bool d)
{
    extendBufferFor(1, MPI_CHAR);
    if (MPI_Pack(&d, 1, MPI_CHAR, mBuffer, mBufferSize, &mMsgSize, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::pack(bool): MPI_Pack() returned error");
}

void cMPICommBuffer::pack(short d)
{
    extendBufferFor(1, MPI_SHORT);
    if (MPI_Pack(&d, 1, MPI_SHORT, mBuffer, mBufferSize, &mMsgSize, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::pack(short): MPI_Pack() returned error");
}


void cMPICommBuffer::pack(unsigned short d)
{
    extendBufferFor(1, MPI_UNSIGNED_SHORT);
    if (MPI_Pack(&d, 1, MPI_UNSIGNED_SHORT, mBuffer, mBufferSize, &mMsgSize, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::pack(unsigned short): MPI_Pack() returned error");
}


void cMPICommBuffer::pack(int d)
{
    extendBufferFor(1, MPI_INT);
    if (MPI_Pack(&d, 1, MPI_INT, mBuffer, mBufferSize, &mMsgSize, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::pack(int): MPI_Pack() returned error");
}


void cMPICommBuffer::pack(unsigned int d)
{
    extendBufferFor(1, MPI_UNSIGNED);
    if (MPI_Pack(&d, 1, MPI_UNSIGNED, mBuffer, mBufferSize, &mMsgSize, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::pack(unsigned int): MPI_Pack() returned error");
}


void cMPICommBuffer::pack(long d)
{
    extendBufferFor(1, MPI_LONG);
    if (MPI_Pack(&d, 1, MPI_LONG, mBuffer, mBufferSize, &mMsgSize, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::pack(long): MPI_Pack() returned error");
}


void cMPICommBuffer::pack(unsigned long d)
{
    extendBufferFor(1, MPI_UNSIGNED_LONG);
    if (MPI_Pack(&d, 1, MPI_UNSIGNED_LONG, mBuffer, mBufferSize, &mMsgSize, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::pack(unsigned long): MPI_Pack() returned error");
}


void cMPICommBuffer::pack(float d)
{
    extendBufferFor(1, MPI_FLOAT);
    if (MPI_Pack(&d, 1, MPI_FLOAT, mBuffer, mBufferSize, &mMsgSize, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::pack(float): MPI_Pack() returned error");
}


void cMPICommBuffer::pack(double d)
{
    extendBufferFor(1, MPI_DOUBLE);
    if (MPI_Pack(&d, 1, MPI_DOUBLE, mBuffer, mBufferSize, &mMsgSize, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::pack(double): MPI_Pack() returned error");
}


void cMPICommBuffer::pack(long double d)
{
    extendBufferFor(1, MPI_LONG_DOUBLE);
    if (MPI_Pack(&d, 1, MPI_LONG_DOUBLE, mBuffer, mBufferSize, &mMsgSize, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::pack(long double): MPI_Pack() returned error");
}


// pack a string
void cMPICommBuffer::pack(const char *d)
{
    int len = d ? strlen(d) : 0;
    pack(len);

    extendBufferFor(len, MPI_CHAR);
    if (MPI_Pack((void *)d, len, MPI_CHAR, mBuffer, mBufferSize, &mMsgSize, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::pack(const char*): MPI_Pack() returned error");
}

void cMPICommBuffer::pack(opp_string& d)
{
    pack(d.buffer());
}


void cMPICommBuffer::pack(const char *d, int size)
{
    extendBufferFor(size, MPI_CHAR);
    if (MPI_Pack((void *)d, size, MPI_CHAR, mBuffer, mBufferSize, &mMsgSize, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::pack(const char*): MPI_Pack() returned error");
}


void cMPICommBuffer::pack(unsigned char *d, int size)
{
    extendBufferFor(size, MPI_UNSIGNED_CHAR);
    if (MPI_Pack(d, size, MPI_UNSIGNED_CHAR, mBuffer, mBufferSize, &mMsgSize, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::pack(unsigned char*): MPI_Pack() returned error");
}


void cMPICommBuffer::pack(bool *d, int size)
{
    extendBufferFor(size, MPI_CHAR); //FIXME there's no MPI_BOOL -- is MPI_CHAR ok here?
    if (MPI_Pack(d, size, MPI_CHAR, mBuffer, mBufferSize, &mMsgSize, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::pack(char*): MPI_Pack() returned error");
}


void cMPICommBuffer::pack(short *d, int size)
{
    extendBufferFor(size, MPI_SHORT);
    if (MPI_Pack(d, size, MPI_CHAR, mBuffer, mBufferSize, &mMsgSize, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::pack(char*): MPI_Pack() returned error");
}


void cMPICommBuffer::pack(unsigned short *d, int size)
{
    extendBufferFor(size, MPI_SHORT);
    if (MPI_Pack(d, size, MPI_CHAR, mBuffer, mBufferSize, &mMsgSize, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::pack(char*): MPI_Pack() returned error");
}


void cMPICommBuffer::pack(int *d, int size)
{
    extendBufferFor(size, MPI_INT);
    if (MPI_Pack(d, size, MPI_INT, mBuffer, mBufferSize, &mMsgSize, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::pack(int*): MPI_Pack() returned error");
}


void cMPICommBuffer::pack(unsigned int *d, int size)
{
    extendBufferFor(size, MPI_UNSIGNED);
    if (MPI_Pack(d, size, MPI_UNSIGNED, mBuffer, mBufferSize, &mMsgSize, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::pack(unsigned*): MPI_Pack() returned error");
}


void cMPICommBuffer::pack(long *d, int size)
{
    extendBufferFor(size, MPI_LONG);
    if (MPI_Pack(d, size, MPI_LONG, mBuffer, mBufferSize, &mMsgSize, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::pack(long*): MPI_Pack() returned error");
}


void cMPICommBuffer::pack(unsigned long *d, int size)
{
    extendBufferFor(size, MPI_UNSIGNED_LONG);
    if (MPI_Pack(d, size, MPI_UNSIGNED_LONG, mBuffer, mBufferSize, &mMsgSize, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::pack(unsigned long*): MPI_Pack() returned error");
}


void cMPICommBuffer::pack(float *d, int size)
{
    extendBufferFor(size, MPI_FLOAT);
    if (MPI_Pack(d, size, MPI_FLOAT, mBuffer, mBufferSize, &mMsgSize, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::pack(float*): MPI_Pack() returned error");
}


void cMPICommBuffer::pack(double *d, int size)
{
    extendBufferFor(size, MPI_DOUBLE);
    if (MPI_Pack(d, size, MPI_DOUBLE, mBuffer, mBufferSize, &mMsgSize, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::pack(double*): MPI_Pack() returned error");
}


void cMPICommBuffer::pack(long double *d, int size)
{
    extendBufferFor(size, MPI_LONG_DOUBLE);
    if (MPI_Pack(d, size, MPI_LONG_DOUBLE, mBuffer, mBufferSize, &mMsgSize, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::pack(long double*): MPI_Pack() returned error");
}


// pack string array
void cMPICommBuffer::pack(const char **d, int size)
{
    for (int i=0; i<size; i++)
        pack(d[i]);
}

void cMPICommBuffer::pack(opp_string *d, int size)
{
    for (int i = 0; i < size; i++)
        pack(d[i]);
}

//--------------------------------

void cMPICommBuffer::unpack(char& d)
{
    if (MPI_Unpack(mBuffer, mMsgSize, &mPosition, &d, 1, MPI_CHAR, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::unpack(char): MPI_Unpack() returned error");
}


void cMPICommBuffer::unpack(unsigned char& d)
{
    if (MPI_Unpack(mBuffer, mMsgSize, &mPosition, &d, 1, MPI_UNSIGNED_CHAR, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::unpack(unsigned char): MPI_Unpack() returned error");
}

void cMPICommBuffer::unpack(bool& d)
{
    if (MPI_Unpack(mBuffer, mMsgSize, &mPosition, &d, 1, MPI_CHAR, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::unpack(short): MPI_Unpack() returned error");
}

void cMPICommBuffer::unpack(short& d)
{
    if (MPI_Unpack(mBuffer, mMsgSize, &mPosition, &d, 1, MPI_SHORT, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::unpack(short): MPI_Unpack() returned error");
}


void cMPICommBuffer::unpack(unsigned short& d)
{
    if (MPI_Unpack(mBuffer, mMsgSize, &mPosition, &d, 1, MPI_UNSIGNED_SHORT, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::unpack(unsigned short): MPI_Unpack() returned error");
}


void cMPICommBuffer::unpack(int& d)
{
    if (MPI_Unpack(mBuffer, mMsgSize, &mPosition, &d, 1, MPI_INT, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::unpack(int): MPI_Unpack() returned error");
}


void cMPICommBuffer::unpack(unsigned int& d)
{
    if (MPI_Unpack(mBuffer, mMsgSize, &mPosition, &d, 1, MPI_UNSIGNED, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::unpack(unsigned int): MPI_Unpack() returned error");
}


void cMPICommBuffer::unpack(long& d)
{
    if (MPI_Unpack(mBuffer, mMsgSize, &mPosition, &d, 1, MPI_LONG, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::unpack(long): MPI_Unpack() returned error");
}


void cMPICommBuffer::unpack(unsigned long& d)
{
    if (MPI_Unpack(mBuffer, mMsgSize, &mPosition, &d, 1, MPI_UNSIGNED_LONG, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::unpack(unsigned long): MPI_Unpack() returned error");
}


void cMPICommBuffer::unpack(float& d)
{
    if (MPI_Unpack(mBuffer, mMsgSize, &mPosition, &d, 1, MPI_FLOAT, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::unpack(float): MPI_Unpack() returned error");
}


void cMPICommBuffer::unpack(double& d)
{
    if (MPI_Unpack(mBuffer, mMsgSize, &mPosition, &d, 1, MPI_DOUBLE, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::unpack(double): MPI_Unpack() returned error");
}


void cMPICommBuffer::unpack(long double& d)
{
    if (MPI_Unpack(mBuffer, mMsgSize, &mPosition, &d, 1, MPI_LONG_DOUBLE, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::unpack(long double): MPI_Unpack() returned error");
}


// unpack a string
void cMPICommBuffer::unpack(const char *&d)
{
    int len;
    unpack(len);
    char *tmp = new char[len+1];
    if (MPI_Unpack(mBuffer, mMsgSize, &mPosition, tmp, len, MPI_CHAR, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::unpack(char*): MPI_Unpack() returned error");
    tmp[len] = '\0';
    d = tmp;
}

void cMPICommBuffer::unpack(opp_string& d)
{
    int len;
    unpack(len);
    d.allocate(len+1);
    if (MPI_Unpack(mBuffer, mMsgSize, &mPosition, d.buffer(), len, MPI_CHAR, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::unpack(char*): MPI_Unpack() returned error");
    d.buffer()[len] = '\0';
}

void cMPICommBuffer::unpack(char *d, int size)
{
    if (MPI_Unpack(mBuffer, mMsgSize, &mPosition, d, size, MPI_CHAR, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::unpack(char*): MPI_Unpack() returned error");
}


void cMPICommBuffer::unpack(unsigned char *d, int size)
{
    if (MPI_Unpack(mBuffer, mMsgSize, &mPosition, d, size, MPI_UNSIGNED_CHAR, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::unpack(char*): MPI_Unpack() returned error");
}


void cMPICommBuffer::unpack(bool *d, int size)
{
    if (MPI_Unpack(mBuffer, mMsgSize, &mPosition, d, size, MPI_CHAR, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::unpack(bool*): MPI_Unpack() returned error");
}

void cMPICommBuffer::unpack(short *d, int size)
{
    if (MPI_Unpack(mBuffer, mMsgSize, &mPosition, d, size, MPI_SHORT, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::unpack(short*): MPI_Unpack() returned error");
}


void cMPICommBuffer::unpack(unsigned short *d, int size)
{
    if (MPI_Unpack(mBuffer, mMsgSize, &mPosition, d, size, MPI_UNSIGNED_SHORT, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::unpack(unsigned short*): MPI_Unpack() returned error");
}


void cMPICommBuffer::unpack(int *d, int size)
{
    if (MPI_Unpack(mBuffer, mMsgSize, &mPosition, d, size, MPI_INT, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::unpack(int*): MPI_Unpack() returned error");
}


void cMPICommBuffer::unpack(unsigned int *d, int size)
{
    if (MPI_Unpack(mBuffer, mMsgSize, &mPosition, d, size, MPI_UNSIGNED, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::unpack(unsigned int*): MPI_Unpack() returned error");
}


void cMPICommBuffer::unpack(long *d, int size)
{
    if (MPI_Unpack(mBuffer, mMsgSize, &mPosition, d, size, MPI_LONG, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::unpack(long*): MPI_Unpack() returned error");
}


void cMPICommBuffer::unpack(unsigned long *d, int size)
{
    if (MPI_Unpack(mBuffer, mMsgSize, &mPosition, d, size, MPI_UNSIGNED_LONG, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::unpack(unsigned long*): MPI_Unpack() returned error");
}


void cMPICommBuffer::unpack(float *d, int size)
{
    if (MPI_Unpack(mBuffer, mMsgSize, &mPosition, d, size, MPI_FLOAT, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::unpack(float*): MPI_Unpack() returned error");
}


void cMPICommBuffer::unpack(double *d, int size)
{
    if (MPI_Unpack(mBuffer, mMsgSize, &mPosition, d, size, MPI_DOUBLE, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::unpack(double*): MPI_Unpack() returned error");
}


void cMPICommBuffer::unpack(long double *d, int size)
{
    if (MPI_Unpack(mBuffer, mMsgSize, &mPosition, d, size, MPI_LONG_DOUBLE, MPI_COMM_WORLD))
        throw new cException("cMPICommBuffer::unpack(long double*): MPI_Unpack() returned error");
}

void cMPICommBuffer::unpack(const char **d, int size)
{
    for (int i = 0; i < size; i++)
        unpack(d[i]);
}

void cMPICommBuffer::unpack(opp_string *d, int size)
{
    for (int i = 0; i < size; i++)
        unpack(d[i]);
}

