package org.omnetpp.runtimeenv.editors;

import org.eclipse.draw2d.IFigure;

public interface IInspectorFigure extends IFigure {

	/**
	 * Returns the associated inspector part.
	 */
	IInspectorPart getInspectorPart();
	
	/**
	 * Sets the associated inspector part.
	 */
	void setInspectorPart(IInspectorPart part);

	/**
     * If there's a move/resize handle at the given coordinates
     * (figure coordinates), return a binary OR of the appropriate
     * SWT.TOP, SWT.BOTTOM, SWT.LEFT, SWT.RIGHT constants, otherwise
     * return 0. For move operation it should set all four.
     */
    int getDragOperation(int x, int y);

   
}
