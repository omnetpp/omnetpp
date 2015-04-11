/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package com.simulcraft.test.gui.access;

import org.eclipse.gef.EditPart;

public class EditPartAccess
    extends Access
{
    protected EditPart editPart;

    public EditPartAccess(EditPart editPart) {
        this.editPart = editPart;
    }

    public EditPart getEditPart() {
        return editPart;
    }
}
