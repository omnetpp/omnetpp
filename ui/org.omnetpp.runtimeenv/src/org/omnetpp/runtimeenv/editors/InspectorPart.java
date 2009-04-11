package org.omnetpp.runtimeenv.editors;

import org.eclipse.draw2d.IFigure;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.experimental.simkernel.swig.cObject;
import org.omnetpp.runtimeenv.Activator;
import org.omnetpp.runtimeenv.ISimulationListener;
import org.omnetpp.runtimeenv.widgets.FigureCanvas;

/**
 * Default implementation for IInspectorPart, base class for inspector classes
 */
public abstract class InspectorPart implements IInspectorPart {
	protected cObject object;
	protected IInspectorFigure figure;
	protected ISimulationListener simulationListener;
	protected ISelectionRequestHandler selectionRequestHandler;
	protected boolean isSelected;

	public InspectorPart(cObject object) {
		super();
		
		this.object = object;

		// update the inspector when something happens in the simulation
		Activator.getSimulationManager().addChangeListener(simulationListener = new ISimulationListener() {
			@Override
			public void changed() {
				update();
			}
		});
	}

	public void dispose() {
	    Activator.getSimulationManager().removeChangeListener(simulationListener);
	}

    public static IInspectorPart findInspectorPartAt(FigureCanvas canvas, int x, int y) {
        IFigure target = canvas.getRootFigure().findFigureAt(x, y);
        while (target != null && !(target instanceof IInspectorFigure))
            target = target.getParent();
        return target==null ? null : ((IInspectorFigure)target).getInspectorPart();
    }


	@Override
	public cObject getObject() {
		return object;
	}

	@Override
	public IInspectorFigure getFigure() {
	    return figure;
	}

	@Override
	public boolean isSelected() {  //XXX needed? canvas holds the selection anyway, this only allows inconsistency!!!
	    return isSelected;
	}

	protected abstract void update();
    
    @Override
    public String toString() {
        return getClass().getSimpleName() + ":(" + object.getClassName() + ")" + object.getFullPath();
    }

    public void setSelectionRequestHandler(ISelectionRequestHandler handler) {
    	selectionRequestHandler = handler;
    }

    @Override
    public ISelectionRequestHandler getSelectionRequestHandler() {
    	return selectionRequestHandler;
    }
    
    @Override
    public void selectionChanged(IStructuredSelection selection) {
    	boolean oldSelectedState = isSelected;
    	isSelected = selection.toList().contains(object);
    	if (oldSelectedState != isSelected)
    		figure.setSelectionBorder(isSelected);
    }

    

}