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
    // summary result data to be stored from finish()
    int total_msgs;  // just an example

    // member functions
    Module_Class_Members($$MODULE_NAME$$,cSimpleModule,$$STACKSIZE$$)
    virtual void activity();
    virtual void finish();
};

Define_Module( $$MODULE_NAME$$ );

void $$MODULE_NAME$$::activity()
{
    // initialize class data members
    total_msgs = 0;

    // retrieve parameter values
    const char *first_param = par("first_param");
    double second_param = par("second_param");
    
    // perform actual task of the module
    for (;;)
    {
        // ...
    }
}

void $$MODULE_NAME$$::finish()
{
    // save summary results (if any)
    writeScalar("total_msgs", total_msgs);
}
