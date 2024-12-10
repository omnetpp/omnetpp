//=========================================================================
//  CMEMCOMMBUFFER.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstring>
#include "omnetpp/simutil.h"
#include "omnetpp/cownedobject.h"
#include "omnetpp/globals.h"
#include "omnetpp/regmacros.h"
#include "omnetpp/opp_string.h"
#include "cmemcommbuffer.h"

namespace omnetpp {

Register_Class(cMemCommBuffer);

#define STORE(type, d)                 { *(type *)(mBuffer+mMsgSize) = d; mMsgSize += sizeof(type); }
#define EXTRACT(type, d)               { d = *(type *)(mBuffer+mPosition); mPosition += sizeof(type); }

#define STOREARRAY(type, d, size)      { if (size > 0) { memcpy(mBuffer+mMsgSize, d, size*sizeof(type)); mMsgSize += size*sizeof(type); } }
#define EXTRACTARRAY(type, d, size)    { if (size > 0) { memcpy(d, mBuffer+mPosition, size*sizeof(type)); mPosition += size*sizeof(type); } }


cMemCommBuffer& cMemCommBuffer::operator=(cMemCommBuffer&& other) noexcept
{
    cCommBufferBase::operator=(std::move(other));
    return *this;
}

void cMemCommBuffer::pack(char d)
{
    checkThreadAccess();
    extendBufferFor(sizeof(char));
    STORE(char, d);
}

void cMemCommBuffer::pack(unsigned char d)
{
    checkThreadAccess();
    extendBufferFor(sizeof(unsigned char));
    STORE(unsigned char, d);
}

void cMemCommBuffer::pack(bool d)
{
    checkThreadAccess();
    extendBufferFor(sizeof(bool));
    STORE(bool, d);
}

void cMemCommBuffer::pack(short d)
{
    checkThreadAccess();
    extendBufferFor(sizeof(short));
    STORE(short, d);
}

void cMemCommBuffer::pack(unsigned short d)
{
    checkThreadAccess();
    extendBufferFor(sizeof(unsigned short));
    STORE(unsigned short, d);
}

void cMemCommBuffer::pack(int d)
{
    checkThreadAccess();
    extendBufferFor(sizeof(int));
    STORE(int, d);
}

void cMemCommBuffer::pack(unsigned int d)
{
    checkThreadAccess();
    extendBufferFor(sizeof(unsigned int));
    STORE(unsigned int, d);
}

void cMemCommBuffer::pack(long d)
{
    checkThreadAccess();
    extendBufferFor(sizeof(long));
    STORE(long, d);
}

void cMemCommBuffer::pack(unsigned long d)
{
    checkThreadAccess();
    extendBufferFor(sizeof(unsigned long));
    STORE(unsigned long, d);
}

void cMemCommBuffer::pack(long long d)
{
    checkThreadAccess();
    extendBufferFor(sizeof(long long));
    STORE(long long, d);
}

void cMemCommBuffer::pack(unsigned long long d)
{
    checkThreadAccess();
    extendBufferFor(sizeof(unsigned long long));
    STORE(unsigned long long, d);
}

void cMemCommBuffer::pack(float d)
{
    checkThreadAccess();
    extendBufferFor(sizeof(float));
    STORE(float, d);
}

void cMemCommBuffer::pack(double d)
{
    checkThreadAccess();
    extendBufferFor(sizeof(double));
    STORE(double, d);
}

void cMemCommBuffer::pack(long double d)
{
    checkThreadAccess();
    extendBufferFor(sizeof(long double));
    STORE(long double, d);
}

// pack a string
void cMemCommBuffer::pack(const char *d)
{
    checkThreadAccess();
    int len = d ? strlen(d) : 0;
    pack(len);
    extendBufferFor(len*sizeof(char));
    STOREARRAY(char, d, len);
}

void cMemCommBuffer::pack(const opp_string& d)
{
    checkThreadAccess();
    pack(d.c_str());
}

void cMemCommBuffer::pack(SimTime d)
{
    checkThreadAccess();
    pack((long long)d.raw());
}

void cMemCommBuffer::pack(const char *d, int size)
{
    checkThreadAccess();
    extendBufferFor(size*sizeof(char));
    STOREARRAY(char, d, size);
}

void cMemCommBuffer::pack(const unsigned char *d, int size)
{
    checkThreadAccess();
    extendBufferFor(size*sizeof(unsigned char));
    STOREARRAY(unsigned char, d, size);
}

void cMemCommBuffer::pack(const bool *d, int size)
{
    checkThreadAccess();
    extendBufferFor(size*sizeof(bool));
    STOREARRAY(bool, d, size);
}

void cMemCommBuffer::pack(const short *d, int size)
{
    checkThreadAccess();
    extendBufferFor(size*sizeof(short));
    STOREARRAY(short, d, size);
}

void cMemCommBuffer::pack(const unsigned short *d, int size)
{
    checkThreadAccess();
    extendBufferFor(size*sizeof(unsigned short));
    STOREARRAY(unsigned short, d, size);
}

void cMemCommBuffer::pack(const int *d, int size)
{
    checkThreadAccess();
    extendBufferFor(size*sizeof(int));
    STOREARRAY(int, d, size);
}

void cMemCommBuffer::pack(const unsigned int *d, int size)
{
    checkThreadAccess();
    extendBufferFor(size*sizeof(unsigned int));
    STOREARRAY(unsigned int, d, size);
}

void cMemCommBuffer::pack(const long *d, int size)
{
    checkThreadAccess();
    extendBufferFor(size*sizeof(long));
    STOREARRAY(long, d, size);
}

void cMemCommBuffer::pack(const unsigned long *d, int size)
{
    checkThreadAccess();
    extendBufferFor(size*sizeof(unsigned long));
    STOREARRAY(unsigned long, d, size);
}

void cMemCommBuffer::pack(const long long *d, int size)
{
    checkThreadAccess();
    extendBufferFor(size*sizeof(long long));
    STOREARRAY(long long, d, size);
}

void cMemCommBuffer::pack(const unsigned long long *d, int size)
{
    checkThreadAccess();
    extendBufferFor(size*sizeof(unsigned long long));
    STOREARRAY(unsigned long long, d, size);
}

void cMemCommBuffer::pack(const float *d, int size)
{
    checkThreadAccess();
    extendBufferFor(size*sizeof(float));
    STOREARRAY(float, d, size);
}

void cMemCommBuffer::pack(const double *d, int size)
{
    checkThreadAccess();
    extendBufferFor(size*sizeof(double));
    STOREARRAY(double, d, size);
}

void cMemCommBuffer::pack(const long double *d, int size)
{
    checkThreadAccess();
    extendBufferFor(size*sizeof(long double));
    STOREARRAY(long double, d, size);
}

// pack string array
void cMemCommBuffer::pack(const char **d, int size)
{
    checkThreadAccess();
    for (int i = 0; i < size; i++)
        pack(d[i]);
}

void cMemCommBuffer::pack(const opp_string *d, int size)
{
    checkThreadAccess();
    for (int i = 0; i < size; i++)
        pack(d[i]);
}

void cMemCommBuffer::pack(const SimTime *d, int size)
{
    checkThreadAccess();
    for (int i = 0; i < size; i++)
        pack(d[i]);
}

// --------------------------------

void cMemCommBuffer::unpack(char& d)
{
    checkThreadAccess();
    EXTRACT(char, d);
}

void cMemCommBuffer::unpack(unsigned char& d)
{
    checkThreadAccess();
    EXTRACT(unsigned char, d);
}

void cMemCommBuffer::unpack(bool& d)
{
    checkThreadAccess();
    EXTRACT(bool, d);
}

void cMemCommBuffer::unpack(short& d)
{
    checkThreadAccess();
    EXTRACT(short, d);
}

void cMemCommBuffer::unpack(unsigned short& d)
{
    checkThreadAccess();
    EXTRACT(unsigned short, d);
}

void cMemCommBuffer::unpack(int& d)
{
    checkThreadAccess();
    EXTRACT(int, d);
}

void cMemCommBuffer::unpack(unsigned int& d)
{
    checkThreadAccess();
    EXTRACT(unsigned int, d);
}

void cMemCommBuffer::unpack(long& d)
{
    checkThreadAccess();
    EXTRACT(long, d);
}

void cMemCommBuffer::unpack(unsigned long& d)
{
    checkThreadAccess();
    EXTRACT(unsigned long, d);
}

void cMemCommBuffer::unpack(long long& d)
{
    checkThreadAccess();
    EXTRACT(long long, d);
}

void cMemCommBuffer::unpack(unsigned long long& d)
{
    checkThreadAccess();
    EXTRACT(unsigned long long, d);
}

void cMemCommBuffer::unpack(float& d)
{
    checkThreadAccess();
    EXTRACT(float, d);
}

void cMemCommBuffer::unpack(double& d)
{
    checkThreadAccess();
    EXTRACT(double, d);
}

void cMemCommBuffer::unpack(long double& d)
{
    checkThreadAccess();
    EXTRACT(long double, d);
}

// unpack a string
void cMemCommBuffer::unpack(const char *& d)
{
    checkThreadAccess();
    int len;
    unpack(len);
    char *tmp = new char[len+1];
    EXTRACTARRAY(char, tmp, len);
    tmp[len] = '\0';
    d = tmp;
}

void cMemCommBuffer::unpack(opp_string& d)
{
    checkThreadAccess();
    int len;
    unpack(len);
    d.reserve(len+1);
    EXTRACTARRAY(char, d.buffer(), len);
    d.buffer()[len] = '\0';
}

void cMemCommBuffer::unpack(SimTime& d)
{
    checkThreadAccess();
    long long raw;
    unpack(raw);
    d.setRaw(raw);
}

void cMemCommBuffer::unpack(char *d, int size)
{
    checkThreadAccess();
    EXTRACTARRAY(char, d, size);
}

void cMemCommBuffer::unpack(unsigned char *d, int size)
{
    checkThreadAccess();
    EXTRACTARRAY(unsigned char, d, size);
}

void cMemCommBuffer::unpack(bool *d, int size)
{
    checkThreadAccess();
    EXTRACTARRAY(bool, d, size);
}

void cMemCommBuffer::unpack(short *d, int size)
{
    checkThreadAccess();
    EXTRACTARRAY(short, d, size);
}

void cMemCommBuffer::unpack(unsigned short *d, int size)
{
    checkThreadAccess();
    EXTRACTARRAY(unsigned short, d, size);
}

void cMemCommBuffer::unpack(int *d, int size)
{
    checkThreadAccess();
    EXTRACTARRAY(int, d, size);
}

void cMemCommBuffer::unpack(unsigned int *d, int size)
{
    checkThreadAccess();
    EXTRACTARRAY(unsigned, d, size);
}

void cMemCommBuffer::unpack(long *d, int size)
{
    checkThreadAccess();
    EXTRACTARRAY(long, d, size);
}

void cMemCommBuffer::unpack(unsigned long *d, int size)
{
    checkThreadAccess();
    EXTRACTARRAY(unsigned long, d, size);
}

void cMemCommBuffer::unpack(long long *d, int size)
{
    checkThreadAccess();
    EXTRACTARRAY(long long, d, size);
}

void cMemCommBuffer::unpack(unsigned long long *d, int size)
{
    checkThreadAccess();
    EXTRACTARRAY(unsigned long long, d, size);
}

void cMemCommBuffer::unpack(float *d, int size)
{
    checkThreadAccess();
    EXTRACTARRAY(float, d, size);
}

void cMemCommBuffer::unpack(double *d, int size)
{
    checkThreadAccess();
    EXTRACTARRAY(double, d, size);
}

void cMemCommBuffer::unpack(long double *d, int size)
{
    checkThreadAccess();
    EXTRACTARRAY(long double, d, size);
}

void cMemCommBuffer::unpack(const char **d, int size)
{
    checkThreadAccess();
    for (int i = 0; i < size; i++)
        unpack(d[i]);
}

void cMemCommBuffer::unpack(opp_string *d, int size)
{
    checkThreadAccess();
    for (int i = 0; i < size; i++)
        unpack(d[i]);
}

void cMemCommBuffer::unpack(SimTime *d, int size)
{
    checkThreadAccess();
    for (int i = 0; i < size; i++)
        unpack(d[i]);
}

}  // namespace omnetpp

