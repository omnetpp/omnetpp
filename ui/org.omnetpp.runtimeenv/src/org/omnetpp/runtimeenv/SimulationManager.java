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
import org.omnetpp.experimental.simkernel.swig.EmptyConfig;
import org.omnetpp.experimental.simkernel.swig.ExecuteOnStartup;
import org.omnetpp.experimental.simkernel.swig.LogBuffer;
import org.omnetpp.experimental.simkernel.swig.NotSoMinimalEnv;
import org.omnetpp.experimental.simkernel.swig.SimTime;
import org.omnetpp.experimental.simkernel.swig.cConfiguration;
import org.omnetpp.experimental.simkernel.swig.cModuleType;
import org.omnetpp.experimental.simkernel.swig.cSimpleModule;
import org.omnetpp.experimental.simkernel.swig.cSimulation;
import org.omnetpp.experimental.simkernel.swig.cStaticFlag;
import org.omnetpp.runtimeenv.editors.ModelCanvas;
import org.omnetpp.runtimeenv.editors.ModuleIDEditorInput;

public class SimulationManager {
    protected ListenerList listeners = new ListenerList();
    protected boolean stopRequested = false;
    protected NotSoMinimalEnv env;

    public SimulationManager() {
        ImageFactory.setImageDirs(new String[]{"C:\\home\\omnetpp40\\omnetpp\\images"}); //FIXME just temporary
        
        // library initializations
        cStaticFlag.set(true);
        ExecuteOnStartup.executeAll();
        SimTime.setScaleExp(-12);
        
        // set up an active simulation object
        cConfiguration config = new EmptyConfig();
        env = new NotSoMinimalEnv(0, null, config);
        config.disown();
        cSimulation simulation = new cSimulation("simulation", env);
        env.disown();
        cSimulation.setActiveSimulation(simulation);
        simulation.disown();
        System.out.println(simulation.getName());

        try {
            cSimulation.loadNedSourceFolder("c:/home/omnetpp40/omnetpp/samples/aloha"); //XXX
            cSimulation.doneLoadingNedFiles();
            cModuleType networkType = cModuleType.find("Aloha_tmp");
            if (networkType == null)
                throw new RuntimeException("network not found");
            simulation.setupNetwork(networkType);
            simulation.startRun();
            
            Display.getDefault().asyncExec(new Runnable() {
                @Override
                public void run() {
                    IWorkbenchWindow workbenchWindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
                    if (workbenchWindow == null) {
                        Display.getDefault().asyncExec(this); //FIXME this is a dirty hack
                        return;
                    }

                    IWorkbenchPage page = workbenchWindow.getActivePage();
                    int moduleID = cSimulation.getActiveSimulation().getSystemModule().getId();
                    try {
                        page.openEditor(new ModuleIDEditorInput(moduleID), ModelCanvas.EDITOR_ID);
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
        // delete the active simulation object
        cSimulation simulation = cSimulation.getActiveSimulation();
        cSimulation.setActiveSimulation(null);
        if (simulation != null)
            simulation.delete();
        cStaticFlag.set(false);
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
        return env.getLogBuffer();
    }
    
    public void rebuildNetwork() {
        // TODO Auto-generated method stub
    }

    public void step() {
        // TODO Auto-generated method stub
    }

    public void slowRun() {
        // TODO Auto-generated method stub
    }

    public void run() {
        cSimulation sim = cSimulation.getActiveSimulation();
        stopRequested = false;
        try {
            while (sim.getSimTime() < 100000 && !stopRequested) {
                cSimpleModule mod = sim.selectNextModule();
                if (mod == null)
                    break;  //XXX
                sim.doOneEvent(mod);
                
                updateUI();
                while (Display.getCurrent().readAndDispatch());
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
