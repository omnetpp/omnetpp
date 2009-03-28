package org.omnetpp.runtimeenv;

import org.eclipse.core.runtime.ISafeRunnable;
import org.eclipse.core.runtime.ListenerList;
import org.eclipse.core.runtime.SafeRunner;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.experimental.simkernel.swig.cSimpleModule;
import org.omnetpp.experimental.simkernel.swig.cSimulation;

public class SimulationManager {
    protected ListenerList listeners = new ListenerList();
    protected boolean stopRequested = false;

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
