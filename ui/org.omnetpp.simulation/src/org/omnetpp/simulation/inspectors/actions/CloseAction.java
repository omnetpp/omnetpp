package org.omnetpp.simulation.inspectors.actions;

import org.eclipse.ui.ISharedImages;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.simulation.inspectors.IInspectorPart;


public class CloseAction extends AbstractInspectorAction {
    public CloseAction() {
        super("Close", AS_PUSH_BUTTON, PlatformUI.getWorkbench().getSharedImages().getImageDescriptor(ISharedImages.IMG_TOOL_DELETE));
    }
    
    @Override
    public void run() {
        IInspectorPart inspector = getInspectorPart();
        inspector.getContainer().close(inspector);
    }
}
