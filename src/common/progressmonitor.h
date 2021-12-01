//=========================================================================
//  PROGRESSMONITOR.H - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_COMMON_PROGRESSMONITOR_H
#define __OMNETPP_COMMON_PROGRESSMONITOR_H

#include <string>
#include "omnetpp/platdep/platdefs.h"

namespace omnetpp {
namespace common {

/**
 * Interface for reporting progress. Compatible with Eclipse's IProgressMonitor class.
 */
class IProgressMonitor
{
    public:

    virtual ~IProgressMonitor() {}

    /* Constant indicating an unknown amount of work. */
    static const int UNKNOWN = -1;

    /**
     * Notifies that the main task is beginning.  This must only be called once
     * on a given progress monitor instance.
     *
     * @param name the name (or description) of the main task
     * @param totalWork the total number of work units into which
     *  the main task is been subdivided. If the value is <code>UNKNOWN</code>
     *  the implementation is free to indicate progress in a way which
     *  doesn't require the total number of work units in advance.
     */
    virtual void beginTask(std::string name, int totalWork) = 0;

    /**
     * Notifies that the work is done; that is, either the main task is completed
     * or the user canceled it. This method may be called more than once
     * (implementations should be prepared to handle this case).
     */
    virtual void done() = 0;

    /**
     * Returns whether cancelation of current operation has been requested.
     * Long-running operations should poll to see if cancelation
     * has been requested.
     *
     * @return <code>true</code> if cancellation has been requested,
     *    and <code>false</code> otherwise
     * @see #setCanceled(boolean)
     */
    virtual bool isCanceled() = 0;

    /**
     * Sets the cancel state to the given value.
     *
     * @param value <code>true</code> indicates that cancelation has
     *     been requested (but not necessarily acknowledged);
     *     <code>false</code> clears this flag
     * @see #isCanceled()
     */
    virtual void setCanceled(bool value) = 0;

    /**
     * Notifies that a subtask of the main task is beginning.
     * Subtasks are optional; the main task might not have subtasks.
     *
     * @param name the name (or description) of the subtask
     */
    virtual void subTask(std::string name) = 0;

    /**
     * Notifies that a given number of work unit of the main task
     * has been completed. Note that this amount represents an
     * installment, as opposed to a cumulative amount of work done
     * to date.
     *
     * @param work a non-negative number of work units just completed
     */
    virtual void worked(int work) = 0;
};

}  // namespace common
}  // namespace omnetpp


#endif

