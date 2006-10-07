//==========================================================================
// nedutil.h - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
// Contents:
//   misc util functions
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#ifndef __NEDUTIL_H
#define __NEDUTIL_H

/**
 * @name Converting simulation time to and from string form.
 *
 * These functions are copies of the functions with similar names
 * (without the NED prefix) in the simulation kernel.
 *
 * @ingroup Functions
 */
//@{

/**
 * Convert a string to double. The string should have a format
 * similar to the one output by simtimeToStr() (like "1s 34ms").
 *
 * Returns -1 if the whole string cannot be interpreted as time.
 * Empty string (or only spaces+tabs) is also an error.
 * E.g. strtoSimtime("3s 600ms x") will return -1.
 */
double NEDStrToSimtime(const char *str);

/**
 * Convert the beginning of a string to double. Similar to
 * strToSimtime(), only it processes the string as far as it
 * can be interpreted as simulation time. It sets the pointer
 * passed to the first character which cannot be interpreted
 * as part of the time string, or to the terminating zero.
 * Empty string is accepted as 0.0.
 * E.g. strToSimtime0("3s 600ms x") will return 3.6 and the
 * pointerstr will point to the character 'x'.
 */
double NEDStrToSimtime0(const char *&str);

/**
 * Converts simulation time (passed as double) into a
 * string like "0.0120000 (12ms)". If no destination pointer
 * is given, uses a static buffer.
 */
char *NEDSimtimeToStr(double t, char *dest=NULL);
//@}

#endif

