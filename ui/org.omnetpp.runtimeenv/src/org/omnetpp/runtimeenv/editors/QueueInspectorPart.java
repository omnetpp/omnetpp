package org.omnetpp.runtimeenv.editors;

import org.eclipse.draw2d.InputEvent;
import org.eclipse.draw2d.MouseEvent;
import org.eclipse.draw2d.MouseListener;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.swt.graphics.Point;
import org.omnetpp.runtime.nativelibs.simkernel.cObject;
import org.omnetpp.runtime.nativelibs.simkernel.cQueue;
import org.omnetpp.runtimeenv.figures.QueueInspectorFigure;

/**
 * Graphical inspector for queues
 * @author Andras
 */
public class QueueInspectorPart extends InspectorPart {

	public QueueInspectorPart(cObject object) {
		super(object);
		figure = new QueueInspectorFigure();
		figure.setInspectorPart(this);

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
			cQueue queue = cQueue.cast(object);
			((QueueInspectorFigure)figure).setQueueContents(queue.getChildObjects());
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
                getContainer().close(QueueInspectorPart.this);
            }
        });
	}

	protected void handleMousePressed(MouseEvent me) {
		System.out.println("QueueInspectorPart: mouse pressed");
		if ((me.getState()& InputEvent.CONTROL) != 0)
			inspectorContainer.toggleSelection(getObject());
		else
			inspectorContainer.select(getObject(), true);
		//note: no me.consume()! it would kill the move/resize listener
	}
}
