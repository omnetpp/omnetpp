//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

import "cmysqlnetbuilder";

//
// Represents a network node. This is just a dummy, but one could of course
// replace it with something useful as well.
//
module Node
    gates:
        in: in[];
        out: out[];
    connections nocheck:
endmodule

//
// The network will by dynamically constructed by the "builder" submodule,
// based on the topology in the MySQL database.
//
module Net60
    submodules:
        builder: cMySQLNetBuilder;
endmodule

network net60 : Net60
endnetwork
