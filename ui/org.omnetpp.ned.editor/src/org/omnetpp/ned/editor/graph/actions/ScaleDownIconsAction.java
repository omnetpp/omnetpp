/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.actions;

import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.swt.SWT;
import org.eclipse.ui.IWorkbenchPart;
import org.omnetpp.ned.editor.graph.parts.CompoundModuleEditPart;

/**
 * Scale up submodule icons in the currently selected compound module
 *
 * @author andras
 */
public class ScaleDownIconsAction extends CompoundModuleAction {

    public static final String ID = "ScaleDownIcons";
    public static final String MENUNAME = "Decrease Icon Size";
    public static final String TOOLTIP = "Decrease submodule icon size";
    public static final ImageDescriptor IMAGE = null;

    public ScaleDownIconsAction(IWorkbenchPart part) {
        super(part);
        setText(MENUNAME);
        setId(ID);
        setToolTipText(TOOLTIP);
        setImageDescriptor(IMAGE);
        setHoverImageDescriptor(IMAGE);
        setAccelerator(SWT.CTRL | 'O');
    }

    @Override
    public void run() {
        CompoundModuleEditPart compoundModule = getSelectionCompoundModule();
        float scale = compoundModule.getIconScale();
        if (scale > 0.3)
            compoundModule.setIconScale(scale * 0.8f);
        compoundModule.getRoot().getContents().refresh();
    }
}
