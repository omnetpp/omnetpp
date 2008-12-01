//=========================================================================
//  CFILECOMMBUFFER.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga, 2003
//          Dept. of Electrical and Computer Systems Engineering,
//          Monash University, Melbourne, Australia
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2003-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <locale.h>

#include "cfilecommbuffer.h"
#include "simutil.h"
#include "cownedobject.h"
#include "globals.h"
#include "regmacros.h"

USING_NAMESPACE

Register_Class(cFileCommBuffer);

#define STORE(FMT,d)    {sprintf(mBuffer+mMsgSize, FMT "\n", d); mMsgSize+=strlen(mBuffer+mMsgSize);}
#define EXTRACT(FMT,d)  {sread(mBuffer, mPosition, FMT, &d);}

// TBD store/extract as ASCII not binary
#define STOREARRAY(type,d,size)   {memcpy(mBuffer+mMsgSize,d,size*sizeof(type)); mMsgSize+=size*sizeof(type);}
#define EXTRACTARRAY(type,d,size) {memcpy(d,mBuffer+mPosition,size*sizeof(type)); mPosition+=size*sizeof(type);}

#define LL  LONGLONG_PRINTF_FORMAT


// helper: match type (i.e. "i " from "i 134")
static void matchtype(char *buffer, int& pos, const char *&fmt)
{
    char *buf = buffer+pos;
    const char *origfmt = fmt;
    while (*buf && *fmt!=' ')
    {
        if (*fmt != *buf)
            throw cRuntimeError("cFileCommBuffer: unpack(): format prefix '%s' doesn't match input", origfmt);
        fmt++; buf++;
    }
    if (!*buf)
        throw cRuntimeError("cFileCommBuffer: unpack(): premature end of input (format prefix '%s' missing)", origfmt);
    fmt++; buf++;
    pos = buf-buffer;
}

static void sread(char *buffer, int& pos, const char *fmt, void *d)
{
    matchtype(buffer, pos, fmt);
    char *buf = buffer+pos;
    setlocale(LC_NUMERIC, "C");
    if (sscanf(buf,fmt,d)!=1)
        throw cRuntimeError("cFileCommBuffer: unpack(): could not convert data");
    // skip data + delimiter whitespace
    while (*buf && *buf!=' ' && *buf!='\n') buf++;
    while (*buf==' ' || *buf=='\n') buf++;
    pos = buf-buffer;
}

cFileCommBuffer::cFileCommBuffer()
{
}

cFileCommBuffer::~cFileCommBuffer()
{
}

void cFileCommBuffer::setMessageSize(int size)
{
    mMsgSize = size;
    mPosition = 0;

    // add a terminating NULL character
    ASSERT(mBufferSize>mMsgSize);
    mBuffer[size] = '\0';
}

void cFileCommBuffer::pack(char d)
{
    extendBufferFor(16);
    STORE("c %d",(int)d);
}

void cFileCommBuffer::pack(unsigned char d)
{
    extendBufferFor(16);
    STORE("uc %u",(unsigned int)d);
}

void cFileCommBuffer::pack(bool d)
{
    extendBufferFor(16);
    STORE("b %d",(int)d);
}

void cFileCommBuffer::pack(short d)
{
    extendBufferFor(16);
    STORE("s %d",(int)d);
}

void cFileCommBuffer::pack(unsigned short d)
{
    extendBufferFor(16);
    STORE("us %u",(unsigned int)d);
}

void cFileCommBuffer::pack(int d)
{
    extendBufferFor(16);
    STORE("i %d",d);
}

void cFileCommBuffer::pack(unsigned int d)
{
    extendBufferFor(16);
    STORE("ui %u",d);
}

void cFileCommBuffer::pack(long d)
{
    extendBufferFor(16);
    STORE("l %ld",d);
}

void cFileCommBuffer::pack(unsigned long d)
{
    extendBufferFor(16);
    STORE("ul %lu",d);
}

void cFileCommBuffer::pack(opp_long_long d)
{
    extendBufferFor(30);
    STORE("ll %"LL"d",d);
}

void cFileCommBuffer::pack(opp_unsigned_long_long d)
{
    extendBufferFor(30);
    STORE("ull %"LL"u",d);
}

void cFileCommBuffer::pack(float d)
{
    extendBufferFor(30);
    STORE("f %g",d);
}

void cFileCommBuffer::pack(double d)
{
    extendBufferFor(30);
    STORE("d %g",d);
}

