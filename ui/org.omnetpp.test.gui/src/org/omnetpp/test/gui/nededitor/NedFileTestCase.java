/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.test.gui.nededitor;

import org.omnetpp.test.gui.access.NedEditorAccess;
import org.omnetpp.test.gui.core.ProjectFileTestCase;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.WorkbenchWindowAccess;


public abstract class NedFileTestCase
    extends ProjectFileTestCase
{
    public NedFileTestCase() {
        super("test.ned");
    }

    public void assertBothEditorsAreAccessible() {
        NedEditorUtils.assertBothEditorsAreAccessible(fileName);
    }

    protected NedEditorAccess findNedEditor() {
        WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindow();
        return (NedEditorAccess)workbenchWindow.findMultiPageEditorPartByTitle(fileName);
    }

    protected void typeIntoTextualNedEditor(String nedSource) {
        NedEditorUtils.typeIntoTextualNedEditor(fileName, nedSource);
    }
}
