package com.simulcraft.test.gui.core;

import junit.framework.Assert;
import junit.framework.TestCase;

import org.eclipse.swt.SWTException;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.internal.Workbench;

import com.simulcraft.test.gui.access.Access;


public abstract class GUITestCase
    extends TestCase
{
	private final static boolean debug = false;

	public abstract class Test {
		public abstract void run() throws Exception;
	}

	public static class Step {
		public Object runAndReturn() throws Exception {
			return null;
		}

		public void run() throws Exception {
		}
	}

	/**
	 * Runs the given test code (the run() method of the Test object passed)
	 * in another thread. This is needed to be able to insert UI events
	 * into the event queue while the GUI is running a nested readAndDispatch()
	 * loop, e.g. inside modal dialogs.
	 */
	public void runTest(final Test test) throws Throwable {
        String testName = new Throwable().getStackTrace()[1].getMethodName();

        try {
	        Access.log(debug, "Starting test: " + testName);

	        // start the background thread which will query the gui and post events
	        Thread thread = new Thread(new Runnable() {
	            public void run() {
	                try {
	                    test.run();
	                }
	                catch (final Throwable t) {
	                    // we caught an exception from the background thread
	                    // we need to re-throw the exception from the UI thread, so the same handling takes place
	                    try {
    	                    Display.getDefault().syncExec(new Runnable() {
    	                        public void run() {
    	                            throw new TestException(t);
    	                        }
    	                    });
	                    }
	                    catch (Throwable th) {
	                        // void, ignore the exception just been thrown and got back here
	                        // because we are in the background thread
	                    }
	                }
	            }
	        }, testName);
	        // start the background thread immediately
	        thread.start();

	        // start processing gui events
    		while (thread.isAlive())
    			Display.getCurrent().readAndDispatch();
		}
        catch (Throwable t) {
            AnimationEffects.displayError(getInterestingCause(t), 3000);

            // KLUDGE: close all shells except the workbench window's shell
            // so that there are no hanging windows left open
            // SWT does not close open windows when exceptions pass through the event loop
            for (Shell shell : Display.getCurrent().getShells()) {
                if (shell != Workbench.getInstance().getActiveWorkbenchWindow().getShell())
                    shell.close();
            }

            // propagate the exception to JUnit
            throw t;
        }
		finally {
		    Access.log(debug, "Finished test: " + testName);
		}
	}

    private static Throwable getInterestingCause(Throwable t) {
        if (t instanceof TestException)
            return getInterestingCause(((TestException)t).getCause());
        else if (t instanceof SWTException)
            return getInterestingCause(((SWTException)t).throwable);
        else
            return t;
    }

	public static Object runStep(final Step step) {
		return runStepWithTimeout(-1, step);
	}

	/**
	 * Runs the given runnable in a synchronized way from the event dispatch thread.
	 * The idea is to run the runnable at least once and keep trying if there was an exception and
	 * there is still some remaining time to run otherwise throw the first exception caught from the runnable.
	 * 
	 * @param timeToRun -1 means run exactly once while positive values mean the runnable may be run multiple times
	 * @param step the runnable to be run from the event dispatch thread
	 * @return
	 */
	public static Object runStepWithTimeout(double timeToRun, final Step step) {
		if (Display.getCurrent() != null) {
			// if we are already in the UI thread, just plain do it
			try {
				step.run();
				return step.runAndReturn();
			} 
			catch (Throwable t) {
				throw new TestException(t);
			}
		}

		//System.out.print("sleep 1s to help debugging");  
		//Access.sleep(1);

		long begin = System.currentTimeMillis();
		boolean hasBeenRunOnce = false;
		final Object[] result = new Object[1];
		Throwable firstThrowable = null;
		final Throwable[] stepThrowables = new Throwable[1];

		while (!hasBeenRunOnce || System.currentTimeMillis() - begin < timeToRun * 1000) {
			if (hasBeenRunOnce)
				Access.sleep(0.5);
				
            Access.log(debug, hasBeenRunOnce ? "Rerunning step" : "Running step");
			stepThrowables[0] = null;

			Display.getDefault().syncExec(new Runnable() {
				public void run() {
					try {
						step.run();
						result[0] = step.runAndReturn();
					} 
					catch (Throwable t) {
						// just store the exception for later use and ignore it for now
						stepThrowables[0] = t;
						Access.log(debug, "Caught: " + t.getClass().getSimpleName() + ": " + t.getMessage());
					}
				}
			});

			Access.log(debug, "Waiting to processing events");

			waitUntilEventQueueBecomesEmpty();		

			// check if step has been run successfully and return
			if (stepThrowables[0] == null)
				return result[0];
			else if (firstThrowable == null)
				firstThrowable = stepThrowables[0];

			hasBeenRunOnce = true;
		}

	    Access.log(debug, "Step failed");
	    throw new TestException(firstThrowable);
	}

	public static void waitUntilEventQueueBecomesEmpty() {
		Assert.assertTrue("This method must not be called from the UI thread", Display.getCurrent()==null);
		while (PlatformUtils.hasPendingUIEvents())
			Thread.yield();
	}
}
