package org.omnetpp.python;

import org.eclipse.ui.IStartup;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.osgi.framework.BundleContext;

import py4j.ClientServer;
import py4j.reflection.ReflectionUtil;

/**
 * The activator class controls the plug-in life cycle
 */
public class Activator extends AbstractUIPlugin implements IStartup
{
    public static final String PLUGIN_ID = "org.omnetpp.python"; //$NON-NLS-1$

    private static Activator plugin;

    public String authToken = java.util.UUID.randomUUID().toString();
    public ClientServer clientServer;

    // This was initially added to allow the Python REPL (in the Console view) to
    // access the required classes for manipulating editors, sequence charts, etc.
    // However, it broke the registration of vector operations, because this way,
    // the VectorOp class was not accessible by chart script runner Python processes.
    // After removing this, we haven't managed to reproduce the original problem with
    // the Python REPL, so we are leaving it commented out for now. TODO: revisit
    /*
    static {
        ReflectionUtil.setClassLoadingStrategy(new PluginClassLoadingStrategy());
    }
    */

    public Activator() {
    }

    public static Activator getDefault() {
        return plugin;
    }

    public String getAuthToken() {
        return authToken;
    }

    @Override
    public void start(BundleContext context) throws Exception {
        super.start(context);
        plugin = this;
        clientServer = new ClientServer.ClientServerBuilder().authToken(authToken).javaPort(3071 + 653).pythonPort(0).build();
    }

    @Override
    public void stop(BundleContext context) throws Exception {
        plugin = null;
        if (clientServer != null)
            clientServer.shutdown();
        super.stop(context);
    }

    @Override
    public void earlyStartup() {
    }
}

