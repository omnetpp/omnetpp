package org.omnetpp.runtimeenv.editors;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.InputEvent;
import org.eclipse.draw2d.MouseEvent;
import org.eclipse.draw2d.MouseListener;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.experimental.simkernel.swig.cDisplayString;
import org.omnetpp.experimental.simkernel.swig.cModule;
import org.omnetpp.experimental.simkernel.swig.cModule_SubmoduleIterator;
import org.omnetpp.experimental.simkernel.swig.cSimulation;
import org.omnetpp.figures.layout.SpringEmbedderLayout;
import org.omnetpp.figures.layout.SubmoduleConstraint;
import org.omnetpp.runtimeenv.Activator;
import org.omnetpp.runtimeenv.figures.CompoundModuleFigureEx;
import org.omnetpp.runtimeenv.figures.SubmoduleFigureEx;

/**
 * 
 * @author Andras
 */
public class GraphicalModulePart extends InspectorPart {
    protected Map<Integer,SubmoduleFigureEx> submodules = new HashMap<Integer,SubmoduleFigureEx>();
    protected Map<Integer,String> lastSubmoduleDisplayStrings = new HashMap<Integer,String>();
    
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

        // mouse handling
        figure.addMouseListener(new MouseListener() {
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
                submoduleFigure.setPinVisible(false);
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
                
                // FIXME should also set the scale factor here (from the containing compound module)
                // otherwise range, size cannot be correctly displayed

                // use the existing constraint if any
                SubmoduleConstraint constraint = submoduleFigure.getSubmoduleConstraint();
                if (constraint == null)                      // or create a new one if no constraint was created until now
                	constraint = new SubmoduleConstraint();  // the location is unspecified in this constraint by default
                
                Point dpsLoc = displayString.getLocation(1.0f);  //FIXME should use scale here
                // check if the figure has a display string that specified a location (figure is fixed)
                constraint.setPinned(dpsLoc != null);
                // use the location specified in the display string (if any) for pinned nodes,
                // otherwise just use the current position stored in the constraint (which can be either unspecified or
                // can store the result from the previous layout run)
                if (constraint.isPinned())
                	constraint.setLocation(dpsLoc);

                // use the preferred size of the figure for constraint size
                constraint.setSize(submoduleFigure.getPreferredSize());
                Assert.isTrue(constraint.height != -1 && constraint.width != -1);        
                // Debug.println("constraint for " + nameToDisplay + ": " + constraint);
                submoduleFigure.setSubmoduleConstraint(constraint);  // store the constraint (needed if a new constraint was created)
                
                lastSubmoduleDisplayStrings.put(id, displayStringText);
            }
        }
        //FIXME remove the net line!
        // requesting an incremental layout (which moves only unspecified nodes automatically, other pinned/unpinned nodes 
        // will be displayed according to the coordinates stored in the constarint object
        ((SpringEmbedderLayout)((CompoundModuleFigureEx)figure).getSubmoduleLayer().getLayoutManager()).requestIncrementalLayout();

    }

    protected void handleMouseDoubleClick(MouseEvent me) {
    	SubmoduleFigureEx submoduleFigure = findSubmoduleAt(me.x,me.y);
		System.out.println("clicked submodule: " + submoduleFigure);
		if (submoduleFigure != null) {
			int submoduleID = submoduleFigure.getModuleID();
            cModule submodule = cSimulation.getActiveSimulation().getModule(submoduleID);
            Activator.openInspector2(submodule, true); //XXX maybe rather: go into?
		}
    }

    protected void handleMousePressed(MouseEvent me) {
    	SubmoduleFigureEx submoduleFigure = findSubmoduleAt(me.x,me.y);
		System.out.println("clicked submodule: " + submoduleFigure);
		if (submoduleFigure == null) {
            if ((me.getState()& InputEvent.CONTROL) != 0)
            	selectionRequestHandler.toggleSelection(getObject());
            else
            	selectionRequestHandler.select(getObject(), true);
		}
		else {
			int submoduleID = submoduleFigure.getModuleID();
            cModule submodule = cSimulation.getActiveSimulation().getModule(submoduleID);
            if ((me.getState()& InputEvent.CONTROL) != 0)
            	selectionRequestHandler.toggleSelection(submodule);
            else
            	selectionRequestHandler.select(submodule, true);
		}
		//note: no me.consume()! it would kill the move/resize listener
    }

    protected void handleMouseReleased(MouseEvent me) {
    }

    public SubmoduleFigureEx findSubmoduleAt(int x, int y) {
        IFigure target = figure.findFigureAt(x, y);
        while (target != null && !(target instanceof SubmoduleFigureEx))
            target = target.getParent();
        return (SubmoduleFigureEx)target;
    }
    
    @Override
    @SuppressWarnings("unchecked")
    public void selectionChanged(IStructuredSelection selection) {
    	super.selectionChanged(selection);
    	
    	// update selection border around submodules
    	List list = selection.toList();
    	cSimulation sim = cSimulation.getActiveSimulation();
        for (SubmoduleFigureEx f : submodules.values()) {
			cModule module = sim.getModule(f.getModuleID());
			f.setSelectionBorderShown(list.contains(module));
        }
    }

    public void populateContextMenu(final MenuManager contextMenuManager, org.eclipse.swt.graphics.Point p) {
		System.out.println(this + ": populateContextMenu invoked");
        SubmoduleFigureEx submoduleFigure = findSubmoduleAt(p.x, p.y);
        if (submoduleFigure != null) {
            int submoduleID = submoduleFigure.getModuleID();
            final cModule module = cSimulation.getActiveSimulation().getModule(submoduleID);

            //XXX factor out actions
            contextMenuManager.add(new Action("Go into") {
                @Override
                public void run() {
                    //Close this inspector; open new inspector at these coordinates; also add to navigation history
                }
            });

            contextMenuManager.add(new Action("Open in New Canvas") {
                @Override
                public void run() {
                    Activator.openInspector2(module, true);
                }
            });

            contextMenuManager.add(new Action("Add to Canvas") {
                @Override
                public void run() {
                    Activator.openInspector2(module, false);
                }
            });
        }
    }

}
