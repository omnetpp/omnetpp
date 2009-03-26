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
import org.omnetpp.experimental.simkernel.swig.cSimulation;
import org.omnetpp.figures.CompoundModuleFigure;
import org.omnetpp.figures.SubmoduleFigure;
import org.omnetpp.figures.layout.SubmoduleConstraint;
import org.omnetpp.runtimeenv.animation.widgets.AnimationCanvas;

//FIXME we require the GEF plugin only because SubmoduleFigure implements HandleBounds!!! Remove that! 
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
        
        cModule submodule = module.getSubmodule("server");
        SubmoduleFigure submoduleFigure = new SubmoduleFigure();
        submoduleFigure.setDisplayString(submodule.getDisplayString());
        submoduleFigure.setName(submodule.getFullName());
        submoduleFigure.setPinDecoration(false);
        moduleFigure.addSubmoduleFigure(submoduleFigure);

        SubmoduleConstraint constraint = new SubmoduleConstraint();
        constraint.setLocation(submoduleFigure.getPreferredLocation());
        constraint.setSize(submoduleFigure.getPreferredSize());
        Assert.isTrue(constraint.height != -1 && constraint.width != -1);
        moduleFigure.getSubmoduleContainer().setConstraint(submoduleFigure, constraint);

        for (int i=0; i<5; i++) {            
            submodule = module.getSubmodule("host", i);
            submoduleFigure = new SubmoduleFigure();
            submoduleFigure.setDisplayString(submodule.getDisplayString());
            submoduleFigure.setName(submodule.getFullName());
            submoduleFigure.setPinDecoration(false);
            moduleFigure.addSubmoduleFigure(submoduleFigure);

            constraint = new SubmoduleConstraint();
            constraint.setLocation(submoduleFigure.getPreferredLocation());
            constraint.setSize(submoduleFigure.getPreferredSize());
            Assert.isTrue(constraint.height != -1 && constraint.width != -1);
            moduleFigure.getSubmoduleContainer().setConstraint(submoduleFigure, constraint);
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
