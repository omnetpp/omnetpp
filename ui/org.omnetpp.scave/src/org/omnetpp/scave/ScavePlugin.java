/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave;

import java.io.File;
import java.io.IOException;
import java.net.URISyntaxException;
import java.util.HashSet;

import org.apache.commons.lang3.SystemUtils;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.NullProgressMonitor;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.resource.ImageRegistry;
import org.eclipse.swt.graphics.Image;
import org.eclipse.ui.ISharedImages;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.omnetpp.scave.pychart.PythonProcessPool;
import org.osgi.framework.BundleContext;
import org.python.pydev.ast.interpreter_managers.InterpreterInfo;
import org.python.pydev.ast.interpreter_managers.InterpreterManagersAPI;
import org.python.pydev.core.IInterpreterInfo;
import org.python.pydev.core.IInterpreterManager;

/**
 * The activator class controls the plug-in life cycle
 */
public class ScavePlugin extends AbstractUIPlugin {

    // The plug-in ID
    public static String PLUGIN_ID;

    // The shared instance
    private static ScavePlugin plugin;

    /**
     * The constructor
     */
    public ScavePlugin() {
        plugin = this;
    }

    /*
     * (non-Javadoc)
     * @see org.eclipse.ui.plugin.AbstractUIPlugin#start(org.osgi.framework.BundleContext)
     */
    public void start(BundleContext context) throws Exception {
        super.start(context);
        PLUGIN_ID = getBundle().getSymbolicName();

        ensurePyDevInterpreterConfigured();
    }

    // source: https://stackoverflow.com/a/32827512/635587
    public static String findExecutableOnPath(String name) {
        for (String dirname : System.getenv("PATH").split(File.pathSeparator)) {
            File file = new File(dirname, name);
            if (file.isFile() && file.canExecute()) {
                return file.getAbsolutePath();
            }
        }
        throw new AssertionError("should have found the executable");
    }

