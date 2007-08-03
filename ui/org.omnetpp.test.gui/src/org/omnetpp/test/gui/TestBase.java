package org.omnetpp.test.gui;

import junit.framework.TestCase;

import org.eclipse.swt.widgets.Display;
import org.omnetpp.test.gui.access.WorkbenchAccess;


public abstract class TestBase extends TestCase {
	private Throwable testThrowable;
	
	protected WorkbenchAccess workbenchAccess = new WorkbenchAccess();
	
	protected class Test {
		public void run() throws Exception {
		}
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

		if (testThrowable != null)
			throw testThrowable;
	}

	protected Object runStep(final Step step) {
		final Object[] result = new Object[1];

		Display.getDefault().syncExec(new Runnable() {
			public void run() {
				try {
					step.run();
					result[0] = step.runAndReturn();
				} 
				catch (Throwable t) {
					// if we caught an exception we need to wrap it to match the runnable interface
					// the SWT syncExec will additionally wrap the exception into an SWT something but that doesn't matter much
					// the special WorkspaceAdvisor will let the exception go up through readAndDispatch event loops and unwind the
					// stack until the test code is reached, see above
					throw new TestException(t);
				}
			}
		});
		
		workbenchAccess.waitUntilEventQueueBecomesEmpty();		
		
		return result[0];
	}
}
