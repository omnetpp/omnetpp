package org.omnetpp.simulation.inspectors;

import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IPersistableElement;

/**
 *
 * @author Andras
 */
public class BlankCanvasEditorInput implements IEditorInput {
    private String name;

    public BlankCanvasEditorInput(String name) {
        this.name = name;
    }

    //@Override
    public boolean exists() {
        return false;
    }

    //@Override
    public ImageDescriptor getImageDescriptor() {
        return null;
    }

    //@Override
    public String getName() {
        return name;
    }

    //@Override
    public IPersistableElement getPersistable() {
        return null;
    }

    //@Override
    public String getToolTipText() {
        return "";
    }

    //@Override
    @SuppressWarnings({ "rawtypes" })
    public Object getAdapter(Class adapter) {
        return null;
    }

}
