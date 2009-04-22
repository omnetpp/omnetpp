package org.omnetpp.runtimeenv.editors;

import org.eclipse.draw2d.InputEvent;
import org.eclipse.draw2d.MouseEvent;
import org.eclipse.draw2d.MouseListener;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.swt.graphics.Point;
import org.omnetpp.experimental.simkernel.swig.cObject;
import org.omnetpp.runtimeenv.figures.TextInspectorFigure;

public class TextInspectorPart extends InspectorPart {

	public TextInspectorPart(cObject object) {
		super(object);
		figure = new TextInspectorFigure();
		figure.setInspectorPart(this);
		update();

		// add mouse selection support
        figure.addMouseListener(new MouseListener.Stub() {
			@Override
			public void mousePressed(MouseEvent me) {
                handleMousePressed(me);
			}
        });
	}

	@Override
	protected void update() {
		super.update();
		((TextInspectorFigure)figure).setTexts("(" + object.getClassName() + ") " + object.getFullPath(), object.info());
	}

	@Override
	public boolean isMaximizable() {
		return false;
	}

	@Override
	public void populateContextMenu(MenuManager contextMenuManager, Point p) {
		// nothing for now
		System.out.println(this + ": populateContextMenu invoked");
	}

	protected void handleMousePressed(MouseEvent me) {
		System.out.println("TextInspectorPart: mouse pressed");
		if ((me.getState()& InputEvent.CONTROL) != 0)
			selectionRequestHandler.toggleSelection(getObject());
		else
			selectionRequestHandler.select(getObject(), true);
		//note: no me.consume()! it would kill the move/resize listener
	}
}
