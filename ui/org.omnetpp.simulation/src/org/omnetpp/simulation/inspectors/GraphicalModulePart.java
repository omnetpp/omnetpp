package org.omnetpp.simulation.inspectors;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.MouseEvent;
import org.eclipse.draw2d.MouseListener;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Point;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.figures.ConnectionFigure;
import org.omnetpp.figures.SubmoduleFigure;
import org.omnetpp.figures.anchors.CompoundModuleGateAnchor;
import org.omnetpp.figures.anchors.GateAnchor;
import org.omnetpp.figures.layout.CompoundModuleLayout;
import org.omnetpp.ned.model.DisplayString;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.figures.CompoundModuleFigureEx;
import org.omnetpp.simulation.figures.SubmoduleFigureEx;
import org.omnetpp.simulation.model.cComponent;
import org.omnetpp.simulation.model.cGate;
import org.omnetpp.simulation.model.cModule;
import org.omnetpp.simulation.model.cObject;

/**
 * An inspector that displays a compound module graphically.
 * @author Andras
 */
//TODO ConnectionFigure must be fixed too!
public class GraphicalModulePart extends AbstractInspectorPart {
	protected static final DisplayString EMPTY_DISPLAYSTRING = new DisplayString(""); 
	
    protected Map<cModule,SubmoduleFigureEx> submodules = new HashMap<cModule,SubmoduleFigureEx>();
    protected Map<cGate,ConnectionFigure> connections = new HashMap<cGate, ConnectionFigure>();
    protected float canvasScale = 1.0f;  //TODO properly do it
    protected int imageSizePercentage = 100; // controls submodule icons; 100 means 1:1
    protected boolean showNameLabels = true;
    protected boolean showArrowHeads = true;

    /**
     * Constructor.
     */
    public GraphicalModulePart(IInspectorContainer parent, cModule module) {
    	super(parent, module);
    	
        // mouse handling
        figure.addMouseListener(new MouseListener() {
            //@Override
            public void mouseDoubleClicked(MouseEvent me) {
                handleMouseDoubleClick(me);
            }

			//@Override
			public void mousePressed(MouseEvent me) {
                handleMousePressed(me);
			}

			//@Override
			public void mouseReleased(MouseEvent me) {
                handleMouseReleased(me);
			}
        });
    }

    @Override
    protected IInspectorFigure createFigure() {
        CompoundModuleFigureEx figure = new CompoundModuleFigureEx();
        return figure;
    }
    
	//@Override
	public boolean isMaximizable() {
		return false;
	}

    public float getCanvasScale() {
		return canvasScale;
	}

	public void setCanvasScale(float canvasScale) {
		Assert.isTrue(canvasScale > 0);
		this.canvasScale = canvasScale;
		refresh();
	}

	public int getImageSizePercentage() {
		return imageSizePercentage;
	}

	public void setImageSizePercentage(int imageSizePercentage) {
		Assert.isTrue(imageSizePercentage > 0);
		this.imageSizePercentage = imageSizePercentage;
		refresh();
	}

	public boolean isShowNameLabels() {
		return showNameLabels;
	}

	public void setShowNameLabels(boolean showNameLabels) {
		this.showNameLabels = showNameLabels;
		refresh();
	}

	public boolean isShowArrowHeads() {
		return showArrowHeads;
	}

	public void setShowArrowHeads(boolean showArrowHeads) {
		this.showArrowHeads = showArrowHeads;
		refresh();
	}

	@Override
	public CompoundModuleFigureEx getFigure() {
	    return (CompoundModuleFigureEx) super.getFigure();
	}

    public SubmoduleFigure getSubmoduleFigure(cModule submodule) {
        return submodules.get(submodule);
    }

    public ConnectionFigure getConnectionFigure(cGate srcGate) {
        return connections.get(srcGate);
    }
    
