//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2003-2008 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

// This module will be the base of the Tic and Toc types
simple Txc4b
{
    parameters:
        bool sendMsgOnInit = default(false); // declare a parameter if we send out a message on init
        int limit;  // declare the new limit parameter
        @display("i=block/routing");
    gates:
        input in;
        output out;
}

//
// Spcialize the simple module by specifying some parameters of the base module.
// We can assign a parameter value here or it can be done when this type will be used
// as a submodule somewhere in a network or compoud module.
// Note that the limit parameter is still unbound here.  
// In addition we put the display properties in the type too. 
//
simple Tic4b extends Txc4b
{
    parameters:
        @display("i=,cyan");
        sendMsgOnInit = true;	// Tic modules should send a message on init
}

//
// Spcialize the module by defining parameters. We could have left the whole body
// empty, because the default value of the sendMsgOnInit parameter is false anyway.
// Note that the limit parameter is still unbound here  
//
simple Toc4b extends Txc4b
{
    parameters:
        @display("i=,gold");
        sendMsgOnInit = false;	// Toc modules should NOT send a message on init
}

//
// Adding module parameters.
//
network Tictoc4b
{
    submodules:
        tic: Tic4b;  // the limit parameter is still unbound here. We will get it from the ini file
        toc: Toc4b; 
    connections:
        tic.out --> {  delay = 100ms; } --> toc.in;
        tic.in <-- {  delay = 100ms; } <-- toc.out;
}

