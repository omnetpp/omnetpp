package com.simulcraft.test.gui.core;

import junit.framework.Assert;
import junit.framework.TestCase;

import org.eclipse.swt.internal.win32.MSG;
import org.eclipse.swt.internal.win32.OS;
import org.eclipse.swt.widgets.Display;

import com.simulcraft.test.gui.access.Access;


public abstract class GUITestCase extends TestCase {
	private final static boolean debug = false;
	
	private Throwable testThrowable;
	
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
		testThrowable = null;

		Throwable throwable = new Throwable();
		String testName = throwable.getStackTrace()[1].getMethodName();
		
		Access.log(debug, "Starting test: " + testName);

		Thread thread = new Thread(new Runnable() {
			public void run() {
				try {
					test.run();
				}
				catch (Throwable t) {
					// we caught an exception from the test
					// we need to store it to be able to re-throw the exception at the end of the test code to notify JUnit about it
					testThrowable = t;
				}
			}
		}, testName);
		thread.start();

		while (thread.isAlive())
			Display.getCurrent().readAndDispatch();

		Access.log(debug, "Finished test: " + testName);

		if (testThrowable != null)
			throw testThrowable;
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
			catch (Exception e) {
				throw new TestException(e);
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

		// the special WorkspaceAdvisor will let the exception go up through readAndDispatch event loops and unwind the
		// stack until the top level test code is reached, see above
		stepThrowables[0] = new TestException(firstThrowable);
		Display.getDefault().syncExec(new Runnable() {
			public void run() {
			    Access.log(debug, "Rethrowing exception from step: " + stepThrowables[0]);

				// TODO: this does not hide popup menus since the code doesn't use try/catch/finally there and will not hide
				// the popup menu upon receiving an exception
				throw new TestException(stepThrowables[0]);
			}
		});

		// normal execution should have been already returned, this will terminate the background test thread
		Assert.assertTrue("Unreachable code reached", false);
		return null;
	}

	public static void waitUntilEventQueueBecomesEmpty() {
		Assert.assertTrue("This method must not be called from the UI thread", Display.getCurrent()==null);
		MSG msg = new MSG();
		while (OS.PeekMessage (msg, 0, 0, 0, OS.PM_NOREMOVE))
			Thread.yield();
	}
}
