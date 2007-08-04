package org.omnetpp.test.gui;

import junit.framework.Assert;
import junit.framework.TestCase;

import org.eclipse.swt.widgets.Display;
import org.omnetpp.test.gui.access.Access;
import org.omnetpp.test.gui.access.WorkbenchAccess;


public abstract class TestBase extends TestCase {
	private final static boolean debug = true;
	
	private Throwable testThrowable;
	
	protected WorkbenchAccess workbenchAccess = new WorkbenchAccess();
	
	protected interface Test {
		public void run() throws Exception;
	}

	protected class Step {
		public Object runAndReturn() throws Exception {
			return null;
		}

		public void run() throws Exception {
		}
	}

	protected void runTest(final Test test) throws Throwable {
		testThrowable = null;

		Throwable throwable = new Throwable();
		String testName = throwable.getStackTrace()[1].getMethodName();
		
		if (debug)
			System.out.println("Starting test: " + testName);

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

		if (debug)
			System.out.println("Finished test: " + testName);

		if (testThrowable != null)
			throw testThrowable;
	}

	protected Object runStep(final Step step) {
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
	protected Object runStepWithTimeout(double timeToRun, final Step step) {
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
				
			if (debug) {
				if (hasBeenRunOnce)
					System.out.println("Rerunning step");
				else
					System.out.println("Running step");
			}

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
					}
				}
			});

			if (debug)
				System.out.println("Waiting to processing events");

			WorkbenchAccess.waitUntilEventQueueBecomesEmpty();		

			if (stepThrowables[0] == null)
				return result[0];
			else if (firstThrowable == null)
				firstThrowable = stepThrowables[0];

			hasBeenRunOnce = true;
		}

		if (debug)
			System.out.println("Step failed");

		// the special WorkspaceAdvisor will let the exception go up through readAndDispatch event loops and unwind the
		// stack until the top level test code is reached, see above
		stepThrowables[0] = firstThrowable;
		Display.getDefault().syncExec(new Runnable() {
			public void run() {
				if (debug)
					System.out.println("Rethrowing exception from step");

				// TODO: this does not hide popup menus since the code doesn't use try/catch/finally there and will not hide
				// the popup menu upon receiving an exception
				throw new TestException(stepThrowables[0]);
			}
		});

		// unreachable code
		Assert.assertTrue("Unreachable code reached", false);
		return null;
	}
}