void cFileCommBuffer::pack(long double d)
{
    // packing as double because "%Lg" does not work on MinGW, see
    // http://www.mingw.org/MinGWiki/index.php/long%20double
    extendBufferFor(30);
    STORE("ld %g",(double)d);
}

// pack a string
void cFileCommBuffer::pack(const char *d)
{
    int len = d ? strlen(d) : 0;
    extendBufferFor(strlen(d)+16);
    sprintf(mBuffer+mMsgSize, "S %d|%s\n", len,d);
    mMsgSize+=strlen(mBuffer+mMsgSize);
}

void cFileCommBuffer::pack(const opp_string& d)
{
    pack(d.c_str());
}

void cFileCommBuffer::pack(SimTime d)
{
    pack((opp_long_long)d.raw());
}

void cFileCommBuffer::pack(const char *d, int size)
{
    extendBufferFor(size*sizeof(char));
    STOREARRAY(char,d,size);
}

void cFileCommBuffer::pack(const unsigned char *d, int size)
{
    extendBufferFor(size*sizeof(unsigned char));
    STOREARRAY(unsigned char,d,size);
}

void cFileCommBuffer::pack(const bool *d, int size)
{
    extendBufferFor(size*sizeof(bool));
    STOREARRAY(bool,d,size);
}

void cFileCommBuffer::pack(const short *d, int size)
{
    extendBufferFor(size*sizeof(short));
    STOREARRAY(short,d,size);
}

void cFileCommBuffer::pack(const unsigned short *d, int size)
{
    extendBufferFor(size*sizeof(unsigned short));
    STOREARRAY(unsigned short,d,size);
}

void cFileCommBuffer::pack(const int *d, int size)
{
    extendBufferFor(size*sizeof(int));
    STOREARRAY(int,d,size);
}

void cFileCommBuffer::pack(const unsigned int *d, int size)
{
    extendBufferFor(size*sizeof(unsigned int));
    STOREARRAY(unsigned int,d,size);
}

void cFileCommBuffer::pack(const long *d, int size)
{
    extendBufferFor(size*sizeof(long));
    STOREARRAY(long,d,size);
}

void cFileCommBuffer::pack(const unsigned long *d, int size)
{
    extendBufferFor(size*sizeof(unsigned long));
    STOREARRAY(unsigned long,d,size);
}

void cFileCommBuffer::pack(const opp_long_long *d, int size)
{
    extendBufferFor(size*sizeof(opp_long_long));
    STOREARRAY(opp_long_long,d,size);
}

void cFileCommBuffer::pack(const opp_unsigned_long_long *d, int size)
{
    extendBufferFor(size*sizeof(opp_unsigned_long_long));
    STOREARRAY(opp_unsigned_long_long,d,size);
}

void cFileCommBuffer::pack(const float *d, int size)
{
    extendBufferFor(size*sizeof(float));
    STOREARRAY(float,d,size);
}

void cFileCommBuffer::pack(const double *d, int size)
{
    extendBufferFor(size*sizeof(double));
    STOREARRAY(double,d,size);
}

void cFileCommBuffer::pack(const long double *d, int size)
{
    extendBufferFor(size*sizeof(long double));
    STOREARRAY(long double,d,size);
}

// pack string array
void cFileCommBuffer::pack(const char **d, int size)
{
    for (int i = 0; i < size; i++)
        pack(d[i]);
}

void cFileCommBuffer::pack(const opp_string *d, int size)
{
    for (int i = 0; i < size; i++)
        pack(d[i]);
}

void cFileCommBuffer::pack(const SimTime *d, int size)
{
    for (int i = 0; i < size; i++)
        pack(d[i]);
}

//--------------------------------

void cFileCommBuffer::unpack(char& d)
{
    int tmp;
    EXTRACT("c %d",tmp);
    d = tmp;
}

void cFileCommBuffer::unpack(unsigned char& d)
{
    int tmp;
    EXTRACT("uc %d",tmp);
    d = tmp;
}

void cFileCommBuffer::unpack(bool& d)
{
    int tmp;
    EXTRACT("b %d",tmp);
    d = tmp;
}

void cFileCommBuffer::unpack(short& d)
{
    int tmp;
    EXTRACT("s %d",tmp);
    d = tmp;
}

void cFileCommBuffer::unpack(unsigned short& d)
{
    int tmp;
    EXTRACT("us %d",tmp);
    d = tmp;
}

void cFileCommBuffer::unpack(int& d)
{
    EXTRACT("i %d",d);
}

