/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.actions;

import org.eclipse.gef.ui.actions.SelectionAction;
import org.eclipse.swt.SWT;
import org.eclipse.ui.IWorkbenchPart;
import org.omnetpp.ned.core.NedResourcesPlugin;
import org.omnetpp.ned.editor.graph.parts.NedConnectionEditPart;
import org.omnetpp.ned.editor.graph.parts.NedEditPart;
import org.omnetpp.ned.editor.graph.parts.SubmoduleEditPart;
import org.omnetpp.ned.model.interfaces.INedTypeElement;

/**
 * Opens the type (or supertype) of the selected element
 *
 * @author rhornig
 */
public class OpenTypeAction extends SelectionAction {

    public static final String ID = "org.omnetpp.ned.editor.graph.OpenType";
    public static final String MENUNAME = "Open &Type";
    public static final String TOOLTIP = "Opens the type of the element";
    public static final String MENUNAME_NEDTYPE = "Open Supert&ype";
    public static final String TOOLTIP_NEDTYPE = "Opens the base type of the element";

    public OpenTypeAction(IWorkbenchPart part) {
        super(part);
        setText(MENUNAME);
        setId(ID);
        setActionDefinitionId(ID);
        setToolTipText(TOOLTIP);
        setAccelerator(SWT.F3);  // affects only the display. actual mapping is done in GraphicalNedEditor.getCommonHandler()
    }

    @Override
    protected boolean calculateEnabled() {
        int size = getSelectedObjects().size();
        if (size == 0)
            return false;
        Object primarySelection = getSelectedObjects().get(size-1);
        if (primarySelection instanceof SubmoduleEditPart || primarySelection instanceof NedConnectionEditPart) {
            setText(MENUNAME);
            setToolTipText(TOOLTIP);
        }
        else if (primarySelection instanceof NedEditPart) {
            setText(MENUNAME_NEDTYPE);
            setToolTipText(TOOLTIP_NEDTYPE);
        }

        return getNedTypeElementToOpen() != null;
    }

    /**
     * The ned type that must be opened for the actual primary selection. For types this is
     * the supertype (if exists) for submodules this is the type, for connection this is the channel
     * used. (Built in ned types are never returned.)
     */
    protected INedTypeElement getNedTypeElementToOpen() {
        int size = getSelectedObjects().size();
        if (size == 0)
            return null;

        Object primarySelection = getSelectedObjects().get(size-1);
        INedTypeElement modelToOpen = null;
        if  (primarySelection instanceof NedConnectionEditPart) {
            modelToOpen = ((NedConnectionEditPart)primarySelection).getNedTypeElementToOpen();
        }
        else if (primarySelection instanceof SubmoduleEditPart) {
            modelToOpen = ((SubmoduleEditPart)primarySelection).getNedTypeElementToOpen();
        }
        else if (primarySelection instanceof NedEditPart) {
            modelToOpen = ((NedEditPart)primarySelection).getNedTypeElementToOpen();
        }

        return modelToOpen;
    }

    @Override
    public void run() {
        INedTypeElement element = getNedTypeElementToOpen();
        if (element != null)
            NedResourcesPlugin.openNedElementInEditor(element);
    }
}
