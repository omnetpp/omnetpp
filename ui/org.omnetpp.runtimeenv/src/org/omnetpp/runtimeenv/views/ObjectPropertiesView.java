package org.omnetpp.runtimeenv.views;

import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.ISharedImages;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.part.ViewPart;
import org.omnetpp.experimental.simkernel.swig.cCollectChildrenVisitor;
import org.omnetpp.experimental.simkernel.swig.cObject;
import org.omnetpp.experimental.simkernel.swig.cSimulation;

public class ObjectPropertiesView extends ViewPart {
	public static final String ID = "org.omnetpp.runtimeenv.ObjectPropertiesView";

	private TreeViewer viewer;

	class ViewContentProvider implements ITreeContentProvider {
	    public Object[] getChildren(Object parentElement) {
	        if (parentElement instanceof cObject) {
	            cObject o = (cObject)parentElement;
	            cCollectChildrenVisitor visitor = new cCollectChildrenVisitor(o);
                visitor.process(o);
                int n = visitor.getArraySize();
                Object[] result = new Object[n];
                for (int i=0; i<n; i++)
                    result[i] = visitor.get(i);
                return result;
	        }
	        return new Object[0];
	    }

	    public Object[] getElements(Object inputElement) {
	        return getChildren(inputElement);
	    }

	    public Object getParent(Object element) {
            if (element instanceof cObject)
                return ((cObject)element).getOwner();
            return null;
	    }

	    public boolean hasChildren(Object element) {
	        return getChildren(element).length > 0;  //FIXME make it more efficient
	    }

	    public void inputChanged(Viewer viewer, Object oldInput, Object newInput) {
	        // Do nothing
	    }

	    public void dispose() {
            // Do nothing
        }
	}

	class ViewLabelProvider extends LabelProvider {
	    @Override
	    public String getText(Object element) {
	        if (element instanceof cObject) {
	            cObject obj = (cObject) element;
	            return "(" + obj.getClassName() + ") " + obj.getFullName() + " -- " + obj.info();
	        }
	        return super.getText(element);
	    }
	    
        @Override
		public Image getImage(Object obj) {
			return PlatformUI.getWorkbench().getSharedImages().getImage(ISharedImages.IMG_OBJ_ELEMENT);
		}
	}

	/**
	 * This is a callback that will allow us to create the viewer and initialize
	 * it.
	 */
	public void createPartControl(Composite parent) {
		viewer = new TreeViewer(parent, SWT.MULTI | SWT.H_SCROLL | SWT.V_SCROLL);
		viewer.setContentProvider(new ViewContentProvider());
		viewer.setLabelProvider(new ViewLabelProvider());
		viewer.setInput(cSimulation.getActiveSimulation());
	}

	/**
	 * Passing the focus request to the viewer's control.
	 */
	public void setFocus() {
		viewer.getControl().setFocus();
	}
}