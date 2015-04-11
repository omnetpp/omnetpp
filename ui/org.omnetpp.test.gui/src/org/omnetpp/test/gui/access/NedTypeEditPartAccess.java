/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.test.gui.access;

import org.omnetpp.ned.editor.graph.parts.NedTypeEditPart;

import com.simulcraft.test.gui.access.EditPartAccess;


public class NedTypeEditPartAccess extends EditPartAccess
{
    public NedTypeEditPartAccess(NedTypeEditPart nedTypeEditPart) {
        super(nedTypeEditPart);
    }

    public NedTypeEditPart getNedTypeEditPart() {
        return (NedTypeEditPart)editPart;
    }
}
