//==========================================================================
//  OBJECTPRINTER.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Levente Meszaros
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OBJECTPRINTER_H
#define __OBJECTPRINTER_H

#include <vector>
#include <iostream>
#include "matchexpression.h"
#include "matchablefield.h"
#include "matchableobject.h"

NAMESPACE_BEGIN


/**
 * A utility class to serialize an object in text form. It is currently used
 * to record message data into the event log file.
 */
class ENVIR_API ObjectPrinter
{
    protected:
        int indentSize;
        char buffer[1024];
        std::vector<MatchExpression> objectMatchExpressions;
        std::vector<std::vector<MatchExpression> > fieldNameMatchExpressionsList;

    public:
        /**
         * FIXME Levy: pls document parameters and operation
         */
        ObjectPrinter(std::vector<MatchExpression> &objectMatchExpressions,
                      std::vector<std::vector<MatchExpression> > &fieldNameMatchExpressionsList,
                      int indentSize);

        void printObjectToStream(std::ostream& ostream, cObject *object);
        std::string printObjectToString(cObject *object);

    protected:
        void printIndent(std::ostream& ostream, int level);
        void printObjectToStream(std::ostream& ostream, void *object, cClassDescriptor *descriptor, int level);
        bool matchesObjectField(cObject *object, int fieldIndex);
};

NAMESPACE_END


#endif

