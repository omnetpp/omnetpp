package org.omnetpp.ned.editor.graph.actions;

import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.IWorkbenchPart;
import org.omnetpp.common.image.ImageFactory;


public class ShowPropertyViewAction extends org.eclipse.gef.ui.actions.SelectionAction {

    public static final String ID = "ShowPropertyView";
    public static final String MENUNAME = "Properties";
    public static final String TOOLTIP = "Shows the property view for the selected objecT(s)";
    public static final ImageDescriptor IMAGE = ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_LAYOUT);

    public ShowPropertyViewAction(IWorkbenchPart part) {
        super(part);
        setText(MENUNAME);
        setId(ID);
        setToolTipText(TOOLTIP);
        setImageDescriptor(IMAGE);
        setHoverImageDescriptor(IMAGE);
    }

    @Override
    protected boolean calculateEnabled() {
        // TODO Auto-generated method stub
        return false;
    }
}
