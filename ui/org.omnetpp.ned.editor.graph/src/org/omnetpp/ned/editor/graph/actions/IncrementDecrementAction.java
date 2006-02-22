/*******************************************************************************
 * Copyright (c) 2000, 2004 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials 
 * are made available under the terms of the Common Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/cpl-v10.html
 * 
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *******************************************************************************/
package org.omnetpp.ned.editor.graph.actions;

import java.util.List;

import org.eclipse.gef.EditPart;
import org.eclipse.gef.Request;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.commands.CompoundCommand;
import org.eclipse.ui.IWorkbenchPart;
import org.omnetpp.ned.editor.graph.misc.ImageFactory;
import org.omnetpp.ned.editor.graph.misc.MessageFactory;
import org.omnetpp.ned.editor.graph.model.old.SubmoduleModel;

public class IncrementDecrementAction extends org.eclipse.gef.ui.actions.SelectionAction {

    private static final String INCREMENT_REQUEST = "Increment", //$NON-NLS-1$
            DECREMENT_REQUEST = "Decrement"; //$NON-NLS-1$

    public static final String INCREMENT = "Increment", //$NON-NLS-1$
            DECREMENT = "Decrement"; //$NON-NLS-1$

    Request request;

    public IncrementDecrementAction(IWorkbenchPart part, boolean increment) {
        super(part);
        if (increment) {
            request = new Request(INCREMENT_REQUEST);
            setText(MessageFactory.IncrementDecrementAction_Increment_ActionLabelText);
            setId(INCREMENT);
            setToolTipText(MessageFactory.IncrementDecrementAction_Increment_ActionToolTipText);
            setImageDescriptor(ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_GROW)); //$NON-NLS-1$
        } else {
            request = new Request(DECREMENT_REQUEST);
            setText(MessageFactory.IncrementDecrementAction_Decrement_ActionLabelText);
            setId(DECREMENT);
            setToolTipText(MessageFactory.IncrementDecrementAction_Decrement_ActionToolTipText);
            setImageDescriptor(ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_SHRINK)); //$NON-NLS-1$
        }
        setHoverImageDescriptor(getImageDescriptor());
    }

    protected boolean calculateEnabled() {
        return canPerformAction();
    }

    private boolean canPerformAction() {
        if (getSelectedObjects().isEmpty()) return false;
        List parts = getSelectedObjects();
        for (int i = 0; i < parts.size(); i++) {
            Object o = parts.get(i);
            if (!(o instanceof EditPart)) return false;
            EditPart part = (EditPart) o;
            // HR changed to be effective on simple modules only
            if (!(part.getModel() instanceof SubmoduleModel)) return false;
        }
        return true;
    }

    private Command getCommand() {
        List editparts = getSelectedObjects();
        CompoundCommand cc = new CompoundCommand();
        cc.setDebugLabel("Increment/Decrement LEDs");//$NON-NLS-1$
        for (int i = 0; i < editparts.size(); i++) {
            EditPart part = (EditPart) editparts.get(i);
            cc.add(part.getCommand(request));
        }
        return cc;
    }

    public void run() {
        execute(getCommand());
    }

}
