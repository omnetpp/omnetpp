//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2019 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// An arc in a Petri net
//
channel Arc extends ned.IdealChannel
{
    @class(Arc);
    int multiplicity = default(1); // Arc multiplicity. Use negative values to create a inhibitor arc.
}
