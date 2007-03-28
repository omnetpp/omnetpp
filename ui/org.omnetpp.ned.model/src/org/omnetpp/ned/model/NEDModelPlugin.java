package org.omnetpp.ned.model;

import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.omnetpp.ned.model.ex.NEDElementFactoryEx;
import org.omnetpp.ned.model.notification.INEDChangeListener;
import org.omnetpp.ned.model.notification.NEDChangeListenerList;
import org.omnetpp.ned.model.notification.NEDModelEvent;
import org.osgi.framework.BundleContext;

/**
 * The main plugin class to be used in the desktop.
 */
public class NEDModelPlugin extends AbstractUIPlugin 
                                implements INEDChangeListener {
    public static String PLUGIN_ID; 
    private transient NEDChangeListenerList nedModelChangeListenerList = null;
    protected long lastEventSerial;
    
	//The shared instance.
	private static NEDModelPlugin plugin;

	/**
	 * The constructor.
	 */
	public NEDModelPlugin() {
	    plugin = this;

        // initialize the model factory to use this factory as default
        // this is called when the plugin is activated
	    NEDElementFactoryEx.setInstance(new NEDElementFactoryEx());

	}

	/**
	 * This method is called upon plug-in activation
	 */
	public void start(BundleContext context) throws Exception {
		super.start(context);
        PLUGIN_ID = getBundle().getSymbolicName();
	}

	/**
	 * This method is called when the plug-in is stopped
	 */
	public void stop(BundleContext context) throws Exception {
		super.stop(context);
		plugin = null;
	}

	/**
	 * Returns the shared instance.
	 *
	 * @return the shared instance.
	 */
	public static NEDModelPlugin getDefault() {
		return plugin;
	}
    
    public static void log(Exception e) {
        IStatus status = new Status(Status.ERROR, PLUGIN_ID, 1, e.getMessage(), e);
        getDefault().getLog().log(status);
    }

    /**
     * @return The listener list attached to the plugin which is notified about 
     * ANY change in the NED model (ie. any change in any file) 
     */
    public NEDChangeListenerList getNEDModelChangeListenerList() {
        if (nedModelChangeListenerList == null)
            nedModelChangeListenerList = new NEDChangeListenerList();
        return nedModelChangeListenerList;
    }

    public void modelChanged(NEDModelEvent event) {
        // skip the event processing if te last serial is greater or equal. only newer
        // events should be processed. this prevent the processing of the same event multiple times
        if (lastEventSerial >= event.getSerial())
            return;
        else // process the event and remember this serial
            lastEventSerial = event.getSerial();

        if(nedModelChangeListenerList == null || !getNEDModelChangeListenerList().isEnabled())
            return;
        // forward to the listerList
        nedModelChangeListenerList.fireModelChanged(event);
        
        System.out.println("NED MODEL NOTIFY: "+event);
    }
}