    static public void ensurePyDevInterpreterConfigured() {

        // This is here to configure a default Python interpreter for PyDev if there are none.
        // Without a configured interpreter, PyDev places an error marker into every project that
        // has a PyDev nature. We tried doing this by setting a default value for the
        // PYTHON_INTERPRETER_PATH ("INTERPRETER_PATH_NEW") preference variable of org.python.pydev
        // in the plugin_customization.ini file of org.omnetpp.main, but that did not have an effect,
        // because PyDev reads this variable only from the INSTANCE preference scope, and does not fall
        // back to the CONFIGURATION scope, where we set it. If it did, that would be a better solution.
        //
        // This way, the error marker still appears until the Scave plugin is loaded (for example, when
        // an .anf file is opened), but it then goes away as a result of the following lines.
        //
        // If a .py file is opened first, while the Scave plugin is not loaded, the autoconfiguration
        // dialogs of PyDev will pop up, and let the user configure an interpreter.
        //
        // We do not use anything from this interpreter configuration at the moment, we just need it
        // to make the error marker disappear, and let PyDev be happy.
        IInterpreterManager manager = InterpreterManagersAPI.getPythonInterpreterManager();
        if (manager.getInterpreterInfos().length == 0) {

            String pythonExecutable = findExecutableOnPath(SystemUtils.IS_OS_WINDOWS ? "python3.exe" : "python3");
            System.out.println("found python3: " + pythonExecutable);

            IInterpreterInfo info = manager.createInterpreterInfo(pythonExecutable, new NullProgressMonitor(), false);
            info.setName("Default Python 3");

            String locationBase = "NULL";
            try {
                locationBase = new File(PythonProcessPool.class.getProtectionDomain().getCodeSource().getLocation().toURI().getPath()).getAbsolutePath();
            }
            catch (URISyntaxException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
            String location = locationBase + File.separator + "python"; // in release builds

            try {
                File f = new File(location);
                if (f.exists()) {
                    location = f.getCanonicalPath();
                    ((InterpreterInfo) info).libs.add(location);
                    ((InterpreterInfo) info).addPredefinedCompletionsPath(location);
                }
            }
            catch (IOException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }

            manager.setInfos(new IInterpreterInfo[] { info }, new HashSet<String>(), new NullProgressMonitor());
        }
        else {
            System.out.println("At least one Python interpreter is configured for PyDev, let's leave it at that, assuming it's fine.");
        }

        /*
        IInterpreterManager manager = InterpreterManagersAPI.getPythonInterpreterManager();
        if (manager.getInterpreterInfos().length == 0) {
            //IInterpreterInfo info = manager.createInterpreterInfo("/usr/bin/python3", new NullProgressMonitor(), false);
            //info.setName("Default Python 3");
            // IInterpreterInfo[] infos = new IInterpreterInfo[] { info };

            IInterpreterInfo[] infos = manager.getInterpretersFromPersistedString("<xml><name>Python 3</name><version>3</version><executable>/usr/bin/python3</executable></xml>");
            manager.setInfos(infos, new HashSet<String>(), new NullProgressMonitor());
        }

        IInterpreterManager manager = InterpreterManagersAPI.getPythonInterpreterManager();
        if (manager.getInterpreterInfos().length == 0) {
            AutoConfigMaker m = new AutoConfigMaker(IInterpreterProviderFactory.InterpreterType.PYTHON, true, null, null);
            m.autoConfigSingleApply(null);
        }
        */

    }
    /*
     * (non-Javadoc)
     * @see org.eclipse.ui.plugin.AbstractUIPlugin#stop(org.osgi.framework.BundleContext)
     */
    public void stop(BundleContext context) throws Exception {
        plugin = null;
        super.stop(context);
    }

    /**
     * Returns the shared instance
     *
     * @return the shared instance
     */
    public static ScavePlugin getDefault() {
        return plugin;
    }

    /**
     * Returns an image descriptor for the image file at the given
     * plug-in relative path
     *
     * @param path the path
     * @return the image descriptor
     */
    public static ImageDescriptor getImageDescriptor(String path) {
        return imageDescriptorFromPlugin(PLUGIN_ID, path);
    }

    /**
     * Creates an image. IMPORTANT: The image is NOT cached! Callers
     * are responsible for disposal of the image.
     */
    public static Image getImage(String path) {
        ImageDescriptor id = getImageDescriptor(path);
        if (id == null) {
            IllegalArgumentException e = new IllegalArgumentException("Cannot load image from: "+path);
            logError(e);
            throw e;
        }
        return id.createImage();
    }

    /**
     * Like getImage(), but the image gets cached in an internal image registry,
     * so clients do not need to (moreover, must not) dispose of the image.
     */
    public static Image getCachedImage(String path) {
        ImageRegistry imageRegistry = getDefault().getImageRegistry();
        Image image = imageRegistry.get(path);
        if (image==null) {
            image = getImage(path);
            imageRegistry.put(path, image);
        }
        return image;
    }

    /**
     * Returns a platform shared image. Note: keys can be taken from IMG_*
     * constants in the ISharedImages class, e.g. ISharedImages.IMG_TOOL_COPY.
     */
    public static Image getSharedImage(String key) {
        ISharedImages sharedImages = PlatformUI.getWorkbench().getSharedImages();
        return sharedImages.getImage(key);
    }

    /**
     * Returns a platform shared image. Note: keys can be taken from IMG_*
     * constants in the ISharedImages class, e.g. ISharedImages.IMG_TOOL_COPY.
     */
    public static ImageDescriptor getSharedImageDescriptor(String key) {
        ISharedImages sharedImages = PlatformUI.getWorkbench().getSharedImages();
        return sharedImages.getImageDescriptor(key);
    }

    public static void logError(Throwable exception) {
        logError(exception.toString(), exception);
    }

    public static void logError(String message, Throwable exception) {
        log(getErrorStatus(0, message, exception));
    }

    public static void log(IStatus status) {
        if (status != null) {
            if (plugin != null) {
                plugin.getLog().log(status);
            }
            else {
                if (status.getMessage() != null)
                    System.err.println(status.getMessage());
                if (status.getException() != null)
                    status.getException().printStackTrace();
            }
        }
    }

    public static IStatus getWarningStatus(String message) {
        return getWarningStatus(0, message);
    }

    public static IStatus getWarningStatus(int code, String message) {
        return getStatus(IStatus.WARNING, code, message, null);
    }

    public static IStatus getErrorStatus(Throwable exception) {
        return getErrorStatus(0, exception.getLocalizedMessage(), exception);
    }

    public static IStatus getErrorStatus(int errorCode, String message, Throwable exception) {
        return getStatus(IStatus.ERROR, errorCode, message, exception);
    }

    public static IStatus getStatus(int severity, int code, String message, Throwable exception) {
        return new Status(severity, PLUGIN_ID, code, message, exception);
    }
}
