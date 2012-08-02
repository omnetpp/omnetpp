package org.omnetpp.simulation.inspectors;

import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.simulation.inspectors.actions.IInspectorAction;
import org.omnetpp.simulation.model.cObject;

/**
 * Default implementation for IInspectorPart, base class for inspector classes
 */
//TODO normal resize for SWT inspectors, module inspectors, etc
public abstract class AbstractInspectorPart implements IInspectorPart, IAdaptable {
    protected cObject object;
    protected IInspectorFigure figure;
    protected IInspectorContainer inspectorContainer;
    protected boolean isSelected;

    public AbstractInspectorPart(IInspectorContainer parent, cObject object) {
        this.object = object;
        this.inspectorContainer = parent;

        figure = createFigure();
        addMoveResizeSupport();
    }

    protected abstract IInspectorFigure createFigure();

    protected void addMoveResizeSupport() {
        // add move/resize/selection support
        new InspectorMouseListener(this); //XXX revise this listener!
    }

    @Override
    @SuppressWarnings("rawtypes")
    public Object getAdapter(Class adapter) {
        // being able to adapt to cObject helps working with the selection
        if (adapter.isInstance(object))
            return object;
        if (adapter.isInstance(this))
            return this;
        return null;
    }
    
//XXX this is the Figure-based implementation of floating controls. Drawback: it can never appear over SWT widgets like the object fields tree
//    class FloatingControlsLocator implements Locator {
//        public void relocate(IFigure target) {
//            // see RelativeLocator.relocate()
//            IFigure reference = AbstractInspectorPart.this.figure;
//            Rectangle targetBounds = new Rectangle(reference.getBounds());
//            reference.translateToAbsolute(targetBounds);
//            target.translateToRelative(targetBounds);
//
//            Dimension targetSize = target.getPreferredSize();
//
//            targetBounds.x += targetBounds.width - targetSize.width;
//            targetBounds.y -= targetSize.height;
//            targetBounds.setSize(targetSize);
//            target.setBounds(targetBounds);
//        }
//    }
//
////    private long controlsDisplayedTime;
//    protected void addFloatingControlsSupport() {
//        //XXX experimental; TODO fade-in/fade-out effect! (layer to be turned into AlphaLayer)
//        final IFigure floatingControls = createFloatingControls();
//        figure.addMouseMotionListener(new MouseMotionListener.Stub() {
//            @Override
//            public void mouseHover(MouseEvent me) {
//                getContainer().getControlsLayer().add(floatingControls);
//                getContainer().getControlsLayer().getLayoutManager().setConstraint(floatingControls, new FloatingControlsLocator());
////                controlsDisplayedTime = System.currentTimeMillis();
//            }
//
//            @Override
//            public void mouseMoved(MouseEvent me) {
//                if (floatingControls.getParent() != null) {
////                    long delay = System.currentTimeMillis() - controlsDisplayedTime;
////                    if (delay >= 1000) // leave it displayed at least for a few seconds
//                        getContainer().getControlsLayer().remove(floatingControls);
//                }
//            }
//        });
//
//        // force the locator to be invoked whenever the inspector figure moves; without this, the floating
//        // controls follow the inspector figure with a quite noticeable lag (apparently the layouter is only
//        // invoked when the mouse rests)
//        figure.addFigureListener(new FigureListener() {
//            @Override
//            public void figureMoved(IFigure source) {
//                getContainer().getControlsLayer().invalidate();
//            }
//        });
//    }
//
//    class ToolButton extends Button {
//        public ToolButton(Image image) {
//            super(image);
//            setRolloverEnabled(true);
//        }
//    }
//    class ToolbarSpace extends Figure {
//        public ToolbarSpace() {
//            setPreferredSize(8, 8);
//        }
//    }
//
//    protected IFigure createFloatingControls() {
//        IFigure toolbar = new Figure();
//        toolbar.setLayoutManager(new ToolbarLayout(true));
//        toolbar.setBorder(new LineBorder(ColorFactory.GREY50, 1));
//
//        //XXX dummy buttons
//        toolbar.add(new ToolButton(PlatformUI.getWorkbench().getSharedImages().getImage(ISharedImages.IMG_ETOOL_HOME_NAV)));
//        toolbar.add(new ToolButton(PlatformUI.getWorkbench().getSharedImages().getImage(ISharedImages.IMG_ETOOL_SAVE_EDIT)));
//        toolbar.add(new ToolButton(PlatformUI.getWorkbench().getSharedImages().getImage(ISharedImages.IMG_ETOOL_PRINT_EDIT)));
//        toolbar.add(new ToolbarSpace());
//        ToolButton closeButton = new ToolButton(PlatformUI.getWorkbench().getSharedImages().getImage(ISharedImages.IMG_TOOL_DELETE));
//        toolbar.add(closeButton);
//
//        closeButton.addActionListener(new ActionListener() {
//            @Override
//            public void actionPerformed(ActionEvent event) {
//                getContainer().close(AbstractInspectorPart.this);
//            }
//        });
//        return toolbar;
//    }

    protected IAction my(IInspectorAction action) {
        action.setInspectorPart(this);
        return action;
    }

    @SuppressWarnings({ "rawtypes", "unchecked" })
    public void raiseToTop() {
        // there's no public API in draw2d for changing the order (Z-order) of children, but
        // Randy Hudson himself suggests just to change the list returned by getChildren()...
        // http://dev.eclipse.org/mhonarc/lists/gef-dev/msg00914.html
        List siblings = figure.getParent().getChildren();
        siblings.remove(figure);
        siblings.add(figure);
        figure.getParent().invalidate();
    }

    public void dispose() {
        System.out.println("disposing inspector: " + object);

        if (figure != null) {
            if (figure.getParent() != null)
                figure.getParent().remove(figure);
            figure = null;
        }

        object = null;
    }

    public boolean isDisposed() {
        return object == null;
    }

    @Override
    public cObject getObject() {
        return object;
    }

    @Override
    public IInspectorFigure getFigure() {
        return figure;
    }

    @Override
    public boolean isSelected() {  //XXX needed? canvas holds the selection anyway, this only allows inconsistency!!!
        return isSelected;
    }

    @Override
    public void refresh() {
        Assert.isTrue(figure.getParent()!=null && inspectorContainer!=null, "inspector not yet installed");
        Assert.isTrue(object != null, "inspector already disposed");

        // automatically close the inspector when the underlying object gets deleted
        if (object.isDisposed()) {
            System.out.println("object disposed - auto-closing inspector: ");
            getContainer().close(this);
        }
    }

    @Override
    public IInspectorContainer getContainer() {
        return inspectorContainer;
    }

    @Override
    public void selectionChanged(IStructuredSelection selection) {
        boolean oldSelectedState = isSelected;
        isSelected = selection.toList().contains(object); //XXX or the inspectorPart???
        if (oldSelectedState != isSelected)
            figure.setSelectionBorder(isSelected);
    }

    @Override
    public String toString() {
        if (object.isDisposed())
            return getClass().getSimpleName() + ":<disposed>";
        else
            return getClass().getSimpleName() + ":(" + object.getClassName() + ")" + object.getFullPath();
    }
}