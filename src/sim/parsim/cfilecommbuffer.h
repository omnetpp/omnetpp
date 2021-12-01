//=========================================================================
//  CFILECOMMBUFFER.H - part of
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

#ifndef __OMNETPP_CFILECOMMBUFFER_H
#define __OMNETPP_CFILECOMMBUFFER_H

#include "ccommbufferbase.h"
#include "omnetpp/opp_string.h"

namespace omnetpp {


/**
 * @brief Communication buffer that packs data in a text representation
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
    cFileCommBuffer() {}

    /**
     * Destructor.
     */
    virtual ~cFileCommBuffer() {}

    /**
     * Redefined because we need to NUL-terminate the buffer.
     */
    void setMessageSize(int size);

    /** @name Pack basic types */
    //@{
    virtual void pack(char d) override;
    virtual void pack(unsigned char d) override;
    virtual void pack(bool d) override;
    virtual void pack(short d) override;
    virtual void pack(unsigned short d) override;
    virtual void pack(int d) override;
    virtual void pack(unsigned int d) override;
    virtual void pack(long d) override;
    virtual void pack(unsigned long d) override;
    virtual void pack(long long d) override;
    virtual void pack(unsigned long long d) override;
    virtual void pack(float d) override;
    virtual void pack(double d) override;
    virtual void pack(long double d) override;
    virtual void pack(const char *d) override;
    virtual void pack(const opp_string& d) override;
    virtual void pack(SimTime d) override;
    //@}

    /** @name Pack arrays of basic types */
    //@{
    virtual void pack(const char *d, int size) override;
    virtual void pack(const unsigned char *d, int size) override;
    virtual void pack(const bool *d, int size) override;
    virtual void pack(const short *d, int size) override;
    virtual void pack(const unsigned short *d, int size) override;
    virtual void pack(const int *d, int size) override;
    virtual void pack(const unsigned int *d, int size) override;
    virtual void pack(const long *d, int size) override;
    virtual void pack(const unsigned long *d, int size) override;
    virtual void pack(const long long *d, int size) override;
    virtual void pack(const unsigned long long *d, int size) override;
    virtual void pack(const float *d, int size) override;
    virtual void pack(const double *d, int size) override;
    virtual void pack(const long double *d, int size) override;
    virtual void pack(const char **d, int size) override;
    virtual void pack(const opp_string *d, int size) override;
    virtual void pack(const SimTime *d, int size) override;
    //@}

    /** @name Unpack basic types */
    //@{
    virtual void unpack(char& d) override;
    virtual void unpack(unsigned char& d) override;
    virtual void unpack(bool& d) override;
    virtual void unpack(short& d) override;
    virtual void unpack(unsigned short& d) override;
    virtual void unpack(int& d) override;
    virtual void unpack(unsigned int& d) override;
    virtual void unpack(long& d) override;
    virtual void unpack(unsigned long& d) override;
    virtual void unpack(long long& d) override;
    virtual void unpack(unsigned long long& d) override;
    virtual void unpack(float& d) override;
    virtual void unpack(double& d) override;
    virtual void unpack(long double& d) override;
    virtual void unpack(const char *&d) override;
    virtual void unpack(opp_string& d) override;
    virtual void unpack(SimTime& d) override;
    //@}

    /** @name Unpack arrays of basic types */
    //@{
    virtual void unpack(char *d, int size) override;
    virtual void unpack(unsigned char *d, int size) override;
    virtual void unpack(bool *d, int size) override;
    virtual void unpack(short *d, int size) override;
    virtual void unpack(unsigned short *d, int size) override;
    virtual void unpack(int *d, int size) override;
    virtual void unpack(unsigned int *d, int size) override;
    virtual void unpack(long *d, int size) override;
    virtual void unpack(unsigned long *d, int size) override;
    virtual void unpack(long long *d, int size) override;
    virtual void unpack(unsigned long long *d, int size) override;
    virtual void unpack(float *d, int size) override;
    virtual void unpack(double *d, int size) override;
    virtual void unpack(long double *d, int size) override;
    virtual void unpack(const char **d, int size) override;
    virtual void unpack(opp_string *d, int size) override;
    virtual void unpack(SimTime *d, int size) override;
    //@}
};

}  // namespace omnetpp


#endif

