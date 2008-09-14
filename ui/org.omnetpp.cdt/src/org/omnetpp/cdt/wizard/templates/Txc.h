{{copyright}}

#ifndef __{{PROJECTNAME}}_TCX_H
#define __{{PROJECTNAME}}_TCX_H

#include <string.h>
#include <omnetpp.h>


/**
 * Implements the Txc simple module. See the NED file for more information.
 */
class Txc : public cSimpleModule
{
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

#endif

