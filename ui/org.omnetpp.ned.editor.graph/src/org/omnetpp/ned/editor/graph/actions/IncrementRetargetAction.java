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

import org.eclipse.ui.actions.RetargetAction;
import org.omnetpp.ned.editor.graph.misc.ImageFactory;
import org.omnetpp.ned.editor.graph.misc.MessageFactory;

/**
 * @author hudsonr
 * @since 2.1
 */
public class IncrementRetargetAction extends RetargetAction {

    /**
     * Constructor for IncrementRetargetAction.
     * 
     * @param actionID
     * @param label
     */
    public IncrementRetargetAction() {
        super(IncrementDecrementAction.INCREMENT,
                MessageFactory.IncrementDecrementAction_Increment_ActionLabelText);
        setToolTipText(MessageFactory.IncrementDecrementAction_Increment_ActionToolTipText);
        setImageDescriptor(ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_GROW)); //$NON-NLS-1$
    }

}
