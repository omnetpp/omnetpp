//
// This file is part of an OMNeT++/OMNEST simulation test.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __STRESSDIRECT_H
#define __STRESSDIRECT_H

#include <omnetpp.h>

class RandomModuleSelector : public cVisitor
{
	protected:
		int numberOfVisitedModules;

	public:
		cModule *selectedModule;
		
	public:
		RandomModuleSelector();

		virtual void visit(cObject *object);
};

class StressDirect : public cSimpleModule
{
    protected:
		cModule *getRandomModule();
        void handleMessage(cMessage *msg);
};

#endif
