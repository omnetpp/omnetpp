//=========================================================================
//  JPROGRESSMONITOR.H - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _JPROGRESSMONITOR_H_
#define _JPROGRESSMONITOR_H_

#include <jni.h>
#include "progressmonitor.h"

/*
 * Implementation of the IProgressMonitor interface
 * that delegates to an org.eclipse.core.runtime.IProgressMonitor
 * java object.
 */
class JniProgressMonitor : public IProgressMonitor
{
	private:
		jobject jProgressMonitor;
		jmethodID beginTaskID, doneID, isCanceledID,
					setCanceledID, subTaskID, workedID;
		
		JNIEnv *env;
	public:
	
	JniProgressMonitor() : env(NULL) {}
	JniProgressMonitor(jobject jProgressMonitor, JNIEnv *env);
	virtual ~JniProgressMonitor();

	virtual void beginTask(std::string name, int totalWork);
	virtual void done();
	virtual bool isCanceled();
	virtual void setCanceled(bool value);
	virtual void subTask(std::string name);
	virtual void worked(int work);
};

#endif

