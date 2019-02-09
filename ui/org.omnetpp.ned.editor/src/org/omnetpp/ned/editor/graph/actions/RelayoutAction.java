/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.actions;

import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.IWorkbenchPart;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.ned.editor.graph.parts.CompoundModuleEditPart;

/**
 * Re-position unpinned submodules in the currently selected compound module
 *
 * @author rhornig, andras
 */
public class RelayoutAction extends CompoundModuleAction {

    public static final String ID = "Relayout";
    public static final String MENUNAME = "Re-&layout";
    public static final String TOOLTIP = "Re-layout compound module";
    public static final ImageDescriptor IMAGE = ImageFactory.global().getDescriptor(ImageFactory.TOOLBAR_IMAGE_LAYOUT);

    public RelayoutAction(IWorkbenchPart part) {
        super(part);
        setText(MENUNAME);
        setId(ID);
        setToolTipText(TOOLTIP);
        setImageDescriptor(IMAGE);
        setHoverImageDescriptor(IMAGE);
    }

    @Override
    public void run() {
        CompoundModuleEditPart compoundModule = getSelectionCompoundModule();
        compoundModule.changeLayout();
        compoundModule.getRoot().getContents().refresh();
    }

}
