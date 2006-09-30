//=========================================================================
//  OCTAVEEXPORT.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _OCTAVEEXPORT_H_
#define _OCTAVEEXPORT_H_

#include <stdio.h>
#include <string>
#include "scavedefs.h"
#include "xyarray.h"
#include "exception.h"

/**
 * Exports data in Octave's "save -text" format.
 *
 * To see the an example, type the following commands in Octave:
 * <pre>
 *    a = [1 2; 3 4; 5 6]
 *    save -text - a
 * </pre>
 *
 * The output:
 * <pre>
 *  # Created by Octave 2.1.73 ...
 *  # name: a
 *  # type: matrix
 *  # rows: 3
 *  # columns: 2
 *   1 2
 *   3 4
 *   5 6
 * </pre>
 *
 * If several variables are saved, the above pattern repeats.
 * Floating-point numbers appear to be written with 14 digit precision.
 *
 * This class exports output vectors as two-column matrices.
 */
class OctaveExport
{
    private:
        std::string fileName;
        FILE *f;
        int prec;

    protected:
        void openFileIfNeeded();
        void writeHeader(const char *name, const char *type, int rows, int columns);

    public:
        OctaveExport(const char *fileName);
        virtual ~OctaveExport();

        void saveVector(const char *name, const XYArray *vec, int startIndex, int endIndex);
        void saveVectorX(const char *name, const XYArray *vec, int startIndex, int endIndex);
        void saveVectorY(const char *name, const XYArray *vec, int startIndex, int endIndex);
        void close();
};

#endif


