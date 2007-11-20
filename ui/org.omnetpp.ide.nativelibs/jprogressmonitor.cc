//=========================================================================
//  JPROGRESSMONITOR.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <jni.h>
#include "jprogressmonitor.h"

JniProgressMonitor::JniProgressMonitor(jobject jProgressMonitor, JNIEnv *env)
	: jProgressMonitor(jProgressMonitor), env(env)
{
	jclass clazz = env->FindClass("org/eclipse/core/runtime/IProgressMonitor");
	if (clazz) {
		this->beginTaskID = env->GetMethodID(clazz, "beginTask", "(Ljava/lang/String;I)V");
		this->doneID = env->GetMethodID(clazz, "done", "()V");
		this->isCanceledID = env->GetMethodID(clazz, "isCanceled", "()Z");
		this->setCanceledID = env->GetMethodID(clazz, "setCanceled", "(Z)V");
		this->subTaskID = env->GetMethodID(clazz, "subTask", "(Ljava/lang/String;)V");
		this->workedID = env->GetMethodID(clazz, "worked", "(I)V");
	}
	if (!beginTaskID || !doneID || !isCanceledID || ! setCanceledID || !subTaskID || !workedID)
		env = NULL;
}

JniProgressMonitor::~JniProgressMonitor()
{
}

void JniProgressMonitor::beginTask(std::string name, int totalWork)
{
	if (env)
	{
		jstring jname = env->NewStringUTF(name.c_str());
		env->CallVoidMethod(jProgressMonitor, beginTaskID, jname, (jint)totalWork);
	}
}

void JniProgressMonitor::done()
{
	if (env)
	{
		env->CallVoidMethod(jProgressMonitor, doneID);
	}
}

bool JniProgressMonitor::isCanceled()
{
	if (env)
	{
		return env->CallBooleanMethod(jProgressMonitor, isCanceledID);
	}
	return false; 
}

void JniProgressMonitor::setCanceled(bool value)
{
	if (env)
	{
		env->CallVoidMethod(jProgressMonitor, setCanceledID, (jboolean)value);
	}
}

void JniProgressMonitor::subTask(std::string name)
{
	if (env)
	{
		jstring jname = env->NewStringUTF(name.c_str());
		env->CallVoidMethod(jProgressMonitor, subTaskID, jname);
	}
}

void JniProgressMonitor::worked(int work)
{
	if (env)
	{
		env->CallVoidMethod(jProgressMonitor, workedID, (jint)work);
	}
}

