//=========================================================================
//  CCOMMBUFFER.H - part of
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

#ifndef __CCOMMBUFFER_H
#define __CCOMMBUFFER_H

#include "cobject.h"
#include "simtime.h"
#include "platdep/intxtypes.h"

NAMESPACE_BEGIN

// forward declaration:
class cCommBuffer;
class cOwnedObject;
class opp_string;


/**
 * Buffer for the communications layer of parallel simulation.
 * This is an abstract class which has different implementations
 * for specific communication mechanisms.
 * For example, the MPI implementation, cMPICommBuffer encapsulates
 * MPI_Pack() and MPI_Unpack().
 *
 * @see cOwnedObject::parsimPack(), cOwnedObject::parsimUnpack()
 *
 * @ingroup ParsimBrief
 * @ingroup Parsim
 */
class SIM_API cCommBuffer : public cObject
{
  public:
    /**
     * Virtual destructor
     */
    virtual ~cCommBuffer() {}

    /** @name Buffer management */
    //@{
    /**
     * Returns true if all data in buffer was used up during unpacking.
     * Returns false if there was underflow (too much data unpacked)
     * or still there's unpacked data in the buffer.
     */
    virtual bool isBufferEmpty() const = 0;

    /**
     * Utility function. To be called after unpacking a communication buffer,
     * it checks whether the buffer is empty. If it is not (i.e. an underflow
     * or overflow occurred), an exception is thrown.
     */
    virtual void assertBufferEmpty() = 0;
    //@}

    /** @name Pack basic types */
    //@{
    /**
     * Packs a character.
     */
    virtual void pack(char d) = 0;
    /**
     * Packs an unsigned character.
     */
    virtual void pack(unsigned char d) = 0;
    /**
     * Packs a bool.
     */
    virtual void pack(bool d) = 0;
    /**
     * Packs a short int.
     */
    virtual void pack(short d) = 0;
    /**
     * Packs an unsigned short int.
     */
    virtual void pack(unsigned short d) = 0;
    /**
     * Packs an int.
     */
    virtual void pack(int d) = 0;
    /**
     * Packs an unsigned int.
     */
    virtual void pack(unsigned int d) = 0;
    /**
     * Packs a long int.
     */
    virtual void pack(long d) = 0;
    /**
     * Packs an unsigned long int.
     */
    virtual void pack(unsigned long d) = 0;
    /**
     * Packs a long long (on MSVC, __int64).
     */
    virtual void pack(opp_long_long d) = 0;
    /**
     * Packs an unsigned long long (on MSVC, __int64).
     */
    virtual void pack(opp_unsigned_long_long d) = 0;
    /**
     * Packs a float.
     */
    virtual void pack(float d) = 0;
    /**
     * Packs a double.
     */
    virtual void pack(double d) = 0;
    /**
     * Packs a long double.
     */
    virtual void pack(long double d) = 0;
    /**
     * Packs a string.
     */
    virtual void pack(const char *d) = 0;
    /**
     * Packs a string.
     */
    virtual void pack(const opp_string& d) = 0;
    /**
     * Packs a SimTime.
     */
    virtual void pack(SimTime d) = 0;
    //@}

    /** @name Pack arrays of basic types */
    //@{
    /**
     * Packs an array of chars.
     */
    virtual void pack(const char *d,  int size) = 0;
    /**
     * Packs an array of unsigned chars.
     */
    virtual void pack(const unsigned char *d,  int size) = 0;
    /**
     * Packs an array of bools.
     */
    virtual void pack(const bool *d, int size) = 0;
    /**
     * Packs an array of shorts.
     */
    virtual void pack(const short *d, int size) = 0;
    /**
     * Packs an array of unsigned shorts.
     */
    virtual void pack(const unsigned short *d, int size) = 0;
    /**
     * Packs an array of ints.
     */
    virtual void pack(const int *d, int size) = 0;
    /**
     * Packs an array of unsigned ints.
     */
    virtual void pack(const unsigned int *d, int size) = 0;
    /**
     * Packs an array of longs.
     */
    virtual void pack(const long *d, int size) = 0;
    /**
     * Packs an array of unsigned longs.
     */
    virtual void pack(const unsigned long *d, int size) = 0;
    /**
     * Packs an array of long long's (on MSVC, __int64).
     */
    virtual void pack(const opp_long_long *d, int size) = 0;
    /**
     * Packs an array of unsigned long long's (on MSVC, __int64).
     */
    virtual void pack(const opp_unsigned_long_long *d, int size) = 0;
    /**
     * Packs an array of floats.
     */
    virtual void pack(const float *d, int size) = 0;
    /**
     * Packs an array of doubles.
     */
    virtual void pack(const double *d, int size) = 0;
    /**
     * Packs an array of long doubles.
     */
    virtual void pack(const long double *d, int size) = 0;
    /**
     * Packs an array of strings.
     */
    void pack(char **d, int size)  {pack((const char **)d,size);}
    /**
     * Packs an array of strings.
     */
    virtual void pack(const char **d, int size) = 0;
    /**
     * Packs an array of opp_strings.
     */
    virtual void pack(const opp_string *d, int size) = 0;
    /**
     * Packs an array of SimTimes.
     */
    virtual void pack(const SimTime *d, int size) = 0;
    //@}

