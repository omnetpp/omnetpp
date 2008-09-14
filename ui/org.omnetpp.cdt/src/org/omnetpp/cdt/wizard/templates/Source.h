{{copyright}}

#ifndef __{{PROJECTNAME}}_SOURCE_H
#define __{{PROJECTNAME}}_SOURCE_H

#include <omnetpp.h>

{{namespace:}}namespace {{namespace}} {

/**
 * Generates messages; see NED file for more info.
 */
class Source : public cSimpleModule
{
  private:
    cMessage *timerMessage;

  public:
     Source();
     virtual ~Source();

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

{{namespace:}}}; // namespace

#endif

