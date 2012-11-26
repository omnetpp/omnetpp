package org.omnetpp.simulation.inspectors;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.MouseEvent;
import org.eclipse.draw2d.MouseListener;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.ToolBarManager;
import org.eclipse.swt.SWT;
import org.omnetpp.common.ui.HoverInfo;
import org.omnetpp.simulation.canvas.IInspectorContainer;
import org.omnetpp.simulation.controller.CommunicationException;
import org.omnetpp.simulation.figures.FigureUtils;
import org.omnetpp.simulation.figures.IInspectorFigure;
import org.omnetpp.simulation.figures.InfoTextInspectorFigure;
import org.omnetpp.simulation.inspectors.actions.CloseAction;
import org.omnetpp.simulation.inspectors.actions.InspectAsObjectAction;
import org.omnetpp.simulation.inspectors.actions.InspectParentAction;
import org.omnetpp.simulation.model.cObject;
import org.omnetpp.simulation.ui.ObjectTreeHoverInfo;

/**
 *
 * @author Andras
 */
//XXX make more options what to display: class+name/fullpath, info, detailedinfo; change color, shape etc
public class InfoTextInspectorPart extends AbstractInspectorPart {

    public InfoTextInspectorPart(IInspectorContainer parent, cObject object, InspectorDescriptor descriptor) {
        super(descriptor, parent, object);

        // add mouse selection support
        figure.addMouseListener(new MouseListener.Stub() {
            @Override
            public void mousePressed(MouseEvent me) {
                handleMousePressed(me);
            }
        });
    }

    @Override
    protected IInspectorFigure createFigure() {
        InfoTextInspectorFigure figure = new InfoTextInspectorFigure();
        getContainer().addMoveResizeSupport(figure);
        return figure;
    }

    @Override
    public void refresh() {
        super.refresh();
        if (!isDisposed()) {
            InfoTextInspectorFigure infoTextFigure = (InfoTextInspectorFigure)figure;
            try {
                object.loadIfUnfilled();
                infoTextFigure.setTexts("(" + object.getClassName() + ") " + object.getFullPath(), object.getInfo());
            }
            catch (CommunicationException e) {
                infoTextFigure.setTexts("(" + object.getClass().getSimpleName() + "?) n/a", "Error loading object info, try Refresh");
            }
        }
    }

    @Override
    public boolean isMaximizable() {
        return false;
    }

    public void populateContextMenu(MenuManager contextMenuManager, int x, int y) {
        contextMenuManager.add(my(new CloseAction()));
    }

    @Override
    public void populateFloatingToolbar(ToolBarManager manager) {
        manager.add(my(new InspectParentAction()));
        manager.add(my(new InspectAsObjectAction()));
    }

    protected void handleMousePressed(MouseEvent me) {
        System.out.println("TextInspectorPart: mouse pressed");
        if ((me.getState()& SWT.CONTROL) != 0)
            inspectorContainer.toggleSelection(getObject());
        else
            inspectorContainer.select(getObject(), true);
        //note: no me.consume()! it would kill the move/resize listener
    }

    @Override
    public HoverInfo getHoverFor(int x, int y) {
        return new ObjectTreeHoverInfo(new Object[] { object }, getContainer());
    }

    @Override
    public int getDragOperation(IFigure figure, int x, int y) {
        return FigureUtils.getBorderResizeInsideMoveDragOperation(x, y, figure.getBounds());
    }
}
