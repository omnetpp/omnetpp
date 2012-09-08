package org.omnetpp.simulation.inspectors;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.MouseEvent;
import org.eclipse.draw2d.MouseListener;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.ToolBarManager;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Point;
import org.omnetpp.simulation.figures.FigureUtils;
import org.omnetpp.simulation.figures.InfoTextInspectorFigure;
import org.omnetpp.simulation.model.cObject;

/**
 *
 * @author Andras
 */
//XXX make more options what to display: class+name/fullpath, info, detailedinfo; change color, shape etc
public class InfoTextInspectorPart extends AbstractInspectorPart {

    public InfoTextInspectorPart(IInspectorContainer parent, cObject object) {
        super(parent, object);

        if (object.isFilledIn())  //XXX why not in refresh?
            object.safeLoad();

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
            ((InfoTextInspectorFigure)figure).setTexts("(" + object.getClassName() + ") " + object.getFullPath(), object.getInfo());
        }
    }

    @Override
    public boolean isMaximizable() {
        return false;
    }

    @Override
    public void populateContextMenu(MenuManager contextMenuManager, Point p) {
        contextMenuManager.add(new Action("Close") {
            @Override
            public void run() {
                getContainer().close(InfoTextInspectorPart.this);
            }
        });
    }

    @Override
    public void populateFloatingToolbar(ToolBarManager manager) {
        // TODO Auto-generated method stub
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
    public int getDragOperation(IFigure figure, int x, int y) {
        return FigureUtils.getBorderResizeInsideMoveDragOperation(x, y, figure.getBounds());
    }
}
