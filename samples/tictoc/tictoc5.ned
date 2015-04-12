//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2003-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

//
// Same as Txc4. This module will be the base of the Tic and Toc types.
//
simple Txc5
{
    parameters:
        bool sendMsgOnInit = default(false);
        int limit = default(2);
        @display("i=block/routing");
    gates:
        input in;
        output out;
}

//
// Specialize the module by defining parameters. We could have left the whole body
// empty, because the default value of the sendMsgOnInit parameter is false anyway.
// Note that the limit parameter is still unbound here.
//
simple Tic5 extends Txc5
{
    parameters:
        @display("i=,cyan");
        sendMsgOnInit = true;   // Tic modules should send a message on init
}

//
// Specialize the module by defining parameters. We could have left the whole body
// empty, because the default value of the sendMsgOnInit parameter is false anyway.
// Note that the limit parameter is still unbound here.
//
simple Toc5 extends Txc5
{
    parameters:
        @display("i=,gold");
        sendMsgOnInit = false;  // Toc modules should NOT send a message on init
}

//
// Adding module parameters.
//
network Tictoc5
{
    submodules:
        tic: Tic5;  // the limit parameter is still unbound here. We will get it from the ini file
        toc: Toc5;
    connections:
        tic.out --> {  delay = 100ms; } --> toc.in;
        tic.in <-- {  delay = 100ms; } <-- toc.out;
}

