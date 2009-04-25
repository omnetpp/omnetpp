package org.omnetpp.runtimeenv.views;

import org.apache.commons.lang.StringUtils;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.viewers.DecoratingStyledCellLabelProvider;
import org.eclipse.jface.viewers.DoubleClickEvent;
import org.eclipse.jface.viewers.IDoubleClickListener;
import org.eclipse.jface.viewers.ILabelProviderListener;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.StyledString;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.jface.viewers.DelegatingStyledCellLabelProvider.IStyledLabelProvider;
import org.eclipse.jface.viewers.StyledString.Styler;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.TextStyle;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.ISharedImages;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.part.ViewPart;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.ui.SelectionProvider;
import org.omnetpp.runtime.nativelibs.simkernel.Simkernel;
import org.omnetpp.runtime.nativelibs.simkernel.cClassDescriptor;
import org.omnetpp.runtime.nativelibs.simkernel.cObject;
import org.omnetpp.runtime.nativelibs.simkernel.cSimulation;
import org.omnetpp.runtimeenv.Activator;
import org.omnetpp.runtimeenv.ISimulationListener;

/**
 *
 * @author Andras
 */
//TODO we should support user-supplied images as well
public class ObjectTreeView extends ViewPart implements ISimulationListener {
	public static final String ID = "org.omnetpp.runtimeenv.ObjectTreeView";

	protected TreeViewer viewer;
    protected MenuManager contextMenuManager = new MenuManager("#PopupMenu");

	class ViewContentProvider implements ITreeContentProvider {
	    public Object[] getChildren(Object element) {
	        if (element instanceof cObject)
	            return ((cObject)element).getChildObjects();
	        return new Object[0];
	    }

	    public Object[] getElements(Object inputElement) {
	        return getChildren(inputElement);
	    }

	    public Object getParent(Object element) {
	        if (element instanceof cObject && !((cObject)element).isZombie())  //otherwise "preservingSelection()" may crash
	            return ((cObject)element).getOwner();
            return null;
	    }

	    public boolean hasChildren(Object element) {
            return ((cObject)element).hasChildObjects();
	    }

	    public void inputChanged(Viewer viewer, Object oldInput, Object newInput) {
	        // Do nothing
	    }

	    public void dispose() {
            // Do nothing
        }
	}

    class ViewLabelProvider implements IStyledLabelProvider {
        private class ColorStyler extends Styler {
            Color color;
            public ColorStyler(Color color) { this.color = color; }
            @Override public void applyStyles(TextStyle textStyle) { textStyle.foreground = color; }
        };
        private Styler blueStyle = new ColorStyler(ColorFactory.BLUE3);
        private Styler greyStyle = new ColorStyler(ColorFactory.GREY60);

        public String getText(Object element) {
            //note: we use "\b...\b" for blue, and "\f" for grey coloring
            if (element instanceof cObject) {
                cObject obj = (cObject) element;
                String typeName = Simkernel.getObjectShortTypeName(obj);
                return obj.getFullName() + " \f(" + typeName + ")";
            }
            return element.toString();
        }

        @Override
        public Image getImage(Object element) {
            if (element instanceof cObject) {
                //FIXME cache image by object's classname!
                cObject object = (cObject)element;
                cClassDescriptor desc = cClassDescriptor.getDescriptorFor(object);
                String icon = desc.getProperty("icon");
                if (!StringUtils.isEmpty(icon)) {
                    return Activator.getCachedImage("icons/obj16/"+icon+".png");
                }
            }
            return PlatformUI.getWorkbench().getSharedImages().getImage(ISharedImages.IMG_OBJS_ERROR_TSK);
        }

        @Override
        public StyledString getStyledText(Object element) {
            String text = getText(element);
            int blueStartIndex = text.indexOf('\b', 0);
            int blueLength = blueStartIndex==-1 ? -1 : text.indexOf('\b', blueStartIndex+1)-blueStartIndex-1;
            if (blueLength > 0)
                text = text.replace("\b", "");
            int greyStartIndex = text.indexOf('\f');
            if (greyStartIndex != -1)
                text = text.replace("\f", "");

            StyledString styledString = new StyledString(text);
            if (greyStartIndex >= 0)
                styledString.setStyle(greyStartIndex, text.length()-greyStartIndex, greyStyle);
            if (blueLength > 0)
                styledString.setStyle(blueStartIndex, blueLength, blueStyle);
            return styledString;
        }

        @Override
        public boolean isLabelProperty(Object element, String property) {
            return true;
        }

        @Override
        public void dispose() {
            // nothing
        }

        @Override
        public void addListener(ILabelProviderListener listener) {
            // nothing
        }

        @Override
        public void removeListener(ILabelProviderListener listener) {
            // nothing
        }
    }

	/**
	 * This is a callback that will allow us to create the viewer and initialize
	 * it.
	 */
	public void createPartControl(Composite parent) {
		viewer = new TreeViewer(parent, SWT.DOUBLE_BUFFERED | SWT.MULTI | SWT.H_SCROLL | SWT.V_SCROLL);
		viewer.setContentProvider(new ViewContentProvider());
        viewer.setLabelProvider(new DecoratingStyledCellLabelProvider(new ViewLabelProvider(), null, null));
		viewer.setInput(cSimulation.getActiveSimulation());

        // create context menu
        getViewSite().registerContextMenu(contextMenuManager, viewer);
        viewer.getTree().setMenu(contextMenuManager.createContextMenu(viewer.getTree()));
        //TODO dynamic menu based on which object is selected

        // double-click opens an inspector
        viewer.addDoubleClickListener(new IDoubleClickListener() {
            public void doubleClick(DoubleClickEvent event) {
                Object element = ((IStructuredSelection)event.getSelection()).getFirstElement();
                if (element instanceof cObject)
                    Activator.openInspector2((cObject)element, false);
            }
        });

        // export our selection to the workbench
		getViewSite().setSelectionProvider(new SelectionProvider());
        viewer.addSelectionChangedListener(new ISelectionChangedListener() {
			@Override
			public void selectionChanged(SelectionChangedEvent event) {
				getViewSite().getSelectionProvider().setSelection(event.getSelection());
			}
        });

        // update when something in the simulation changes
		Activator.getSimulationManager().addSimulationListener(this);
	}

    /**
	 * Passing the focus request to the viewer's control.
	 */
	public void setFocus() {
		viewer.getControl().setFocus();
	}

	@Override
    public void changed() {
        viewer.refresh();
    }

    @Override
    public void dispose() {
        Activator.getSimulationManager().removeSimulationListener(this);
        super.dispose();
    }
}