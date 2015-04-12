//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


import mysql.cMySQLNetBuilder;

import node.Node;

@namespace();

//
// The network will by dynamically constructed by the "builder" submodule,
// based on the topology in the MySQL database.
//
network Net60
{
    submodules:
        builder: cMySQLNetBuilder;
}

