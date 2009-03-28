package org.omnetpp.runtimeenv.editors;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.MouseEvent;
import org.eclipse.draw2d.MouseListener;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.part.EditorPart;
import org.omnetpp.experimental.simkernel.swig.cModule;
import org.omnetpp.experimental.simkernel.swig.cModule_SubmoduleIterator;
import org.omnetpp.experimental.simkernel.swig.cSimulation;
import org.omnetpp.figures.CompoundModuleFigure;
import org.omnetpp.figures.ConnectionFigure;
import org.omnetpp.figures.SubmoduleFigure;
import org.omnetpp.figures.anchors.CompoundModuleGateAnchor;
import org.omnetpp.figures.anchors.GateAnchor;
import org.omnetpp.figures.layout.SubmoduleConstraint;
import org.omnetpp.runtimeenv.Activator;
import org.omnetpp.runtimeenv.ISimulationListener;
import org.omnetpp.runtimeenv.animation.widgets.AnimationCanvas;

//TODO canvas selection mechanism
//TODO submodule context menu: "Open (go into)", "Add to this canvas", "Open in new network view" 
public class ModelCanvas extends EditorPart {
    public static final String EDITOR_ID = "org.omnetpp.runtimeenv.editors.ModelCanvas";

    @Override
    public void init(IEditorSite site, IEditorInput input) throws PartInitException {
        if (!(input instanceof ModuleIDEditorInput))
            throw new PartInitException("Invalid input: it must be a module in the simulation");
        setSite(site);
        setInput(input);
        setPartName(input.getName());
    }

    @Override
    public void createPartControl(Composite parent) {
//        parent.setLayout(new FormLayout());
//        parent.setBackground(new Color(null, 228, 228, 228));
//        
//        createCoolbar();
//        createSimulationToolbar();
//        createNavigationToolbar();
//        createTimeGauges();
//        createSpeedSlider();
//
//        coolBar.setWrapIndices(new int[] {2,3});
        
        AnimationCanvas canvas = new AnimationCanvas(parent, SWT.DOUBLE_BUFFERED);

        int moduleID = ((ModuleIDEditorInput)getEditorInput()).getModuleID();
        cModule module = cSimulation.getActiveSimulation().getModule(moduleID);
        final CompoundModuleFigure moduleFigure = new CompoundModuleFigure();
        canvas.getRootFigure().add(moduleFigure);
        moduleFigure.setDisplayString(module.getDisplayString());
        canvas.getRootFigure().setConstraint(moduleFigure, new Rectangle(0,0,500,500));
        
        for (cModule_SubmoduleIterator it = new cModule_SubmoduleIterator(module); !it.end(); it.next()) {
            cModule submodule = it.get();
            SubmoduleFigure submoduleFigure = new SubmoduleFigure();
            submoduleFigure.setDisplayString(submodule.getDisplayString());
            submoduleFigure.setName(submodule.getFullName());
            submoduleFigure.setPinDecoration(false);
            moduleFigure.getSubmoduleLayer().add(submoduleFigure);
            
            SubmoduleConstraint constraint = new SubmoduleConstraint();
            constraint.setLocation(submoduleFigure.getPreferredLocation());
            constraint.setSize(submoduleFigure.getPreferredSize());
            Assert.isTrue(constraint.height != -1 && constraint.width != -1);
            moduleFigure.getSubmoduleLayer().setConstraint(submoduleFigure, constraint);
            
            //TODO just testing
            ConnectionFigure connectionFigure = new ConnectionFigure();
            connectionFigure.setArrowHeadEnabled(true);
            connectionFigure.setSourceAnchor(new GateAnchor(submoduleFigure));
            connectionFigure.setTargetAnchor(new CompoundModuleGateAnchor(moduleFigure));
            moduleFigure.getConnectionLayer().add(connectionFigure);
        }

        moduleFigure.addMouseListener(new MouseListener() {
            @Override
            public void mouseDoubleClicked(MouseEvent me) {}

            @Override
            public void mousePressed(MouseEvent me) {
                IFigure target = moduleFigure.findFigureAt(me.x, me.y);
                System.out.println(target);
            }

            @Override
            public void mouseReleased(MouseEvent me) {}
        });
        
        //FIXME remove this listener when editor gets closed
        Activator.getSimulationManager().addChangeListener(new ISimulationListener() {
            @Override
            public void changed() {
                //FIXME update display!
            }
        });

    }

    @Override
    public void setFocus() {
        // TODO Auto-generated method stub
        
    }

    @Override
    public void doSave(IProgressMonitor monitor) {
        // Nothing
    }

    @Override
    public void doSaveAs() {
        // Nothing
    }

    @Override
    public boolean isDirty() {
        return false;
    }

    @Override
    public boolean isSaveAsAllowed() {
        return false;
    }
}
