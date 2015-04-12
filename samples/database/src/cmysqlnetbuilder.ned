//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


package mysql;

@namespace();

//
// Dynamically sets up a network, with the topology coming from a MySQL database.
// Modules will be created as children of the target module (denoted by the 
// "targetModule" parameter).
//
// This module needs to be located in the simulation, so that
// - its initialization takes place before the initialization of the target module, or
// - it is descendant of the target module
// Otherwise, the initialization of the created modules will be incorrect (stage 0 will be skipped).
//
// For example:
//   - inside the any ancestor of targetModule, preceding the targetModule,
//   - as first member of system module,
//   - as descendant of targetModule module.
//
simple cMySQLNetBuilder
{
    parameters:
        string mysqlConnectionName = default("mysql");   // database configuration object name in ini file
        string networkName;     // Network name in database
        string targetModule = default("");    // parent module for generated modules, empty means the parent of this module
}
