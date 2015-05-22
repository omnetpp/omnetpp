//==========================================================================
//  CMESSAGEPRINTER.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_CMESSAGEPRINTER_H
#define __OMNETPP_CMESSAGEPRINTER_H

#include "cownedobject.h"

NAMESPACE_BEGIN

class cMessage;

/**
 * Base class for message printers.
 *
 * @see Register_MessagePrinter()
 * @ingroup EnvirExtensions
 */
class SIM_API cMessagePrinter : public cNoncopyableOwnedObject
{
    public:
        cMessagePrinter() {}
        virtual ~cMessagePrinter() {}

        /**
         * Guideline:
         *  0 or negative: does not support this message
         *  10: returned by cDefaultMessagePrinter
         */
        virtual int getScoreFor(cMessage *msg) const = 0;

        /**
         * TODO
         */
        virtual void printMessage(std::ostream& os, cMessage *msg) const = 0;
};

class SIM_API cDefaultMessagePrinter : public cMessagePrinter
{
    public:
        cDefaultMessagePrinter() {}
        virtual ~cDefaultMessagePrinter() {}
        virtual int getScoreFor(cMessage *msg) const override;
        virtual void printMessage(std::ostream& os, cMessage *msg) const override;
};

NAMESPACE_END

#endif


