package org.omnetpp.runtimeenv.editors;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.MouseEvent;
import org.eclipse.draw2d.MouseListener;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.ISelectionProvider;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.omnetpp.common.ui.SelectionProvider;
import org.omnetpp.experimental.simkernel.swig.cDisplayString;
import org.omnetpp.experimental.simkernel.swig.cModule;
import org.omnetpp.experimental.simkernel.swig.cModule_SubmoduleIterator;
import org.omnetpp.experimental.simkernel.swig.cSimulation;
import org.omnetpp.figures.layout.SubmoduleConstraint;
import org.omnetpp.runtimeenv.figures.CompoundModuleFigureEx;
import org.omnetpp.runtimeenv.figures.SubmoduleFigureEx;

/**
 * 
 * @author Andras
 */
public class GraphicalModulePart extends InspectorPart {
    protected Map<Integer,SubmoduleFigureEx> submodules = new HashMap<Integer,SubmoduleFigureEx>();
    protected Map<Integer,String> lastSubmoduleDisplayStrings = new HashMap<Integer,String>();
    protected MouseListener mouseListener;
    protected ISelectionProvider selectionProvider = new SelectionProvider();
    
    /**
     * Constructor.
     */
    public GraphicalModulePart(cModule module) {
    	super(module);

        figure = new CompoundModuleFigureEx();
        figure.setInspectorPart(this);

        ((CompoundModuleFigureEx)figure).setDisplayString(module.getDisplayString());
        
//            //TODO just testing
//            ConnectionFigure connectionFigure = new ConnectionFigure();
//            connectionFigure.setArrowHeadEnabled(true);
//            connectionFigure.setSourceAnchor(new GateAnchor(submoduleFigure));
//            connectionFigure.setTargetAnchor(new CompoundModuleGateAnchor(moduleFigure));
//            moduleFigure.getConnectionLayer().add(connectionFigure);

        update();

        // handle mouse
        figure.addMouseListener(mouseListener = new MouseListener() {
            @Override
            public void mouseDoubleClicked(MouseEvent me) {
                handleMouseDoubleClick(me);
            }
            @Override
            public void mousePressed(MouseEvent me) {
                handleMousePressed(me);
            }
            @Override
            public void mouseReleased(MouseEvent me) {
                handleMouseReleased(me);
            }
        });
        
        // reflect selection changes
        selectionProvider.addSelectionChangedListener(new ISelectionChangedListener() {
            @Override
            public void selectionChanged(SelectionChangedEvent event) {
                // update visual feedback
                isSelected = false;
                if (event.getSelection() instanceof IStructuredSelection) {
                    IStructuredSelection selection = (IStructuredSelection)event.getSelection();
                    isSelected = selection.toList().contains(GraphicalModulePart.this);
                    ((CompoundModuleFigureEx)figure).setDragHandlesShown(isSelected);
                }
            }
        });
    }

	@Override
	public boolean isMaximizable() {
		return false;
	}
    
    @Override
	protected void update() {
        refreshChildren();
        refreshVisuals();
    }

    protected void refreshChildren() {
        //TODO only call this function if there were any moduleCreated/moduleDeleted notifications from the simkernel
    	CompoundModuleFigureEx moduleFigure = (CompoundModuleFigureEx)figure;
        cSimulation sim = cSimulation.getActiveSimulation();
        ArrayList<Integer> toBeRemoved = null;
        ArrayList<Integer> toBeAdded = null;
        
        // find submodule figures whose module has been deleted
        for (int id : submodules.keySet()) {
            if (sim.getModule(id) == null) { //FIXME create & use moduleExists(id) for efficiency
                if (toBeRemoved == null)
                    toBeRemoved = new ArrayList<Integer>();
                toBeRemoved.add(id);
            }
        }

        // find submodules that not yet have a figure
        for (cModule_SubmoduleIterator it = new cModule_SubmoduleIterator(cModule.cast(object)); !it.end(); it.next()) {
            int id = it.get().getId();  //FIXME performance: add getModuleId() to the iterator directly
            if (!submodules.containsKey(id)) {
                if (toBeAdded == null)
                    toBeAdded = new ArrayList<Integer>();
                toBeAdded.add(id);
            }
        }

        // do the removals and additions
        if (toBeRemoved != null) {
            for (int id : toBeRemoved) {
                moduleFigure.getSubmoduleLayer().remove(submodules.get(id));
                submodules.remove(id);
                lastSubmoduleDisplayStrings.remove(id);
            }
        }
        if (toBeAdded != null) {
            for (int id : toBeAdded) {
                // create figure
                SubmoduleFigureEx submoduleFigure = new SubmoduleFigureEx();
                submoduleFigure.setModuleID(id);
                submoduleFigure.setPinDecoration(false);
                submoduleFigure.setName(sim.getModule(id).getFullName());
                moduleFigure.getSubmoduleLayer().add(submoduleFigure);
                submodules.put(id, submoduleFigure);
                lastSubmoduleDisplayStrings.put(id, null);
            }
        }
    }

    protected void refreshVisuals() {
    	CompoundModuleFigureEx moduleFigure = (CompoundModuleFigureEx)figure;
        cSimulation sim = cSimulation.getActiveSimulation();
        for (int id : submodules.keySet()) {
            cDisplayString displayString = sim.getModule(id).getDisplayString();
            String displayStringText = displayString.toString();
            if (!displayStringText.equals(lastSubmoduleDisplayStrings.get(id))) {
                SubmoduleFigureEx submoduleFigure = submodules.get(id);
                submoduleFigure.setDisplayString(displayString);

                // layouting magic
                SubmoduleConstraint constraint = new SubmoduleConstraint();
                constraint.setLocation(submoduleFigure.getPreferredLocation());
                constraint.setSize(submoduleFigure.getPreferredSize());
                Assert.isTrue(constraint.height != -1 && constraint.width != -1);
                moduleFigure.getSubmoduleLayer().setConstraint(submoduleFigure, constraint);
                
                lastSubmoduleDisplayStrings.put(id, displayStringText);
            }
        }
    }

    protected void handleMousePressed(MouseEvent me) {
        System.out.println("clicked submodule: " + findSubmoduleAt(me.x,me.y));
    }

    protected void handleMouseDoubleClick(MouseEvent me) {
        // TODO Auto-generated method stub
    }
    
    protected void handleMouseReleased(MouseEvent me) {
        // TODO Auto-generated method stub
    }
    
    public SubmoduleFigureEx findSubmoduleAt(int x, int y) {
        IFigure target = figure.findFigureAt(x, y);
        while (target != null && !(target instanceof SubmoduleFigureEx))
            target = target.getParent();
        return (SubmoduleFigureEx)target;
    }

    @Override
    public ISelection getSelection() {
        return selectionProvider.getSelection();
    }
    
    @Override
    public void setSelection(ISelection selection) {
        selectionProvider.setSelection(selection);
    }

    @Override
    public void addSelectionChangedListener(ISelectionChangedListener listener) {
        selectionProvider.addSelectionChangedListener(listener);
    }
    
    @Override
    public void removeSelectionChangedListener(ISelectionChangedListener listener) {
        selectionProvider.removeSelectionChangedListener(listener);
    }
}
