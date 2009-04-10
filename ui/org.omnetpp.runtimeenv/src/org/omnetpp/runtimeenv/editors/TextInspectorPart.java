package org.omnetpp.runtimeenv.editors;

import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.omnetpp.experimental.simkernel.swig.cObject;
import org.omnetpp.runtimeenv.figures.TextInspectorFigure;

public class TextInspectorPart extends InspectorPart {

	public TextInspectorPart(cObject object) {
		super(object);
		figure = new TextInspectorFigure();
		figure.setInspectorPart(this);
		update();
	}

	@Override
	public boolean isSelected() {
		return true; //XXX only so that it can be moved
	}
	
	@Override
	protected void update() {
		((TextInspectorFigure)figure).setTexts("(" + object.getClassName() + ") " + object.getFullPath(), object.info());
	}

	@Override
	public boolean isMaximizable() {
		return false;
	}

	@Override
	public void addSelectionChangedListener(ISelectionChangedListener listener) {
		// TODO Auto-generated method stub

	}

	@Override
	public ISelection getSelection() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public void removeSelectionChangedListener(ISelectionChangedListener listener) {
		// TODO Auto-generated method stub
	}

	@Override
	public void setSelection(ISelection selection) {
		// TODO Auto-generated method stub
	}

}
