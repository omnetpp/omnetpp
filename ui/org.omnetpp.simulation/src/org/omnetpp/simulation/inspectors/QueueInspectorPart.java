package org.omnetpp.simulation.inspectors;

import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.MouseEvent;
import org.eclipse.draw2d.MouseListener;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.ToolBarManager;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.swt.SWT;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.ui.HoverInfo;
import org.omnetpp.simulation.canvas.IInspectorContainer;
import org.omnetpp.simulation.canvas.SelectionItem;
import org.omnetpp.simulation.canvas.SelectionUtils;
import org.omnetpp.simulation.controller.CommunicationException;
import org.omnetpp.simulation.figures.FigureUtils;
import org.omnetpp.simulation.figures.IInspectorFigure;
import org.omnetpp.simulation.figures.QueueInspectorFigure;
import org.omnetpp.simulation.inspectors.actions.CloseAction;
import org.omnetpp.simulation.inspectors.actions.InspectAsObjectAction;
import org.omnetpp.simulation.inspectors.actions.InspectParentAction;
import org.omnetpp.simulation.model.cObject;
import org.omnetpp.simulation.ui.ObjectTreeHoverInfo;

/**
 * Experimental graphical inspector for queues. Note: the code does not assume the object is
 * actually a cQueue -- it simply displays the object's children (getChildObjects())
 * in a queue-like figure.
 *
 * @author Andras
 */
//XXX add tooltip for message items
//XXX add similar context menu as GraphicalModuleInspectorPart has
//XXX lots of stuff common with GraphicalModuleInspectorPart -- factor them out?
public class QueueInspectorPart extends AbstractInspectorPart {
    private Map<cObject,IFigure> objectToFigureMap = new HashMap<cObject, IFigure>();
    private Map<IFigure,cObject> figureToObjectMap = new HashMap<IFigure, cObject>();
    private cObject[] prevObjects = null; // just to detect changes...

    public QueueInspectorPart(InspectorDescriptor descriptor, IInspectorContainer parent, cObject object) {
        super(descriptor, parent, object);

        // mouse handling
        //XXX near copy-paste from GraphicalModuleInspectorPart, factor out!
        figure.addMouseListener(new MouseListener() {
            //@Override
            public void mouseDoubleClicked(MouseEvent me) {
                handleMouseDoubleClick(me);
            }

            //@Override
            public void mousePressed(MouseEvent me) {
                handleMousePressed(me);
            }

            //@Override
            public void mouseReleased(MouseEvent me) {
                handleMouseReleased(me);
            }
        });
    }

    @Override
    protected IInspectorFigure createFigure() {
        QueueInspectorFigure figure = new QueueInspectorFigure();
        getContainer().addMoveResizeSupport(figure);
        return figure;
    }

    @Override
    public void refresh() {
        super.refresh();

        if (!isDisposed()) {
            cObject[] childObjects;
            try {
                object.loadIfUnfilled();
                childObjects = object.getChildObjects();
                object.getSimulation().loadUnfilledObjects(childObjects);
            }
            catch (CommunicationException e) {
                return; // leave everything as it is now
            }

            if (!Arrays.equals(childObjects, prevObjects)) {
                populateQueueFigure(childObjects);
                prevObjects = childObjects;
            }
        }
    }

    protected void populateQueueFigure(cObject[] childObjects) {
        // clear and re-add message figures
        QueueInspectorFigure queueFigure = (QueueInspectorFigure)figure;
        queueFigure.removeAll();
        objectToFigureMap.clear();
        figureToObjectMap.clear();
        for (cObject msg : childObjects) {
            IFigure msgFigure = queueFigure.createQueueItemFigure();
            queueFigure.add(msgFigure);
            objectToFigureMap.put(msg,msgFigure);
            figureToObjectMap.put(msgFigure,msg);
        }
    }

    public IFigure findQueueItemFigureAt(int x, int y) {
        for (IFigure f = figure.findFigureAt(x, y); f != null && f != figure; f = f.getParent())
            if (figureToObjectMap.containsKey(f))
                return f;
        return null;
    }


    @Override
    public boolean isMaximizable() {
        return false;
    }

    @Override
    public void populateContextMenu(MenuManager contextMenuManager, int x, int y) {
        contextMenuManager.add(my(new CloseAction()));
    }

    @Override
    public void populateFloatingToolbar(ToolBarManager manager) {
        manager.add(my(new InspectParentAction()));
        manager.add(my(new InspectAsObjectAction()));
    }

    @Override
    public void selectionChanged(IStructuredSelection selection) {
        super.selectionChanged(selection);

        // update selection border around submodules
        List<cObject> selectedLocalObjects = SelectionUtils.getObjects(selection, this, cObject.class);
        for (cObject obj : objectToFigureMap.keySet()) {
            IFigure itemFigure = objectToFigureMap.get(obj);
            if (itemFigure != null)
                itemFigure.setForegroundColor(selectedLocalObjects.contains(obj) ? ColorFactory.BLACK : ColorFactory.RED);
        }
    }

    //XXX near copy/paste from GraphicalModuleInspectorPart - factor out
    protected void handleMouseDoubleClick(MouseEvent me) {
        IFigure queueItemFigure = findQueueItemFigureAt(me.x,me.y);
        System.out.println("clicked item: " + queueItemFigure);
        if (queueItemFigure != null) {
            cObject obj = figureToObjectMap.get(queueItemFigure);
            getContainer().inspect(obj);
        }
    }

    //XXX near copy/paste from GraphicalModuleInspectorPart - factor out
    protected void handleMousePressed(MouseEvent me) {
        IFigure queueItemFigure = findQueueItemFigureAt(me.x,me.y);
        System.out.println("clicked item: " + queueItemFigure);
        if (queueItemFigure == null) {
            if ((me.getState()&SWT.CONTROL) != 0)
                inspectorContainer.toggleSelection(getObject());
            else
                inspectorContainer.select(getObject(), true);
        }
        else {
            cObject obj = figureToObjectMap.get(queueItemFigure);
            if ((me.getState()&SWT.CONTROL) != 0)
                inspectorContainer.toggleSelection(new SelectionItem(this, obj));
            else
                inspectorContainer.select(new SelectionItem(this, obj), true);
        }
        //note: no me.consume()! it would kill the move/resize listener
    }

    protected void handleMouseReleased(MouseEvent me) {
    }

    @Override
    public HoverInfo getHoverFor(int x, int y) {
        IFigure queueItemFigure = findQueueItemFigureAt(x,y);
        if (queueItemFigure != null) {
            cObject queueItem = figureToObjectMap.get(queueItemFigure);
            return new ObjectTreeHoverInfo(new Object[] { queueItem }, getContainer());
        }
        else {
            return new ObjectTreeHoverInfo(new Object[] { object }, getContainer());
        }
    }

    @Override
    public int getDragOperation(IFigure figure, int x, int y) {
        return FigureUtils.getBorderResizeInsideMoveDragOperation(x, y, figure.getBounds());
    }
}
