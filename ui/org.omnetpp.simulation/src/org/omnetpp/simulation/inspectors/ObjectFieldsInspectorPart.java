package org.omnetpp.simulation.inspectors;

import java.util.List;

import org.eclipse.draw2d.FocusEvent;
import org.eclipse.draw2d.FocusListener;
import org.eclipse.draw2d.GridData;
import org.eclipse.draw2d.GridLayout;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.LineBorder;
import org.eclipse.draw2d.Panel;
import org.eclipse.draw2d.PositionConstants;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.jface.action.ToolBarManager;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.figures.TooltipFigure;
import org.omnetpp.simulation.canvas.IInspectorContainer;
import org.omnetpp.simulation.canvas.SelectionItem;
import org.omnetpp.simulation.canvas.SelectionUtils;
import org.omnetpp.simulation.controller.CommunicationException;
import org.omnetpp.simulation.figures.FigureUtils;
import org.omnetpp.simulation.figures.IInspectorFigure;
import org.omnetpp.simulation.figures.ObjectFieldsViewerFigure;
import org.omnetpp.simulation.inspectors.actions.CloseAction;
import org.omnetpp.simulation.inspectors.actions.InspectParentAction;
import org.omnetpp.simulation.inspectors.actions.SetModeAction;
import org.omnetpp.simulation.inspectors.actions.SortAction;
import org.omnetpp.simulation.model.cObject;
import org.omnetpp.simulation.ui.ObjectFieldsViewer.Mode;
import org.omnetpp.simulation.ui.ObjectFieldsViewer.Ordering;

/**
 *
 * @author Andras, Tomi
 */
//TODO adaptive label: display the most useful info that fits in the available space
public class ObjectFieldsInspectorPart extends AbstractInspectorPart {
    private InspectorFigure frame;
    private Label label;
    private ObjectFieldsViewerFigure viewer;

    public ObjectFieldsInspectorPart(InspectorDescriptor descriptor, IInspectorContainer parent, cObject object) {
        super(descriptor, parent, object);
    }

    private class InspectorFigure extends Panel implements IInspectorFigure {

        public InspectorFigure() {
            setRequestFocusEnabled(true);
        }

        @Override
        public Dimension getPreferredSize(int wHint, int hHint) {
            Dimension size = getSize();
            if (size != null)
                return size;
            return super.getPreferredSize(wHint, hHint);
        }

        @Override
        public Dimension getMinimumSize(int wHint, int hHint) {
            return new Dimension(100, 100);
        }
    }

    @Override
    protected IInspectorFigure createFigure() {
        frame = new InspectorFigure();
        frame.setLayoutManager(new GridLayout(1, false));
        frame.setSize(300, 200);
        frame.setBorder(new LineBorder(Display.getDefault().getSystemColor(SWT.COLOR_WIDGET_BORDER)));

        label = new Label();
        label.setLabelAlignment(PositionConstants.LEFT);
        frame.add(label);
        frame.setConstraint(label, new GridData(GridData.FILL, GridData.FILL, true, false));

        viewer = new ObjectFieldsViewerFigure();
        viewer.setMode(ObjectFieldsViewerFigure.getPreferredMode(object));
        viewer.setInput(object);
        frame.add(viewer);
        frame.setConstraint(viewer, new GridData(GridData.FILL, GridData.FILL, true, true));

        frame.addFocusListener(new FocusListener.Stub() {
            @Override public void focusGained(FocusEvent event) {
                viewer.requestFocus();
            }
        });

        // double-click to inspect
        viewer.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetDefaultSelected(SelectionEvent e) {
                // inspect the selected object(s)
                ISelection selection = viewer.getSelection();
                List<cObject> objects = SelectionUtils.getObjects(selection, cObject.class);
                getContainer().inspect(objects, true);
            }
        });

        // export selection
        viewer.addSelectionChangedListener(new ISelectionChangedListener() {
            @Override
            public void selectionChanged(SelectionChangedEvent e) {
                // wrap to SelectionItems and export to the inspector canvas
                Object[] array = ((IStructuredSelection)e.getSelection()).toArray();
                for (int i = 0; i < array.length; i++)
                    array[i] = new SelectionItem(ObjectFieldsInspectorPart.this, array[i]);
                getContainer().select(array, true);
            }
        });

        getContainer().addMoveResizeSupport(frame);

        return frame;
    }

    @Override
    public boolean isMaximizable() {
        return false;
    }

    public Mode getMode() {
        return viewer.getMode();
    }

    public void setMode(Mode mode) {
        viewer.setMode(mode);
        getContainer().updateFloatingToolbarActions();
    }

    public Ordering getOrdering() {
        return viewer.getOrdering();
    }

    public void setOrdering(Ordering ordering) {
        viewer.setOrdering(ordering);
        getContainer().updateFloatingToolbarActions();
    }

    @Override
    public void populateContextMenu(MenuManager contextMenuManager, int x, int y) {
        contextMenuManager.add(my(new CloseAction()));
    }

    @Override
    public void populateFloatingToolbar(ToolBarManager manager) {
        manager.add(my(new InspectParentAction()));
        manager.add(new Separator());
        manager.add(my(new SortAction()));
        manager.add(new Separator());
        manager.add(my(new SetModeAction(Mode.PACKET)));
        manager.add(my(new SetModeAction(Mode.ESSENTIALS)));
        manager.add(my(new SetModeAction(Mode.CHILDREN)));
        manager.add(my(new SetModeAction(Mode.GROUPED)));
        manager.add(my(new SetModeAction(Mode.INHERITANCE)));
        manager.add(my(new SetModeAction(Mode.FLAT)));
    }

    @Override
    public void refresh() {
        super.refresh();

        if (!isDisposed()) {
            try {
                object.loadIfUnfilled();
                String text = "(" + object.getShortTypeName() + ") " + object.getFullPath() + " - " + object.getInfo();
                label.setText(text);
                TooltipFigure tooltipFigure = new TooltipFigure();
                tooltipFigure.setText(text);
                label.setToolTip(tooltipFigure);  // because label text is usually not fully visible
            }
            catch (CommunicationException e) {
                String text = "(" + object.getClass().getSimpleName() + "?) n/a";
                label.setText(text);
                TooltipFigure tooltipFigure = new TooltipFigure();
                tooltipFigure.setText(text);
                label.setToolTip(tooltipFigure);
            }

            viewer.refresh();
        }
    }

    @Override
    public int getDragOperation(IFigure figure, int x, int y) {
        if (figure == frame)
            return FigureUtils.getBorderResizeInsideMoveDragOperation(x, y, figure.getBounds());
        if (figure == label || figure == viewer)
            return FigureUtils.getMoveOnlyDragOperation(x, y, figure.getBounds());
        return 0;
    }

    @Override
    protected void setSelectionMark(boolean isSelected) {
        super.setSelectionMark(isSelected);
    }
}
