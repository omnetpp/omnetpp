package org.omnetpp.runtimeenv.editors;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.InputEvent;
import org.eclipse.draw2d.MouseEvent;
import org.eclipse.draw2d.MouseListener;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.swt.graphics.Point;
import org.omnetpp.experimental.simkernel.swig.cDisplayString;
import org.omnetpp.experimental.simkernel.swig.cGate;
import org.omnetpp.experimental.simkernel.swig.cModule;
import org.omnetpp.experimental.simkernel.swig.cModule_GateIterator;
import org.omnetpp.experimental.simkernel.swig.cModule_SubmoduleIterator;
import org.omnetpp.experimental.simkernel.swig.cSimulation;
import org.omnetpp.figures.ConnectionFigure;
import org.omnetpp.figures.anchors.CompoundModuleGateAnchor;
import org.omnetpp.figures.anchors.GateAnchor;
import org.omnetpp.figures.layout.CompoundModuleLayout;
import org.omnetpp.runtimeenv.Activator;
import org.omnetpp.runtimeenv.figures.CompoundModuleFigureEx;
import org.omnetpp.runtimeenv.figures.SubmoduleFigureEx;

/**
 * An inspector that displays a compound module graphically.
 * @author Andras
 */
//TODO ConnectionFigure must be fixed too!
public class GraphicalModulePart extends InspectorPart {
    protected Map<cModule,SubmoduleFigureEx> submodules = new HashMap<cModule,SubmoduleFigureEx>();
    protected Map<cGate,ConnectionFigure> connections = new HashMap<cGate, ConnectionFigure>();
    
