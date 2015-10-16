//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __QUEUEING_QUEUEINGDEFS_H
#define __QUEUEING_QUEUEINGDEFS_H

#include <omnetpp.h>

using namespace omnetpp;

#if OMNETPP_VERSION < 0x0400
#  error At least OMNeT++/OMNEST version 4.0 required
#endif

#if defined(QUEUEING_EXPORT)
#  define QUEUEING_API OPP_DLLEXPORT
#elif defined(QUEUEING_IMPORT)
#  define QUEUEING_API OPP_DLLIMPORT
#else
#  define QUEUEING_API
#endif

#endif


