//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

package networks;

//
// A network extended with a scenario manager
//
network DumbbellScenario extends Dumbbell
{
    submodules:
        scenarioManager: ScenarioManager;
}

