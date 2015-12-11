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
 * Zoom in the currently selected compound module
 *
 * @author andras
 */
public class ZoomInAction extends CompoundModuleAction {

    public static final String ID = "ZoomIn";
    public static final String MENUNAME = "Zoom &In";
    public static final String TOOLTIP = "Zoom in compound module";
    public static final ImageDescriptor IMAGE = ImageFactory.global().getDescriptor(ImageFactory.TOOLBAR_IMAGE_ZOOMPLUS);

    public ZoomInAction(IWorkbenchPart part) {
        super(part);
        setText(MENUNAME);
        setId(ID);
        setToolTipText(TOOLTIP);
        setImageDescriptor(IMAGE);
        setHoverImageDescriptor(IMAGE);
        setAccelerator(SWT.KEYPAD_ADD | SWT.CTRL);
    }

    @Override
    protected boolean calculateEnabled() {
        CompoundModuleEditPart compoundModule = getSelectionCompoundModule();
        return compoundModule != null && compoundModule.canZoomIn();
    }

    @Override
    public void run() {
        CompoundModuleEditPart compoundModule = getSelectionCompoundModule();
        compoundModule.zoomIn();
        compoundModule.getRoot().getContents().refresh();
    }
}
