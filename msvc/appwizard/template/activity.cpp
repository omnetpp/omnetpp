//-------------------------------------------------------------
// file: $$MODULE_NAME$$.cpp
//-------------------------------------------------------------


#include <omnetpp.h>

// $$MODULE_NAME$$ simple module class
//
// $$DESCRIPTION$$
//
class $$MODULE_NAME$$ : public cSimpleModule
{
    // scalar results (to be stored from finish())
    int total_msgs;  // just an example

    // member functions
    Module_Class_Members($$MODULE_NAME$$,cSimpleModule,$$STACKSIZE$$)
    virtual void activity();
    // ...
    // further member functions (if you want to break up activity() into smaller functions)
    // ...
    virtual void finish();
};

Define_Module( $$MODULE_NAME$$ );

void $$MODULE_NAME$$::activity()
{
    // initialize class data members
    total_msgs = 0;

    // retrieve parameter values, like this:
    const char *first_param = par("first_param");
    double second_param = par("second_param");
    
    // perform actual task of the module
    for (;;)
    {
        cMessage *msg = receive();
        // ...
        // process message
        // ...
    }   
}

void $$MODULE_NAME$$::finish()
{
    // save summary results (if any)
    recordScalar("total_msgs", total_msgs);
}
