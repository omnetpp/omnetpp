package org.omnetpp.simulation.inspectors;

import org.eclipse.draw2d.InputEvent;
import org.eclipse.draw2d.MouseEvent;
import org.eclipse.draw2d.MouseListener;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.swt.graphics.Point;
import org.omnetpp.simulation.figures.InfoTextInspectorFigure;
import org.omnetpp.simulation.model.cObject;

/**
 *
 * @author Andras
 */
//XXX make more options what to display: class+name/fullpath, info, detailedinfo; change color, shape etc
public class InfoTextInspectorPart extends InspectorPart {

	public InfoTextInspectorPart(cObject object) {
		super(object);

		if (object.isFilledIn())
            object.safeLoad();
		
		figure = new InfoTextInspectorFigure();
		figure.setInspectorPart(this);

        // add move/resize/selection support
        new InspectorMouseListener(this); //XXX revise this 
		
		// add mouse selection support
        figure.addMouseListener(new MouseListener.Stub() {
			@Override
			public void mousePressed(MouseEvent me) {
                handleMousePressed(me);
			}
        });
	}

	@Override
	public void refresh() {
		super.refresh();
		if (!isDisposed()) {
			((InfoTextInspectorFigure)figure).setTexts("(" + object.getClassName() + ") " + object.getFullPath(), object.getInfo());
		}
	}

	//@Override
	public boolean isMaximizable() {
		return false;
	}

	//@Override
	public void populateContextMenu(MenuManager contextMenuManager, Point p) {
        contextMenuManager.add(new Action("Close") {
            @Override
            public void run() {
                getContainer().close(InfoTextInspectorPart.this);
            }
        });
	}

	protected void handleMousePressed(MouseEvent me) {
		System.out.println("TextInspectorPart: mouse pressed");
		if ((me.getState()& InputEvent.CONTROL) != 0)
			inspectorContainer.toggleSelection(getObject());
		else
			inspectorContainer.select(getObject(), true);
		//note: no me.consume()! it would kill the move/resize listener
	}
}
