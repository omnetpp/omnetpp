//=========================================================================
//  CFILECOMMBUFFER.H - part of
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

#ifndef __CFILECOMMBUFFER_H__
#define __CFILECOMMBUFFER_H__

#include "ccommbufferbase.h"

NAMESPACE_BEGIN


/**
 * Communication buffer that packs data in a text representation
 * into a memory buffer.
 *
 * @ingroup Parsim
 */
class SIM_API cFileCommBuffer : public cCommBufferBase
{
  public:
    /**
     * Constructor.
     */
    cFileCommBuffer();

    /**
     * Destructor.
     */
    virtual ~cFileCommBuffer();

    /**
     * Redefined because we need to NULL-terminate the buffer.
     */
    void setMessageSize(int size);

    /** @name Pack basic types */
    //@{
    virtual void pack(char d);
    virtual void pack(unsigned char d);
    virtual void pack(bool d);
    virtual void pack(short d);
    virtual void pack(unsigned short d);
    virtual void pack(int d);
    virtual void pack(unsigned int d);
    virtual void pack(long d);
    virtual void pack(unsigned long d);
    virtual void pack(_long_long_t d);
    virtual void pack(_unsigned_long_long_t d);
    virtual void pack(float d);
    virtual void pack(double d);
    virtual void pack(long double d);
    virtual void pack(const char *d);
    virtual void pack(const opp_string& d);
    virtual void pack(SimTime d);
    //@}

    /** @name Pack arrays of basic types */
    //@{
    virtual void pack(const char *d, int size);
    virtual void pack(const unsigned char *d, int size);
    virtual void pack(const bool *d, int size);
    virtual void pack(const short *d, int size);
    virtual void pack(const unsigned short *d, int size);
    virtual void pack(const int *d, int size);
    virtual void pack(const unsigned int *d, int size);
    virtual void pack(const long *d, int size);
    virtual void pack(const unsigned long *d, int size);
    virtual void pack(const _long_long_t *d, int size);
    virtual void pack(const _unsigned_long_long_t *d, int size);
    virtual void pack(const float *d, int size);
    virtual void pack(const double *d, int size);
    virtual void pack(const long double *d, int size);
    virtual void pack(const char **d, int size);
    virtual void pack(const opp_string *d, int size);
    virtual void pack(const SimTime *d, int size);
    //@}

    /** @name Unpack basic types */
    //@{
    virtual void unpack(char& d);
    virtual void unpack(unsigned char& d);
    virtual void unpack(bool& d);
    virtual void unpack(short& d);
    virtual void unpack(unsigned short& d);
    virtual void unpack(int& d);
    virtual void unpack(unsigned int& d);
    virtual void unpack(long& d);
    virtual void unpack(unsigned long& d);
    virtual void unpack(_long_long_t& d);
    virtual void unpack(_unsigned_long_long_t& d);
    virtual void unpack(float& d);
    virtual void unpack(double& d);
    virtual void unpack(long double& d);
    virtual void unpack(const char *&d);
    virtual void unpack(opp_string& d);
    virtual void unpack(SimTime& d);
    //@}

    /** @name Unpack arrays of basic types */
    //@{
    virtual void unpack(char *d, int size);
    virtual void unpack(unsigned char *d, int size);
    virtual void unpack(bool *d, int size);
    virtual void unpack(short *d, int size);
    virtual void unpack(unsigned short *d, int size);
    virtual void unpack(int *d, int size);
    virtual void unpack(unsigned int *d, int size);
    virtual void unpack(long *d, int size);
    virtual void unpack(unsigned long *d, int size);
    virtual void unpack(_long_long_t *d, int size);
    virtual void unpack(_unsigned_long_long_t *d, int size);
    virtual void unpack(float *d, int size);
    virtual void unpack(double *d, int size);
    virtual void unpack(long double *d, int size);
    virtual void unpack(const char **d, int size);
    virtual void unpack(opp_string *d, int size);
    virtual void unpack(SimTime *d, int size);
    //@}
};

NAMESPACE_END


#endif