    /** @name Unpack basic types */
    //@{
    /**
     * Unpacks a char.
     */
    virtual void unpack(char& d) = 0;
    /**
     * Unpacks an unsigned char.
     */
    virtual void unpack(unsigned char& d) = 0;
    /**
     * Unpacks a bool.
     */
    virtual void unpack(bool& d) = 0;
    /**
     * Unpacks a short.
     */
    virtual void unpack(short& d) = 0;
    /**
     * Unpacks an unsigned short.
     */
    virtual void unpack(unsigned short& d) = 0;
    /**
     * Unpacks an int.
     */
    virtual void unpack(int& d) = 0;
    /**
     * Unpacks an unsigned int.
     */
    virtual void unpack(unsigned int& d) = 0;
    /**
     * Unpacks a long.
     */
    virtual void unpack(long& d) = 0;
    /**
     * Unpacks an unsigned long.
     */
    virtual void unpack(unsigned long& d) = 0;
    /**
     * Unpacks a long long (on MSVC, __int64).
     */
    virtual void unpack(opp_long_long& d) = 0;
    /**
     * Unpacks an unsigned long long (on MSVC, __int64).
     */
    virtual void unpack(opp_unsigned_long_long& d) = 0;
    /**
     * Unpacks a float.
     */
    virtual void unpack(float& d) = 0;
    /**
     * Unpacks a double.
     */
    virtual void unpack(double& d) = 0;
    /**
     * Unpacks a long double.
     */
    virtual void unpack(long double& d) = 0;
    /**
     * Unpacks a string -- storage will be allocated for it via new char[].
     */
    void unpack(char *&d)  {unpack((const char *&)d);}
    /**
     * Unpacks a string -- storage will be allocated for it via new char[].
     */
    virtual void unpack(const char *&d) = 0;
    /**
     * Unpacks an opp_string.
     */
    virtual void unpack(opp_string& d) = 0;
    /**
     * Unpacks a SimTime.
     */
    virtual void unpack(SimTime& d) = 0;
    //@}

    /** @name Unpack arrays of basic types */
    //@{
    /**
     * Unpacks an array of chars.
     */
    virtual void unpack(char *d, int size) = 0;
    /**
     * Unpacks an array of unsigned chars.
     */
    virtual void unpack(unsigned char *d, int size) = 0;
    /**
     * Unpacks an array of bools.
     */
    virtual void unpack(bool *d, int size) = 0;
    /**
     * Unpacks an array of shorts.
     */
    virtual void unpack(short *d, int size) = 0;
    /**
     * Unpacks an array of unsigned shorts.
     */
    virtual void unpack(unsigned short *d, int size) = 0;
    /**
     * Unpacks an array of ints.
     */
    virtual void unpack(int *d, int size) = 0;
    /**
     * Unpacks an array of unsigned ints.
     */
    virtual void unpack(unsigned int *d, int size) = 0;
    /**
     * Unpacks an array of longs.
     */
    virtual void unpack(long *d, int size) = 0;
    /**
     * Unpacks an array of unsigned longs.
     */
    virtual void unpack(unsigned long *d, int size) = 0;
    /**
     * Unpacks an array of long long's (on MSVC, __int64).
     */
    virtual void unpack(opp_long_long *d, int size) = 0;
    /**
     * Unpacks an array of unsigned long long's (on MSVC, __int64).
     */
    virtual void unpack(opp_unsigned_long_long *d, int size) = 0;
    /**
     * Unpacks an array of floats.
     */
    virtual void unpack(float *d, int size) = 0;
    /**
     * Unpacks an array of doubles.
     */
    virtual void unpack(double *d, int size) = 0;
    /**
     * Unpacks an array of long doubles.
     */
    virtual void unpack(long double *d, int size) = 0;
    /**
     * Unpacks an array of strings -- storage for strings will be allocated via new char[].
     */
    void unpack(char **d, int size)  {unpack((const char **)d,size);}
    /**
     * Unpacks an array of strings -- storage for strings will be allocated via new char[].
     */
    virtual void unpack(const char **d, int size) = 0;
    /**
     * Unpacks an array of opp_strings.
     */
    virtual void unpack(opp_string *d, int size) = 0;
    /**
     * Unpacks an array of SimTime.
     */
    virtual void unpack(SimTime *d, int size) = 0;
    //@}

    /** @name Utility functions */
    //@{
    /**
     * Stores and returns the given flag.
     */
    bool packFlag(bool flag)  {pack(flag); return flag;}

    /**
     * Unpacks and returns a bool flag stored by packFlag().
     */
    bool checkFlag() {bool flag; unpack(flag); return flag;}

    /**
     * Packs an object.
     */
    virtual void packObject(cObject *obj);

    /**
     * Unpacks and returns an object.
     */
    virtual cObject *unpackObject();
    //@}
};

NAMESPACE_END

#endif