void cFileCommBuffer::unpack(unsigned int& d)
{
    EXTRACT("ui %d",d);
}

void cFileCommBuffer::unpack(long& d)
{
    EXTRACT("l %ld",d);
}

void cFileCommBuffer::unpack(unsigned long& d)
{
    EXTRACT("ul %ld",d);
}

void cFileCommBuffer::unpack(opp_long_long& d)
{
    EXTRACT("ll %"LL"d",d);
}

void cFileCommBuffer::unpack(opp_unsigned_long_long& d)
{
    EXTRACT("ull %"LL"u",d);
}

void cFileCommBuffer::unpack(float& d)
{
    double tmp;
    EXTRACT("f %lg",tmp);
    d = tmp;
}

void cFileCommBuffer::unpack(double& d)
{
    EXTRACT("d %lg",d);
}

void cFileCommBuffer::unpack(long double& d)
{
    // packing as double because "%Lg" does not work on MinGW, see
    // http://www.mingw.org/MinGWiki/index.php/long%20double
    double tmp;
    EXTRACT("ld %lg",tmp);
    d = tmp;
}

// unpack a string
void cFileCommBuffer::unpack(const char *&d)
{
    const char *fmt = "S ";
    matchtype(mBuffer,mPosition,fmt);
    char *buf = mBuffer+mPosition;
    int len = atoi(buf);
    while (*buf && *buf!='|') buf++;
    if (!*buf)
        throw cRuntimeError("cFileCommBuffer: unpack(): missing '|' with in string format");
    buf++;
    char *tmp = new char[len+1];
    memcpy(tmp,buf,len);
    tmp[len] = '\0';
    buf+=len;
    // skip delimiter whitespace
    while (*buf==' ' || *buf=='\n') buf++;
    d = tmp;
    mPosition = buf-mBuffer;
}

void cFileCommBuffer::unpack(opp_string& d)
{
    char *s;
    unpack((const char *&)s);
    d.reserve(strlen(s)+1);
    strcpy(d.buffer(),s);
    delete [] s;
}

void cFileCommBuffer::unpack(SimTime& d)
{
    opp_long_long raw;
    unpack(raw);
    d.setRaw(raw);
}

void cFileCommBuffer::unpack(char *d, int size)
{
    EXTRACTARRAY(char,d,size);
}

void cFileCommBuffer::unpack(unsigned char *d, int size)
{
    EXTRACTARRAY(unsigned char,d,size);
}

void cFileCommBuffer::unpack(bool *d, int size)
{
    EXTRACTARRAY(bool,d,size);
}

void cFileCommBuffer::unpack(short *d, int size)
{
    EXTRACTARRAY(short,d,size);
}

void cFileCommBuffer::unpack(unsigned short *d, int size)
{
    EXTRACTARRAY(unsigned short,d,size);
}

void cFileCommBuffer::unpack(int *d, int size)
{
    EXTRACTARRAY(int,d,size);
}

void cFileCommBuffer::unpack(unsigned int *d, int size)
{
    EXTRACTARRAY(unsigned,d,size);
}

void cFileCommBuffer::unpack(long *d, int size)
{
    EXTRACTARRAY(long,d,size);
}

void cFileCommBuffer::unpack(unsigned long *d, int size)
{
    EXTRACTARRAY(unsigned long,d,size);
}

void cFileCommBuffer::unpack(opp_long_long *d, int size)
{
    EXTRACTARRAY(opp_long_long,d,size);
}

void cFileCommBuffer::unpack(opp_unsigned_long_long *d, int size)
{
    EXTRACTARRAY(opp_unsigned_long_long,d,size);
}

void cFileCommBuffer::unpack(float *d, int size)
{
    EXTRACTARRAY(float,d,size);
}

void cFileCommBuffer::unpack(double *d, int size)
{
    EXTRACTARRAY(double,d,size);
}

void cFileCommBuffer::unpack(long double *d, int size)
{
    EXTRACTARRAY(long double,d,size);
}

void cFileCommBuffer::unpack(const char **d, int size)
{
    for (int i = 0; i < size; i++)
        unpack(d[i]);
}

void cFileCommBuffer::unpack(opp_string *d, int size)
{
    for (int i = 0; i < size; i++)
        unpack(d[i]);
}

void cFileCommBuffer::unpack(SimTime *d, int size)
{
    for (int i = 0; i < size; i++)
        unpack(d[i]);
}

