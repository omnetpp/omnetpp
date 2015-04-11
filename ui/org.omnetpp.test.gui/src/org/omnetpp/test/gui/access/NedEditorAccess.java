/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.test.gui.access;

import org.omnetpp.ned.editor.NedEditor;

import com.simulcraft.test.gui.access.MultiPageEditorPartAccess;
import com.simulcraft.test.gui.access.TextEditorAccess;

public class NedEditorAccess
    extends MultiPageEditorPartAccess
{

    public NedEditorAccess(NedEditor multiPageNedEditor) {
        super(multiPageNedEditor);
    }

    @Override
    public NedEditor getWorkbenchPart() {
        return (NedEditor)workbenchPart;
    }

    public TextEditorAccess getTextualNedEditor() {
        return (TextEditorAccess) createAccess(getWorkbenchPart().getTextEditor());
    }

    public TextEditorAccess activateTextEditor() {
        return (TextEditorAccess) activatePageEditor("Text");
    }

    public GraphicalNedEditorAccess activateGraphicalEditor() {
        return (GraphicalNedEditorAccess) activatePageEditor("Graphical");
    }

    public TextEditorAccess ensureActiveTextEditor() {
        return (TextEditorAccess)ensureActiveEditor("Text");
    }

    public GraphicalNedEditorAccess ensureActiveGraphicalEditor() {
        return (GraphicalNedEditorAccess)ensureActiveEditor("Graphical");
    }
}
