package org.omnetpp.runtimeenv.editors;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.IFigure;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.common.ui.FigureCanvas;
import org.omnetpp.experimental.simkernel.swig.cObject;
import org.omnetpp.runtimeenv.Activator;
import org.omnetpp.runtimeenv.ISimulationListener;

/**
 * Default implementation for IInspectorPart, base class for inspector classes
 */
public abstract class InspectorPart implements IInspectorPart {
	protected cObject object;
	protected IInspectorFigure figure;
	protected IInspectorContainer inspectorContainer;
	protected ISimulationListener simulationListener;
	protected boolean isSelected;

	public InspectorPart(cObject object) {
		this.object = object;

		// update the inspector when something happens in the simulation
		Activator.getSimulationManager().addSimulationListener(simulationListener = new ISimulationListener() {
			@Override
			public void changed() {
				update();
			}
		});
	}

	public void dispose() {
		System.out.println("inspector disposed: " + object);
		object = null;
	    Activator.getSimulationManager().removeSimulationListener(simulationListener);
	}

	public boolean isDisposed() {
		return object == null;
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

	protected void update() {
		Assert.isTrue(object != null, "inspector already disposed");
		
		// automatically close the inspector when the underlying object gets deleted
		if (object.isZombie()) {
	    	System.out.println("object became zombie - auto-closing inspector: ");
			getContainer().close(this);
		}
	}
    
	@Override
	public void setContainer(IInspectorContainer container) {
		this.inspectorContainer = container;
	}

	@Override
    public IInspectorContainer getContainer() {
    	return inspectorContainer;
    }
    
    @Override
    public void selectionChanged(IStructuredSelection selection) {
    	boolean oldSelectedState = isSelected;
    	isSelected = selection.toList().contains(object); //XXX or the inspectorPart???
    	if (oldSelectedState != isSelected)
    		figure.setSelectionBorder(isSelected);
    }

    @Override
    public String toString() {
    	if (object.isZombie())
    		return getClass().getSimpleName() + ":<deleted-object>";
    	else
    		return getClass().getSimpleName() + ":(" + object.getClassName() + ")" + object.getFullPath();
    }
    

}