package org.omnetpp.runtimeenv.editors;

import java.util.HashMap;
import java.util.Map;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.MouseEvent;
import org.eclipse.draw2d.MouseListener;
import org.eclipse.draw2d.geometry.Rectangle;
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

/**
 * 
 * @author Andras
 */
public class GraphicalModulePart {
    //may add cSimulation and SimulationManager as well
    protected CompoundModuleFigure moduleFigure;
    protected int moduleID;
    protected Map<Integer,SubmoduleFigure> submodules = new HashMap<Integer,SubmoduleFigure>();
    protected ISimulationListener simulationListener;
    protected MouseListener mouseListener;
    
    /**
     * Constructor.
     * @param parentFigure typically the root figure on the canvas
     * @param moduleID id of the compound module to be displayed
     */
    public GraphicalModulePart(IFigure parentFigure, int moduleID) {
        this.moduleID = moduleID;
        cModule module = cSimulation.getActiveSimulation().getModule(moduleID);

        moduleFigure = new CompoundModuleFigure();
        parentFigure.add(moduleFigure);
        parentFigure.setConstraint(moduleFigure, new Rectangle(0,0,500,500));  //XXX

        moduleFigure.setDisplayString(module.getDisplayString());
        
        for (cModule_SubmoduleIterator it = new cModule_SubmoduleIterator(module); !it.end(); it.next()) {
            cModule submodule = it.get();
            SubmoduleFigure submoduleFigure = new SubmoduleFigure();
            submodules.put(submodule.getId(), submoduleFigure);
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

        moduleFigure.addMouseListener(mouseListener = new MouseListener() {
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
    
    protected void update() {
        //XXX
        //   moduleFigure.invalidate();
        //   moduleFigure.invalidateTree();
        //   System.out.println(cSimulation.getActiveSimulation().getSystemModule().getSubmodule("server").getDisplayString().str());
    }

    protected void handleMousePressed(MouseEvent me) {
        // TODO Auto-generated method stub
        IFigure target = moduleFigure.findFigureAt(me.x, me.y);
        System.out.println(target);
    }

    protected void handleMouseDoubleClick(MouseEvent me) {
        // TODO Auto-generated method stub
    }
    
    protected void handleMouseReleased(MouseEvent me) {
        // TODO Auto-generated method stub
    }
}
