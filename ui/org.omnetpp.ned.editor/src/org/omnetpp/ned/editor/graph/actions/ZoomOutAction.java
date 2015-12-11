/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.actions;

import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.swt.SWT;
import org.eclipse.ui.IWorkbenchPart;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.ned.editor.graph.parts.CompoundModuleEditPart;

/**
 * Zoom out the currently selected compound module
 *
 * @author rhornig
 */
public class ZoomOutAction extends CompoundModuleAction {

    public static final String ID = "ZoomOut";
    public static final String MENUNAME = "Zoom &Out";
    public static final String TOOLTIP = "Zoom out compound module";
    public static final ImageDescriptor IMAGE = ImageFactory.global().getDescriptor(ImageFactory.TOOLBAR_IMAGE_ZOOMMINUS);

    public ZoomOutAction(IWorkbenchPart part) {
        super(part);
        setText(MENUNAME);
        setId(ID);
        setToolTipText(TOOLTIP);
        setImageDescriptor(IMAGE);
        setHoverImageDescriptor(IMAGE);
        setAccelerator(SWT.KEYPAD_SUBTRACT | SWT.CTRL);
    }

    @Override
    protected boolean calculateEnabled() {
        CompoundModuleEditPart compoundModule = getSelectionCompoundModule();
        return compoundModule != null && compoundModule.canZoomOut();
    }

    @Override
    public void run() {
        CompoundModuleEditPart compoundModule = getSelectionCompoundModule();
        compoundModule.zoomOut();
        compoundModule.getRoot().getContents().refresh();
    }
}
