package org.omnetpp.test.gui;

import org.eclipse.swt.widgets.Display;

/**
 * Aspect for manipulating GUI stuff from a background thread.
 * 
 * @author Andras
 */
public aspect DoInUIThread {

	/**
	 * Surround every public method of the UI "Access" classes 
	 * (org.omnetpp.test.gui.access.*) with Display.syncExec() calls,
	 * because they operate on SWT widgets, the UI event queue, etc,
	 * which would cause Illegal Thread Access when done from 
	 * a background thread.
	 * 
	 * When the code is already executing in the UI thread, just
	 * proceed with the call.
	 */
	//TODO: ignore this advice when a @NoSyncExec annotation is present
	//TODO: not for inner classes i.e. IPredicate.matches()!!!  within? withincode? cflow? cflowbelow?
	//TODO: make hardcoded timeout parameterizable
	Object around(): execution(public * org.omnetpp.test.gui.access.*.*(..)) {
	    String method = thisJoinPointStaticPart.getSignature().getDeclaringType().getName() + "." + thisJoinPointStaticPart.getSignature().getName();
	    if (Display.getCurrent() != null) {
	    	System.out.println("AJ: doing " + method + " (already in UI thread)");
	    	return proceed();
	    }
	    else {
	    	System.out.println("AJ: doing in UI thread: " + method);
	    	return TestBase.runStepWithTimeout(5, new TestBase.Step() {
	    		public Object runAndReturn() {
	    			return proceed();
	    		}
	    	});
	    }
	}

	/**
	 * In test cases, the test*() methods should be run in a background thread so that
	 * it can run independent of the UI.
	 */
	//TODO: for all classes that extend junit.framework.TestCase, not only inifile tests
	void around(final TestBase t): target(t) && execution(public void org.omnetpp.test.gui.inifileeditor.*.test*()) {
	    String method = thisJoinPointStaticPart.getSignature().getDeclaringType().getName() + "." + thisJoinPointStaticPart.getSignature().getName();
	    System.out.println("AJ: running test case: " + method);
	    try {
		    TestBase.Test testRunnable = t.new Test() {
		    	public void run() throws Exception {
		    		proceed(t);
		    	}
		    };
	    	t.runTest(testRunnable);
	    } 
	    catch (Throwable e) {
	    	throw new TestException(e);
	    }
	}
	
	
}
