package org.omnetpp.runtimeenv;

import org.eclipse.core.runtime.ISafeRunnable;
import org.eclipse.core.runtime.ListenerList;
import org.eclipse.core.runtime.SafeRunner;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.experimental.simkernel.DummyEnvirCallback;
import org.omnetpp.experimental.simkernel.swig.Javaenv;
import org.omnetpp.experimental.simkernel.swig.LogBuffer;
import org.omnetpp.experimental.simkernel.swig.cEnvir;
import org.omnetpp.experimental.simkernel.swig.cModule;
import org.omnetpp.experimental.simkernel.swig.cModuleType;
import org.omnetpp.experimental.simkernel.swig.cSimpleModule;
import org.omnetpp.experimental.simkernel.swig.cSimulation;
import org.omnetpp.runtimeenv.editors.BlankCanvasEditorInput;
import org.omnetpp.runtimeenv.editors.ModelCanvas;

/**
 * 
 * @author Andras
 */
public class SimulationManager {
    protected ListenerList listeners = new ListenerList();
    protected boolean stopRequested = false;

    public SimulationManager() {
        ImageFactory.initialize(new String[]{"C:\\home\\omnetpp40\\omnetpp\\images"}); //FIXME just temporary; maybe into Application?

        Javaenv env = Javaenv.cast(cSimulation.getActiveEnvir());
        env.setJCallback(null, new DummyEnvirCallback());
        
        try {
        	//env.getConfigEx().activateConfig("Terminal",0); //XXX
        	env.getConfigEx().activateConfig("Net2",0); //XXX
        	env.readPerRunOptions();
        	
            String networkName = env.getConfigEx().getConfigValue("network");
			cModuleType networkType = cModuleType.find(networkName);
            if (networkType == null)
                throw new RuntimeException("network not found");

            cSimulation simulation = cSimulation.getActiveSimulation();
            simulation.setupNetwork(networkType);
            env.startRun();
            
            Display.getDefault().asyncExec(new Runnable() {
                @Override
                public void run() {
                    IWorkbenchWindow workbenchWindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
                    if (workbenchWindow == null) {
                        Display.getDefault().asyncExec(this); //FIXME this is a dirty hack
                        return;
                    }

                    IWorkbenchPage page = workbenchWindow.getActivePage();
                	cSimulation simulation = cSimulation.getActiveSimulation();
                    cModule module = simulation.getSystemModule();
                    try {
                        page.openEditor(new BlankCanvasEditorInput(""), ModelCanvas.EDITOR_ID);
                        Activator.openInspector2(module, false);
                    }
                    catch (PartInitException e) {
                        e.printStackTrace(); //XXX
                    }
                }
            });
        } 
        catch (Exception e) {
            e.printStackTrace();
        }
    }
    
    public void dispose() {
    	//FIXME do all such stuff in Application?
        // delete the active simulation object
//    	Simkernel.shutdownUserInterface();
/*
    	cSimulation simulation = cSimulation.getActiveSimulation();
        cSimulation.setActiveSimulation(null);
        if (simulation != null)
            simulation.delete();
*/            
//        cStaticFlag.set(false);
    }
    
    public void addChangeListener(ISimulationListener listener) {
        listeners.add(listener);
    }

    public void removeChangeListener(ISimulationListener listener) {
        listeners.remove(listener);
    }

    protected void updateUI() {
        for (final Object listener : listeners.getListeners()) {
            SafeRunner.run(new ISafeRunnable() {
                public void handleException(Throwable e) {
                    // exception logged in SafeRunner#run
                }
                public void run() throws Exception {
                    ((ISimulationListener)listener).changed();
                }
            });
        }
    }

    public LogBuffer getLogBuffer() {
        cEnvir env = cSimulation.getActiveEnvir();
        return Javaenv.cast(env).getLogBuffer();
    }
    
    public void rebuildNetwork() {
        // TODO Auto-generated method stub
    }

    public void step() {
        cSimulation sim = cSimulation.getActiveSimulation();
        try {
            cSimpleModule mod = sim.selectNextModule();
            if (mod == null)
                System.out.println("no more events"); //XXX
            else {
                sim.doOneEvent(mod);

                updateUI();
            }
        }
//XXX
//        catch (cTerminationException& e) {
//            printf("Finished: %s\n", e.what());
//        }
        catch (RuntimeException e) {
            e.printStackTrace();
        }
    }

    public void slowRun() {
        // TODO Auto-generated method stub
    }

    public void run() {
        cSimulation sim = cSimulation.getActiveSimulation();
        stopRequested = false;
        long tStart = System.currentTimeMillis();
        long numEvents = 0;
        try {
            while (sim.getSimTime() < 100000 && !stopRequested) {
                cSimpleModule mod = sim.selectNextModule();
                if (mod == null)
                    break;  //XXX
                sim.doOneEvent(mod);

                updateUI();
                while (Display.getCurrent().readAndDispatch());
                
                //XXX preliminary speed metering, for debugging purposes
                numEvents++;
                if ((numEvents&16) == 0 && System.currentTimeMillis()-tStart > 1000) {
                    long t = System.currentTimeMillis();
                    long dt = t - tStart;
                    System.out.println("events/sec: " + (numEvents * 1000.0 / dt));
                    numEvents = 0;
                    tStart = t;
                }
            }
        }
//XXX
//        catch (cTerminationException& e) {
//            printf("Finished: %s\n", e.what());
//        }
        catch (RuntimeException e) {
            e.printStackTrace();
        }
    }

    public void fastRun() {
        // TODO Auto-generated method stub
    }

    public void expressRun() {
        // TODO Auto-generated method stub
    }

    public void runUntil() {
        // TODO Auto-generated method stub
    }

    public void stop() {
        stopRequested = true;
    }

    public void callFinish() {
        // TODO Auto-generated method stub
    }

}