    /**
     * Constructor.
     */
    public GraphicalModulePart(cModule module) {
    	super(module);

        figure = new CompoundModuleFigureEx();
        figure.setInspectorPart(this);

        ((CompoundModuleFigureEx)figure).setDisplayString(module.getDisplayString());

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
	public void refresh() {
		super.refresh();
    	if (!isDisposed()) { 
    		refreshChildren();
    		refreshConnections();
    		refreshVisuals();
		}
    }

    protected void refreshChildren() {
        //TODO only call this function if there were any moduleCreated/moduleDeleted notifications from the simkernel
    	CompoundModuleFigureEx moduleFigure = (CompoundModuleFigureEx)figure;
        List<cModule> toBeRemoved = null;
        List<cModule> toBeAdded = null;
        
        // find submodule figures whose module has been deleted
        for (cModule submodule : submodules.keySet()) {
            if (submodule.isZombie() || !submodule.getParentModule().equals(object)) {
                if (toBeRemoved == null)
                    toBeRemoved = new ArrayList<cModule>();
                toBeRemoved.add(submodule);
            }
        }

        // find submodules that not yet have a figure
        for (cModule_SubmoduleIterator it = new cModule_SubmoduleIterator(cModule.cast(object)); !it.end(); it.next()) {
            cModule submodule = it.get();
            if (!submodules.containsKey(submodule)) {
                if (toBeAdded == null)
                    toBeAdded = new ArrayList<cModule>();
                toBeAdded.add(submodule);
            }
        }

        // do the removals and additions
        if (toBeRemoved != null) {
            for (cModule submodule : toBeRemoved) {
                moduleFigure.getSubmoduleLayer().remove(submodules.get(submodule));
                submodules.remove(submodule);
            }
        }
        if (toBeAdded != null) {
            for (cModule submodule : toBeAdded) {
                // create figure
                SubmoduleFigureEx submoduleFigure = new SubmoduleFigureEx();
                submoduleFigure.setFont(getContainer().getControl().getFont()); // to speed up figure's getFont()
                submoduleFigure.setModuleID(submodule.getId());
                submoduleFigure.setPinVisible(false);
                submoduleFigure.setName(submodule.getFullName());
                moduleFigure.getSubmoduleLayer().add(submoduleFigure);
                submodules.put(submodule, submoduleFigure);
            }
        }
    }

    protected void refreshConnections() {
    	CompoundModuleFigureEx moduleFigure = (CompoundModuleFigureEx)figure;
        ArrayList<cGate> toBeRemoved = null;
        ArrayList<cGate> toBeAdded = null;
    	
        // find connection figures whose connection has been deleted
        for (cGate gate : connections.keySet()) {
        	if (gate.isZombie() || gate.getNextGate() == null) {
                if (toBeRemoved == null)
                    toBeRemoved = new ArrayList<cGate>();
                toBeRemoved.add(gate);
            }
        }

        // find connections that not yet have a figure.
        // loop through all submodules and enclosing module, and find their connections
        boolean atParent = false;
        cModule parentModule = cModule.cast(object);
		for (cModule_SubmoduleIterator it = new cModule_SubmoduleIterator(parentModule); !atParent; it.next()) {
			cModule module = !it.end() ? it.get() : parentModule;
			if (module==parentModule) 
				atParent = true; 
            for (cModule_GateIterator gi = new cModule_GateIterator(module); !gi.end(); gi.next()) {
                cGate gate = gi.get();
                if (gate.getType()==(atParent ? cGate.Type.INPUT : cGate.Type.OUTPUT) && gate.getNextGate() != null) {
                    if (!connections.containsKey(gate)) {
                        if (toBeAdded == null)
                            toBeAdded = new ArrayList<cGate>();
                        toBeAdded.add(gate);
                    }
                }
            }
        }

        // do the removals and additions
        if (toBeRemoved != null) {
            for (cGate gate : toBeRemoved) {
                moduleFigure.getConnectionLayer().remove(connections.get(gate));
                connections.remove(gate);
            }
        }
        if (toBeAdded != null) {
            for (cGate gate : toBeAdded) {
                // create figure
            	cGate targetGate = gate.getNextGate();
            	ConnectionFigure connectionFigure = new ConnectionFigure();
            	connectionFigure.setArrowHeadEnabled(true);
            	GateAnchor sourceAnchor = (gate.getOwnerModule().equals(parentModule) ? 
            			new CompoundModuleGateAnchor(moduleFigure) : 
            				new GateAnchor(submodules.get(gate.getOwnerModule())));
            	GateAnchor targetAnchor = (targetGate.getOwnerModule().equals(parentModule) ? 
            			new CompoundModuleGateAnchor(moduleFigure) : 
            				new GateAnchor(submodules.get(targetGate.getOwnerModule())));
            	connectionFigure.setSourceAnchor(sourceAnchor);
            	connectionFigure.setTargetAnchor(targetAnchor);
            	moduleFigure.getConnectionLayer().add(connectionFigure);
                connections.put(gate, connectionFigure);
            }
        }
    }
    
    protected void refreshVisuals() {
    	float scale = 1.0f;  //FIXME from compound module display string

    	// refresh submodules
    	for (cModule submodule : submodules.keySet()) {
    		SubmoduleFigureEx submoduleFigure = submodules.get(submodule);
    		cDisplayString displayString = submodule.getDisplayString();
    		submoduleFigure.setDisplayString(scale, displayString);
    		submoduleFigure.setSubmoduleVectorIndex(submodule.getName(), submodule.getVectorSize(), submodule.getIndex());
    	}

    	// refresh connections
    	//FIXME this is very slow!!!! even when there are no display strings at all. ConnectionFigure is not caching the displaystring?
    	for (cGate gate : connections.keySet()) {
    		ConnectionFigure connectionFigure = connections.get(gate);
    		cDisplayString displayString = gate.getDisplayString();
    		connectionFigure.setDisplayString(displayString);
    	}
    	
    	//Debug.debug = true;
        //FigureUtils.debugPrintRootFigureHierarchy(figure);
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
            	inspectorContainer.toggleSelection(getObject());
            else
            	inspectorContainer.select(getObject(), true);
		}
		else {
			int submoduleID = submoduleFigure.getModuleID();
            cModule submodule = cSimulation.getActiveSimulation().getModule(submoduleID);
            if ((me.getState()& InputEvent.CONTROL) != 0)
            	inspectorContainer.toggleSelection(submodule);
            else
            	inspectorContainer.select(submodule, true);
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
        if (submoduleFigure == null)
        	populateBackgroundContextMenu(contextMenuManager, p);
        else 
        	populateSubmoduleContextMenu(contextMenuManager, submoduleFigure, p);
    }

    protected void populateBackgroundContextMenu(MenuManager contextMenuManager, Point p) {
		//XXX factor out actions

		contextMenuManager.add(new Action("Zoom in") {
		    @Override
		    public void run() {
		        //TODO see ZoomManager.zoomIn(); relies on ScalableFigure.setScale()
		    }
		});
		contextMenuManager.add(new Action("Zoom out") {
		    @Override
		    public void run() {
		        //TODO see ZoomManager.zoomOut(); relies on ScalableFigure.setScale()
		    }
		});

		contextMenuManager.add(new Separator());

		contextMenuManager.add(new Action("Re-layout") {
			@Override
			public void run() {
		    	CompoundModuleFigureEx moduleFigure = (CompoundModuleFigureEx)figure;
				CompoundModuleLayout layouter = moduleFigure.getSubmoduleLayouter();
				layouter.requestFullLayout();
				//layouter.setSeed(layouter.getSeed()+1);  -- not needed
				moduleFigure.getSubmoduleLayer().revalidate();
			}
		});

		contextMenuManager.add(new Separator());
		
		contextMenuManager.add(new Action("Go to parent") {
		    @Override
		    public void run() {
		        //Close this inspector; open new inspector at these coordinates; also add to navigation history
		    }
		});

		contextMenuManager.add(new Separator());
		
		contextMenuManager.add(new Action("Close") {
		    @Override
		    public void run() {
		        getContainer().close(GraphicalModulePart.this);
		    }
		});
	}

	protected void populateSubmoduleContextMenu(MenuManager contextMenuManager, SubmoduleFigureEx submoduleFigure, Point p) {
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