    @Override
    public void refresh() {
        super.refresh();
        if (!isDisposed()) {
            cModule module = (cModule) object;
            if (module.isFilledIn())
                module.safeLoad();
            try {
                Collection<? extends cObject> submodsList = Arrays.asList(module.getSubmodules());
                module.getController().loadUnfilledObjects(submodsList);
            }
            catch (IOException e) {  //FIXME ez itt valahogy nem tul jo!
                SimulationPlugin.logError(e);   
            }

//            CompoundModuleFigureEx moduleFigure = (CompoundModuleFigureEx)figure;
//            moduleFigure.setDisplayString(getDisplayStringFrom(module)); //XXX needed? the same is in refreshVisuals()!!!

            
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
            if (submodule.isDisposed() || submodule.getParentModule() != object) {
                if (toBeRemoved == null)
                    toBeRemoved = new ArrayList<cModule>();
                toBeRemoved.add(submodule);
            }
        }

        // find submodules that not yet have a figure
        for (cModule submodule : ((cModule)object).getSubmodules()) {
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
                submoduleFigure.setPinVisible(false);
                moduleFigure.getSubmoduleLayer().add(submoduleFigure);
                submoduleFigure.setRangeFigureLayer(((CompoundModuleFigureEx)figure).getInternalModuleFigure().getBackgroundDecorationLayer());
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
        	if (gate.isDisposed() || gate.getNextGate() == null) {
                if (toBeRemoved == null)
                    toBeRemoved = new ArrayList<cGate>();
                toBeRemoved.add(gate);
            }
        }

        // find connections that not yet have a figure.
        // loop through all submodules and enclosing module, and find their connections
        boolean atParent = false;
        cModule parentModule = (cModule)object;
        cModule[] submodules2 = parentModule.getSubmodules();
		for (int i = 0; i <= submodules2.length; i++) {
			cModule module = i < submodules2.length ? submodules2[i] : parentModule;
			if (module==parentModule)
				atParent = true;
			try {
                module.getController().loadUnfilledObjects(module.getGates());
            }
            catch (IOException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
            for (cGate gate : module.getGates()) {
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
            	GateAnchor sourceAnchor = (gate.getOwnerModule().equals(parentModule) ?
            			new CompoundModuleGateAnchor(moduleFigure.getInternalModuleFigure()) :
            				new GateAnchor(submodules.get(gate.getOwnerModule())));
            	GateAnchor targetAnchor = (targetGate.getOwnerModule().equals(parentModule) ?
            			new CompoundModuleGateAnchor(moduleFigure.getInternalModuleFigure()) :
            				new GateAnchor(submodules.get(targetGate.getOwnerModule())));
            	connectionFigure.setSourceAnchor(sourceAnchor);
            	connectionFigure.setTargetAnchor(targetAnchor);
            	moduleFigure.getConnectionLayer().add(connectionFigure);
                connections.put(gate, connectionFigure);
            }
        }
    }

    protected void refreshVisuals() {
    	CompoundModuleFigureEx moduleFigure = (CompoundModuleFigureEx)figure;
        cModule parentModule = (cModule) object;
        moduleFigure.setDisplayString(getDisplayStringFrom(parentModule));

        float scale = canvasScale * moduleFigure.getInternalModuleFigure().getScale(); //FIXME the compound module's scale should be set too

    	// refresh submodules
    	for (cModule submodule : submodules.keySet()) {
    		SubmoduleFigureEx submoduleFigure = submodules.get(submodule);
    		submoduleFigure.setDisplayString(scale, getDisplayStringFrom(submodule));
    		submoduleFigure.setName(showNameLabels ? submodule.getFullName() : null);
    		submoduleFigure.setSubmoduleVectorIndex(submodule.getName(), submodule.getVectorSize(), submodule.getIndex());
    		submoduleFigure.setImageSizePercentage(imageSizePercentage);
    	}

    	// refresh connections
    	//FIXME this is very slow!!!! even when there are no display strings at all. ConnectionFigure is not caching the displaystring?
    	for (cGate gate : connections.keySet()) {
    		ConnectionFigure connectionFigure = connections.get(gate);
    		connectionFigure.setDisplayString(getDisplayStringFrom(gate.getChannel())); // note: gate.getDisplayString() would implicitly create a cIdealChannel!
        	connectionFigure.setArrowHeadEnabled(showArrowHeads);
    	}

    	//Debug.debug = true;
        //FigureUtils.debugPrintRootFigureHierarchy(figure);
    }

    protected IDisplayString getDisplayStringFrom(cComponent component) {
    	if (component == null)
    	    return EMPTY_DISPLAYSTRING;
    	if (!component.isFilledIn())
    	    component.safeLoad();  //XXX maybe not here...
    	return component.getDisplayString();
    }
    
    protected cModule findSubmoduleFor(SubmoduleFigureEx submoduleFigure) {
        for (cModule submodule : submodules.keySet())
            if (submodules.get(submodule) == submoduleFigure)
                return submodule;
        return null;
    }

    protected void handleMouseDoubleClick(MouseEvent me) {
    	SubmoduleFigureEx submoduleFigure = findSubmoduleAt(me.x,me.y);
		System.out.println("clicked submodule: " + submoduleFigure);
		if (submoduleFigure != null) {
		    cModule submodule = findSubmoduleFor(submoduleFigure);
		    if (submodule != null)
		        inspectorContainer.inspect(submodule);
		}
    }


    protected void handleMousePressed(MouseEvent me) {
    	SubmoduleFigureEx submoduleFigure = findSubmoduleAt(me.x,me.y);
		System.out.println("clicked submodule: " + submoduleFigure);
		if (submoduleFigure == null) {
            if ((me.getState() & SWT.CONTROL) != 0)
            	inspectorContainer.toggleSelection(getObject());
            else
            	inspectorContainer.select(getObject(), true);
		}
		else {
            cModule submodule = findSubmoduleFor(submoduleFigure);
            if ((me.getState()& SWT.CONTROL) != 0)
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

    @SuppressWarnings("rawtypes")
    @Override
    public void selectionChanged(IStructuredSelection selection) {
    	super.selectionChanged(selection);

    	// update selection border around submodules
    	List list = selection.toList();
        for (cModule submodule : submodules.keySet()) {
            SubmoduleFigureEx submoduleFigure = submodules.get(submodule);
            submoduleFigure.setSelectionBorderShown(list.contains(submodule));
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

		contextMenuManager.add(new Action("Zoom in") {
		    @Override
		    public void run() {
		    	canvasScale *= 1.5;
		    	if (Math.abs(canvasScale-1.0) < 0.01)
		    		canvasScale = 1.0f; // prevent accumulation of rounding errors
		    	refresh();
		    }
		});
		contextMenuManager.add(new Action("Zoom out") {
		    @Override
		    public void run() {
		    	canvasScale /= 1.5;
		    	if (Math.abs(canvasScale-1.0) < 0.01)
		    		canvasScale = 1.0f; // prevent accumulation of rounding errors
		    	refresh();
		    }
		});

		contextMenuManager.add(new Action("Enlarge icons") {
		    @Override
		    public void run() {
		    	if (imageSizePercentage < 500) {
		    		imageSizePercentage *= 1.2;
		    		if (Math.abs(imageSizePercentage-100) < 15)
		    			imageSizePercentage = 100; // prevent accumulation of rounding errors
		    		refresh();
		    	}
		    }
		});
		contextMenuManager.add(new Action("Reduce icons") {
		    @Override
		    public void run() {
		    	if (imageSizePercentage > 10) {
		    		imageSizePercentage /= 1.2;
		    		if (Math.abs(imageSizePercentage-100) < 15)
		    			imageSizePercentage = 100; // prevent accumulation of rounding errors
		    		refresh();
		    	}
		    }
		});

		contextMenuManager.add(new Separator());

		contextMenuManager.add(new Action("Toggle name labels") {
		    @Override
		    public void run() {
		    	showNameLabels = !showNameLabels;
		    	refresh();
		    }
		});

		contextMenuManager.add(new Action("Toggle arrowheads") {
		    @Override
		    public void run() {
		    	showArrowHeads = !showArrowHeads;
		    	refresh();
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
		final cModule module = findSubmoduleFor(submoduleFigure);

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
                //MessageDialog.openInformation(null,  "Open", "TODO: open module " + module.getFullPath());
		        inspectorContainer.inspect(module);  //TODO on new canvas!
		    }
		});

		contextMenuManager.add(new Action("Add to Canvas") {
		    @Override
		    public void run() {
		        //MessageDialog.openInformation(null,  "Open", "TODO: open module " + module.getFullPath());
		        inspectorContainer.inspect(module); //TODO on same canvas!
		    }
		});
	}

}
