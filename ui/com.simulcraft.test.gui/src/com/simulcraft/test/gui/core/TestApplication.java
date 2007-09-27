package com.simulcraft.test.gui.core;

import org.eclipse.core.runtime.Platform;
import org.eclipse.equinox.app.IApplicationContext;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.internal.ide.application.IDEApplication;
import org.eclipse.ui.internal.ide.application.IDEWorkbenchAdvisor;
import org.omnetpp.common.util.ReflectionUtils;

public class TestApplication extends IDEApplication
{
	private static final boolean debug = true;

    private static final String PROP_EXIT_CODE = "eclipse.exitcode"; //$NON-NLS-1$

    /**
     * A special return code that will be recognized by the launcher and used to
     * restart the workbench.
     */
    private static final Integer EXIT_RELAUNCH = new Integer(24);

    public Object start(IApplicationContext appContext) throws Exception {
        Display display = createDisplay();

        try {
            Shell shell = new Shell(display, SWT.ON_TOP);

            try {
                if (!(boolean)(Boolean)ReflectionUtils.invokeMethod(this, "checkInstanceLocation", shell)) {
                    Platform.endSplash();
                    return EXIT_OK;
                }
            } finally {
                if (shell != null) {
					shell.dispose();
				}
            }

            // create the workbench with this advisor and run it until it exits
            // N.B. createWorkbench remembers the advisor, and also registers
            // the workbench globally so that all UI plug-ins can find it using
            // PlatformUI.getWorkbench() or AbstractUIPlugin.getWorkbench()
            int returnCode = PlatformUI.createAndRunWorkbench(display, new IDEWorkbenchAdvisor() {
            	@Override
            	public void eventLoopException(Throwable exception) {
            		if (exception instanceof TestException ||
            		    exception.getCause() instanceof TestException ||
            		    (exception.getCause() != null && exception.getCause().getCause() instanceof TestException))
            		{
            			if (debug)
            				System.out.println("Rethrowing exception from event loop");
 
            			throw (RuntimeException)exception;
            		}

            		super.eventLoopException(exception);
            	}
            });

            // the workbench doesn't support relaunch yet (bug 61809) so
            // for now restart is used, and exit data properties are checked
            // here to substitute in the relaunch return code if needed
            if (returnCode != PlatformUI.RETURN_RESTART) {
				return EXIT_OK;
			}

            // if the exit code property has been set to the relaunch code, then
            // return that code now, otherwise this is a normal restart
            return EXIT_RELAUNCH.equals(Integer.getInteger(PROP_EXIT_CODE)) ? EXIT_RELAUNCH : EXIT_RESTART;
        }
        finally {
            if (display != null) {
				display.dispose();
			}
        }
    }
}
