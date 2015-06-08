//=========================================================================
//  OCTAVEEXPORT.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Tamas Borbely
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_OCTAVEEXPORT_H
#define __OMNETPP_SCAVE_OCTAVEEXPORT_H

#include <cstdio>
#include <string>
#include <set>
#include "common/exception.h"
#include "scavedefs.h"
#include "xyarray.h"

NAMESPACE_BEGIN
namespace scave {

/**
 * Exports data in Octave's "save -text" format. This format can be loaded
 * into R (r-project.org) as well.
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
class SCAVE_API OctaveExport
{
    private:
        std::string fileName;
        FILE *f;
        int prec;
        std::set<std::string> savedVars;

//FIXME create separate OctaveFileWriter and OctaveExport (which builds upon the former)
    protected:
        void openFileIfNeeded();
        void writeMatrixHeader(const char *name, int rows, int columns);
        void writeDescription(const char *name, const char *description);
        void writeString(const char *name, const char *value);

    public:
        OctaveExport(const char *fileName);
        virtual ~OctaveExport();

        void setPrecision(int prec) {this->prec = prec;}
        std::string makeUniqueName(const char *nameHint);

        void saveVector(const char *name, const char *description, const XYArray *vec, int startIndex=0, int endIndex=-1);
        void saveVectorX(const char *name, const char *description, const XYArray *vec, int startIndex=0, int endIndex=-1);
        void saveVectorY(const char *name, const char *description, const XYArray *vec, int startIndex=0, int endIndex=-1);
        void close();
};

} // namespace scave
NAMESPACE_END


#endif


