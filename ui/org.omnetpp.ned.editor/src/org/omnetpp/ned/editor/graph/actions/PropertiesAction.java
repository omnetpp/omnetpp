/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.actions;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.gef.commands.Command;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IWorkbenchPart;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.ned.editor.graph.dialogs.PropertiesDialog;
import org.omnetpp.ned.editor.graph.parts.EditPartUtil;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.interfaces.INedModelProvider;


/**
 * Action to edit graphical and other properties of an item (module, submodule,
 * channel, connection, etc.)
 *
 * @author andras
 */
public class PropertiesAction extends org.eclipse.gef.ui.actions.SelectionAction {
    public static final String ID = "org.omnetpp.ned.editor.graph.Properties";
    public static final String MENUNAME = "&Properties...";
    public static final String TOOLTIP = "Edit properties";
    public static final ImageDescriptor IMAGE = ImageFactory.global().getDescriptor(ImageFactory.TOOLBAR_IMAGE_PROPERTIES);

    public PropertiesAction(IWorkbenchPart part) {
        super(part);
        setText(MENUNAME);
        setId(ID);
        setActionDefinitionId(ID);
        setAccelerator(SWT.CONTROL + SWT.CR);  // affects only the display. actual mapping is done in GraphicalNedEditor.getCommonHandler()
        setToolTipText(TOOLTIP);
        setImageDescriptor(IMAGE);
    }

    @Override
    protected boolean calculateEnabled() {
        boolean containsUneditable = false;
        for (Object obj : getSelectedObjects())
            if (!EditPartUtil.isEditable(obj))
                containsUneditable = true;
        if (containsUneditable)
            return false;

        List<INedElement> tmp = new ArrayList<INedElement>();
        for (Object obj : getSelectedObjects())
            if (obj instanceof INedModelProvider)
                tmp.add(((INedModelProvider)obj).getModel());
        INedElement[] elements = tmp.toArray(new INedElement[]{});
        return PropertiesDialog.supportsEditing(elements);
    }

    @Override
    public void run() {
        //  collect elements to edit. According to calculateEnabled(), this is
        // not empty, contains homogeneous items, all items are editable, etc.
        List<INedElement> tmp = new ArrayList<INedElement>();
        for (Object obj : getSelectedObjects())
            if (obj instanceof INedModelProvider)
                tmp.add(((INedModelProvider)obj).getModel());
        INedElement[] elements = tmp.toArray(new INedElement[]{});

        PropertiesDialog dialog = new PropertiesDialog(Display.getDefault().getActiveShell(), elements);

        if (dialog.open() != Dialog.OK)
            return; // cancelled

        Command command = dialog.getResultCommand();
        execute(command);
    }
}
