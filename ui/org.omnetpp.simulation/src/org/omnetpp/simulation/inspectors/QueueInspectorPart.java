package org.omnetpp.simulation.inspectors;

import java.io.IOException;
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
import org.eclipse.swt.graphics.Point;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.simulation.canvas.IInspectorContainer;
import org.omnetpp.simulation.canvas.SelectionItem;
import org.omnetpp.simulation.canvas.SelectionUtils;
import org.omnetpp.simulation.figures.FigureUtils;
import org.omnetpp.simulation.figures.IInspectorFigure;
import org.omnetpp.simulation.figures.QueueInspectorFigure;
import org.omnetpp.simulation.inspectors.actions.CloseAction;
import org.omnetpp.simulation.inspectors.actions.InspectParentAction;
import org.omnetpp.simulation.model.cObject;

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

    public QueueInspectorPart(IInspectorContainer parent, cObject object) {
        super(parent, object);

        if (object.isFilledIn())
            object.safeLoad(); //XXX why not in refresh()?

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
            QueueInspectorFigure queueFigure = (QueueInspectorFigure)figure;

            if (!object.isFilledIn())
                object.safeLoad(); //XXX exception handling

            // only rebuild everything if queue contents has changed
            cObject[] childObjects = object.getChildObjects();
            try {
                object.getSimulation().loadUnfilledObjects(childObjects);
            }
            catch (IOException e) {
                e.printStackTrace();  //FIXME better error handling
            }
            if (!Arrays.equals(childObjects, prevObjects)) {
                // clear and re-add message figures
                prevObjects = childObjects;
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
        }
    }

    public IFigure findQueueItemFigureAt(int x, int y) {
        for (IFigure f = figure.findFigureAt(x, y); f!=null && f!=figure; f=f.getParent())
            if (figureToObjectMap.containsKey(f))
                return f;
        return null;
    }


    @Override
    public boolean isMaximizable() {
        return false;
    }

    @Override
    public void populateContextMenu(MenuManager contextMenuManager, Point p) {
        contextMenuManager.add(my(new CloseAction()));
    }

    @Override
    public void populateFloatingToolbar(ToolBarManager manager) {
        manager.add(my(new InspectParentAction()));
    }

    @Override
    @SuppressWarnings("unchecked")
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
    public int getDragOperation(IFigure figure, int x, int y) {
        return FigureUtils.getBorderResizeInsideMoveDragOperation(x, y, figure.getBounds());
    }
}
