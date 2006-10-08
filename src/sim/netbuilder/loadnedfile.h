//==========================================================================
//  CNEDFILELOADER.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `terms' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CNEDFILELOADER_H
#define __CNEDFILELOADER_H

#include <vector>
#include <map>
#include "defs.h"

class cProperties;
class cProperty;

class NEDElement;
class ParamNode;
class GateNode;
class PropertyNode;

/**
 * Loads NED files.
 *
 * How does dynamic NED loading work?
 *
 * - before network setup, we read some NED files, and register
 *   the modules types and network types in them (by creating cDynamicCompoundModuleType
 *   objects and adding them to the 'networktypes' list).
 *
 * - then we can instantiate _any_ network: it may contain module types that were
 *   read in dynamically, it'll work without problem.
 */
class SIM_API cNEDFileLoader
{
  protected:
    // storage for NED components not resolved yet because of missing dependencies
    std::vector<NEDElement *> pendingList;

  protected:
/*
    NEDElement *parseAndValidateNedFile(const char *nedfname, bool isXML);
    bool areDependenciesResolved(NEDElement *node);
    void tryResolvePendingDeclarations();
    void buildNEDDeclaration(NEDElement *node);
    cNEDDeclaration::ParamDescription extractParamDescription(ParamNode *paramNode);
    cNEDDeclaration::GateDescription extractGateDescription(GateNode *gateNode);
    cProperties *extractProperties(NEDElement *parent);
    cProperty *extractProperty(PropertyNode *propNode);
*/

  public:
    cNEDFileLoader();
    ~cNEDFileLoader();

    /**
     * Parses the given NED file, and converts modules/channels/interfaces to
     * cNEDDeclaration. Components that depend on other components
     * (e.g. contain "extends") may be temporarily put off until all
     * dependencies are loaded.
     */
    void loadNedFile(const char *nedfname, bool isXML);

    /**
     * Issues errors for components that are still unresolved because of
     * missing (unloaded) dependencies.
     */
    void done();
};

#endif


